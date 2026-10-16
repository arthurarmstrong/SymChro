#include "nanobind/nanobind.h"
#include "nanobind/stl/chrono.h"
#include "symchro.h"
#include <chrono>
#include <string>

namespace nb = nanobind;

using namespace nb::literals;

namespace Sym {

template <typename T> T create_duration(long duration) { return T(duration); }

NB_MODULE(pysymchro, m) {

  nb::class_<SymChro>(m, "symchro")
      .def(nb::init<float, float, float>(), "P"_a = DEFAULT_P,
           "I"_a = DEFAULT_I, "D"_a = DEFAULT_D)
      .def("reset", &SymChro::reset)
      .def("pid_update_ns", &SymChro::pid_update<std::chrono::nanoseconds>)
      .def("get_sleep_required_ns",
           &SymChro::get_sleep_required<std::chrono::nanoseconds>)
      .def("pid_update_ms", &SymChro::pid_update<std::chrono::milliseconds>)
      .def("get_sleep_required_ms",
           &SymChro::get_sleep_required<std::chrono::milliseconds>)
      .def("pid_update_secs", &SymChro::pid_update<std::chrono::seconds>)
      .def("get_sleep_required_secs",
           &SymChro::get_sleep_required<std::chrono::seconds>)
      .def("get", &SymChro::get, nb::arg("key"))
      .def("set", &SymChro::set, nb::arg("key"), nb::arg("val"))
      .def("start_work", &SymChro::startWork)
      .def("end_work", &SymChro::endWork)
      .def("check_point", &SymChro::checkPoint)
      .def("set_debug", &SymChro::setDebug, "val"_a)
      .def_prop_ro("current_ratio", &SymChro::getCurrentRatio);

  m.def("nanoseconds", &create_duration<std::chrono::nanoseconds>,
        nb::arg("Duration"));

  m.def("microseconds", &create_duration<std::chrono::microseconds>,
        nb::arg("Duration"));

  m.def("milliseconds", &create_duration<std::chrono::milliseconds>,
        nb::arg("Duration"));

  m.def("seconds", &create_duration<std::chrono::seconds>, nb::arg("Duration"));
}
}; // namespace Sym