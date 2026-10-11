#include <chrono>
class SymChro {

public: 

SymChro(int,float=0.5,float=0.001,float=0.1);

std::chrono::nanoseconds update(std::chrono::nanoseconds , std::chrono::nanoseconds );
void reset();
bool loop();

private:

int ref_;
std::chrono::nanoseconds refErr_;
std::chrono::nanoseconds integral_;
std::chrono::nanoseconds lastErrMeasured_;
float P_;
float I_;
float D_;

};