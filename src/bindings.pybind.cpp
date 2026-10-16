
#include "pybind11/chrono.h"
#include "pybind11/pybind11.h"
#include "symchro.h"
#include <chrono>
#include <string>
namespace py = pybind11;

namespace Sym {

template <typename T> T create_duration(long duration) { return T(duration); }

PYBIND11_MODULE(pysymchro, m, py::mod_gil_not_used()) {

  py::class_<SymChro>(m, "symchro")
      .def(py::init<int, float, float, float>(), py::arg("Ref"),
           py::arg_v("P", DEFAULT_P, "Proportional gain"),
           py::arg_v("I", DEFAULT_I, "Integral gain"),
           py::arg_v("D", DEFAULT_D, "Derivative gain"))
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
      .def("get", &SymChro::get, py::arg("key"))
      .def("set", &SymChro::set, py::arg("key"), py::arg("val"))
      .def("start_work", &SymChro::startWork)
      .def("end_work", &SymChro::endWork);

  m.def("nanoseconds", &create_duration<std::chrono::nanoseconds>,
        py::arg("Duration"));

  m.def("microseconds", &create_duration<std::chrono::microseconds>,
        py::arg("Duration"));

  m.def("milliseconds", &create_duration<std::chrono::milliseconds>,
        py::arg("Duration"));

  m.def("seconds", &create_duration<std::chrono::seconds>, py::arg("Duration"));
}

}; // namespace Sym