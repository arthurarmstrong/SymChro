#include "symchro.h"
#include <chrono>
#include <ctime>
#include <deque>
#include <iostream>
#include <ratio>
#include <thread>

namespace Sym {

SymChro::SymChro(float P, float I, float D)
    : P_(P), I_(I), D_(D), integral_(0), lastErrMeasured_(0), systemClock_(),
      steadyClock_(), lastCheckPoint_(steadyClock_.now()), lastTenCycles_(10),
      lastTenSimSteps_(10), simTime_(0), lastSleep_(0), sleepShouldBe_(0),
      debug_(false) {};

void SymChro::reset() { this->integral_ = std::chrono::nanoseconds(0); }

template <typename T> T average(const std::deque<T> &q) {
  T total(0);

  for (auto i : q) {
    total += i;
  }

  return total / q.size();
}

void SymChro::simTick(const std::chrono::nanoseconds &dtSim) {
  simTime_ += std::chrono::duration_cast<std::chrono::nanoseconds>(dtSim);
}

float SymChro::getCurrentRatio() { return currentRatio_; }

void SymChro::setDebug(bool val) { debug_ = val; }

template <typename T>
T SymChro::get_sleep_required(const T &dtSim, const int &desiredRatio) {

  static std::chrono::time_point<std::chrono::steady_clock> now;

  static std::chrono::milliseconds wallTime(0);

  static std::chrono::nanoseconds dtSim_;

  dtSim_ = std::chrono::duration_cast<std::chrono::nanoseconds>(dtSim);

  static std::chrono::time_point<std::chrono::steady_clock> startTime =
      steadyClock_.now();

  static std::chrono::nanoseconds errMeasured;

  now = steadyClock_.now();

  lastTenSimSteps_.push_back(dtSim_);

  // increment the sim clock
  simTick(dtSim_);

  sleepShouldBe_ =
      average(lastTenSimSteps_) / desiredRatio - average(lastTenCycles_);

  if (sleepShouldBe_ <= std::chrono::nanoseconds(0)) {
    sleepShouldBe_ = std::chrono::nanoseconds(0);
  }

  currentRatio_ =
      average(lastTenSimSteps_) / (average(lastTenCycles_) + lastSleep_);

  lastSleep_ = sleepShouldBe_;

  if (debug_) {
    std::cout
        << "Sim Time : "
        << std::chrono::duration_cast<std::chrono::seconds>(simTime_).count()
        << ", Wall Time: "
        << std::chrono::duration_cast<std::chrono::seconds>(now - startTime)
               .count()
        << ", Current Ratio: " << currentRatio_ << std::endl;
  }

  return std::chrono::duration_cast<T>(sleepShouldBe_);
}

float SymChro::get(std::string s) {

  for (auto i : property_map_) {
    if (i.first == s) {
      return *i.second;
    }
  }

  throw(-1);
}

void SymChro::set(std::string s, float val) {
  for (auto i : property_map_) {
    if (i.first == s) {
      *i.second = val;
      return;
    }
  }
  throw(-1);
}

template <typename T> T SymChro::pid_update(const T &errMeasured, const T &dt) {
  static T derrdt;

  this->integral_ += errMeasured;

  derrdt = errMeasured - std::chrono::duration_cast<T>(this->lastErrMeasured_);

  this->lastErrMeasured_ = errMeasured;

  auto ret = std::chrono::duration_cast<T>(
      -this->P_ * errMeasured - this->I_ * this->integral_ - this->D_ * derrdt);
  return ret;
}

void SymChro::startWork() { startedWork_ = steadyClock_.now(); }

void SymChro::checkPoint() {

  auto now = steadyClock_.now();
  lastTenCycles_.push_back((now - lastCheckPoint_) - lastSleep_);
  lastCheckPoint_ = now;
}

void SymChro::endWork() {
  lastTenCycles_.push_back(steadyClock_.now() - startedWork_);
}

template std::chrono::nanoseconds
SymChro::get_sleep_required<std::chrono::nanoseconds>(
    const std::chrono::nanoseconds &, const int &);

template std::chrono::milliseconds
SymChro::get_sleep_required<std::chrono::milliseconds>(
    const std::chrono::milliseconds &, const int &);

template std::chrono::microseconds
SymChro::get_sleep_required<std::chrono::microseconds>(
    const std::chrono::microseconds &, const int &);

template std::chrono::seconds
SymChro::get_sleep_required<std::chrono::seconds>(const std::chrono::seconds &,
                                                  const int &);

template std::chrono::nanoseconds
SymChro::pid_update<std::chrono::nanoseconds>(const std::chrono::nanoseconds &,
                                              const std::chrono::nanoseconds &);

template std::chrono::milliseconds
SymChro::pid_update<std::chrono::milliseconds>(
    const std::chrono::milliseconds &, const std::chrono::milliseconds &);

template std::chrono::microseconds
SymChro::pid_update<std::chrono::microseconds>(
    const std::chrono::microseconds &, const std::chrono::microseconds &);

template std::chrono::seconds
SymChro::pid_update<std::chrono::seconds>(const std::chrono::seconds &,
                                          const std::chrono::seconds &);

}; // namespace Sym