#include "symchro.h"
#include <chrono>
#include <ctime>
#include <deque>
#include <iostream>
#include <thread>

namespace Sym {

SymChro::SymChro(int ref, float P, float I, float D)
    : P_(P), I_(I), D_(D), integral_(0), lastErrMeasured_(0), systemClock_(),
      steadyClock_(), lastTenCycles_(10), lastTenSimSteps_(10), lastSleep_(0) {

      };

void SymChro::reset() { this->integral_ = std::chrono::nanoseconds(0); }

template <typename T = std::chrono::milliseconds>
void tick(T &simTime, const T &dt) {
  simTime += dt;
};

template <typename T> T average(const std::deque<T> &q) {
  T total(0);

  for (auto i : q) {
    total += i;
  }

  return total / q.size();
}

template <typename T>
T SymChro::get_sleep_required(const T &dtSim, const int &desiredRatio) {

  static std::chrono::time_point<std::chrono::system_clock> now;

  static std::chrono::milliseconds simTime(0);
  static std::chrono::milliseconds wallTime(0);

  std::chrono::time_point<std::chrono::system_clock> lastTWall =
      systemClock_.now();

  static std::chrono::time_point<std::chrono::system_clock> startTime =
      systemClock_.now();

  T sleepShouldBe;
  static std::chrono::nanoseconds errMeasured;

  T sleepFor;

  now = systemClock_.now();

  lastTenSimSteps_.push_back(dtSim);

  // increment the sim clock
  tick(simTime, std::chrono::duration_cast<std::chrono::milliseconds>(dtSim));

  sleepShouldBe =
      std::chrono::duration_cast<T>(average(lastTenSimSteps_)) / desiredRatio -
      std::chrono::duration_cast<T>(average(lastTenCycles_));

  currentRatio_ =
      average(lastTenSimSteps_) / (average(lastTenCycles_) + sleepShouldBe);

  lastSleep_ = sleepShouldBe;

  wallTime += std::chrono::duration_cast<std::chrono::milliseconds>(workTime_ +
                                                                    lastSleep_);

  std::cout << "Sim Time : "
            << std::chrono::duration_cast<std::chrono::seconds>(simTime).count()
            << ", Wall Time: "
            << std::chrono::duration_cast<std::chrono::seconds>(
                   systemClock_.now() - startTime)
                   .count()
            << ", Current Ratio: " << currentRatio_
            << ", Sleepshouldbe: " << sleepShouldBe.count() << std::endl;

  return sleepFor;
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

void SymChro::startWork() { startedWork_ = systemClock_.now(); }

void SymChro::endWork() {
  lastTenCycles_.push_back(systemClock_.now() - startedWork_);
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