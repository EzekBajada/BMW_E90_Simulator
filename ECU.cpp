#include "Car.h"

ECU::ECU(Car* car, CAN* transmitter)
{
    this->transmitter = transmitter;
    this->car = car;  
}

ECU::~ECU()
{
}
