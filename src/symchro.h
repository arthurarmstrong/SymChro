#include <chrono>
#include <deque>
#include <map>
#include <ratio>
#include <string>

namespace Sym {

inline constexpr float DEFAULT_P = 0.5;
inline constexpr float DEFAULT_I = 0.001;
inline constexpr float DEFAULT_D = 0.1;

class SymChro {

public:
  SymChro(float = DEFAULT_P, float = DEFAULT_I, float = DEFAULT_D);

  void reset();
  float get(std::string);
  void set(std::string, float);

  template <typename T> T get_sleep_required(const T &, const int &);
  template <typename T> T pid_update(const T &, const T &);

  void startWork();
  void endWork();
  void checkPoint();
  void simTick(const std::chrono::nanoseconds &);
  void setDebug(bool);
  float getCurrentRatio();

private:
  std::chrono::nanoseconds integral_;
  std::chrono::nanoseconds lastErrMeasured_;
  float P_;
  float I_;
  float D_;
  std::map<std::string, float *> property_map_{
      {"P", &P_}, {"I", &I_}, {"D", &D_}};
  float currentRatio_;
  std::chrono::system_clock systemClock_;
  std::chrono::steady_clock steadyClock_;

  std::chrono::time_point<std::chrono::steady_clock> startedWork_;
  std::chrono::time_point<std::chrono::steady_clock> lastCheckPoint_;
  std::chrono::nanoseconds workTime_;

  std::deque<std::chrono::nanoseconds> lastTenCycles_;
  std::deque<std::chrono::nanoseconds> lastTenSimSteps_;
  std::chrono::nanoseconds simTime_;

  std::chrono::nanoseconds lastSleep_;
  std::chrono::nanoseconds sleepShouldBe_;

  bool debug_;
};

}; // namespace Sym