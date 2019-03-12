#include "Car.h"

CAN::CAN(int Pin, byte Speed)
{
    this->initialised = false;
    this->Pin = Pin;
    this->Speed = Speed;
    this->interface = new MCP_CAN(this->Pin);    
}


bool CAN::Init()
{
    this->initialised = false;
    unsigned long timeout = millis() + 1000;
    while (CAN_OK != this->interface->begin(this->Speed)) 
    {
        delay(100);
        if (timeout < millis()) return false;
    }
    this->initialised = true;
    return true;
}

bool CAN::Transmit(CANMessage* message)
{
    if (!this->initialised) return false;
    return this->interface->sendMsgBuf(message->id, message->isExtended ? 1:0, message->length, message->data) == CAN_OK; 
}
