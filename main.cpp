#include "src/symchro.h"
#include <chrono>
#include <iostream>

int main(int argc, char **argv) {

  int ratio = 40;

  if (argc > 1) {
    ratio = std::stoi(argv[1]);
  };

  std::cout << "Setting ratio to: " << ratio << "\n";

  Sym::SymChro sym{ratio, .5, .001, 0.1};

  sym.get_sleep_required(std::chrono::nanoseconds(200), 40.);

  std::cout << "Done";
}
