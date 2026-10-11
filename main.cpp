#include <iostream>
#include "src/symchro.h"

int main(int argc, char** argv){

    int ratio = 40;

    if (argc >1) {
        ratio = std::stoi(argv[1]);
    };

    std::cout << "Setting ratio to: " << ratio << "\n";

    SymChro sym {ratio,.5,.001,0.1};

    sym.loop();

    std::cout << "Done";
}
