#include "symchro.h"
#include <chrono>
#include <deque>
#include <iostream>
#include <thread>

SymChro::SymChro(int ref, float P, float I, float D)
    : ref_(ref), P_(P), I_(I), D_(D), refErr_(0), integral_(0),
      lastErrMeasured_(0) {

      };

std::chrono::nanoseconds SymChro::update(std::chrono::nanoseconds errMeasured,
                                         std::chrono::nanoseconds dt) {
  this->integral_ += errMeasured;
  static std::chrono::nanoseconds derrdt = errMeasured - this->lastErrMeasured_;
  this->lastErrMeasured_ = errMeasured;

  auto ret =
      -this->P_ * errMeasured - this->I_ * this->integral_ - this->D_ * derrdt;

  return std::chrono::duration_cast<std::chrono::nanoseconds>(ret);
}

void SymChro::reset() { this->integral_ = std::chrono::nanoseconds(0); }

void tick(std::chrono::milliseconds &simTime,
          const std::chrono::milliseconds &dt) {
  simTime += dt;
};

std::chrono::nanoseconds
average(const std::deque<std::chrono::nanoseconds> &q) {
  std::chrono::nanoseconds total(0);

  for (auto i : q) {
    total += i;
  }

  return total / q.size();
}

bool SymChro::loop() {

  std::chrono::system_clock clock;
  const std::chrono::milliseconds simDt(200);
  std::chrono::milliseconds simTime(0);
  std::chrono::milliseconds wallTime(0);

  std::chrono::time_point<std::chrono::system_clock> lastTWall = clock.now();
  std::chrono::time_point<std::chrono::system_clock> startTime = clock.now();
  std::chrono::nanoseconds tCycle;

  float currentRatio;

  std::deque<std::chrono::nanoseconds> lastTenCycles(10);
  std::chrono::time_point<std::chrono::system_clock> now;
  std::chrono::nanoseconds sleepFor(100);
  std::chrono::nanoseconds sleepShouldBe;
  std::chrono::nanoseconds errMeasured;

  while (true) {
    now = clock.now();

    tCycle = now - lastTWall;

    lastTenCycles.emplace_back(tCycle);

    // sim work
    tick(simTime, simDt);

    currentRatio = lastTenCycles.size() *
                   std::chrono::duration_cast<std::chrono::nanoseconds>(simDt) /
                   average(lastTenCycles);

    sleepShouldBe =
        std::chrono::duration_cast<std::chrono::nanoseconds>(simDt) / ref_ -
        tCycle - sleepFor;

    errMeasured = sleepFor - sleepShouldBe;

    sleepFor = std::max(std::chrono::nanoseconds(0),
                        this->update(errMeasured, tCycle));

    wallTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        clock.now() - startTime);

    std::this_thread::sleep_for(sleepFor);

    lastTWall = now;

    std::cout << "Sim Time : " << simTime.count()
              << ", Wall Time: " << wallTime.count()
              << ", Current Ratio: " << currentRatio
              << ", Slept: " << sleepFor.count() << std::endl;
  };
}