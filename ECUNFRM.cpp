#include "Car.h"

ECUNFRM::ECUNFRM(Car* car, CAN* transmitter):ECU(car, transmitter)
{
    MessageTimer21A = 26;
    
    BrakeLight = false;
    SideLight = true;
    DippedBeam = true;
    MainBeam = true;
    FrontFogLights = true;
    RearFogLights = true;
    InteriorLight = false;
    ExteriorLight = false;
}

ECUNFRM::~ECUNFRM()
{
}

//Light Status
void ECUNFRM::SendMessage21A()
{
    CANMessage* message = new CANMessage(0x21A, 3);  
    for(int i = 0; i < 3; i++) message->data[i] = 0;     
    if (BrakeLight) message->data[0] = message->data[0] | 128;
    if (SideLight) message->data[0] = message->data[0] | 4;
    if (DippedBeam) message->data[0] = message->data[0] | 1;
    if (MainBeam) message->data[0] = message->data[0] | 2;
    if (FrontFogLights) message->data[0] = message->data[0] | 32;
    if (RearFogLights) message->data[0] = message->data[0] | 64;    
    if (InteriorLight) message->data[1] = message->data[1] | 16;
    if (ExteriorLight) message->data[1] = message->data[1] | 2;    
    message->data[2] = 0xF7;
    transmitter->Transmit(message); 
    delete message; 
    MessageTimer21A = millis(); 
}


void ECUNFRM::Run()
{
    if(millis() - MessageTimer21A >= 5000) SendMessage21A();
}
