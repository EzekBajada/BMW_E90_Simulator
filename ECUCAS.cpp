#include "Car.h"

ECUCAS::ECUCAS(Car* car, CAN* transmitter):ECU(car, transmitter)
{
    MessageTimer130 = 0;
    MessageTimer26E = 2; 
    Counter130 = 50;
    
    KeyInSlot = true;
    IgnitionOn = true;    
    EngineRunning = true;
    ClutchPressed = false;    
}

ECUCAS::~ECUCAS()
{
}

//Ignition and Key Status
void ECUCAS::SendMessage130() 
{    
    CANMessage* message = new CANMessage(0x130, 5);      
    //00h engine off
    //40h engine off (key is being inserted)
    //41h engine off (key in position 1 also this value when engine is stopped)
    //45h engine running. (also key to position 2)
    //55h engine is turning on (from key or auto stop start)    
    message->data[0] = 0x00;    
    if (IgnitionOn && !EngineRunning) message->data[0] = 0x41;
    if (IgnitionOn && EngineRunning) message->data[0] = 0x45;          
    //00h key not in slot
    //40h key in slot
    message->data[1] = KeyInSlot ? 0x42 : 0x00;    
    if (KeyInSlot || EngineRunning)
        message->data[2] = ClutchPressed ? 0x60 : 0x20;
    else 
        message->data[2] = 0xD0;        
    if (EngineRunning)
        message->data[2] = message->data[2] | 0x01;
    else
    {
        if (IgnitionOn) 
            message->data[2] = message->data[2] | 0x02;
        else
            message->data[2] = message->data[2] | 0x00;                         
    }    
    if (EngineRunning)
        message->data[3] = 0x80;
    else
        message->data[3] = 0x00;    
    message->data[3] = message->data[3] | 0x0F;    
    message->data[4] = (byte) (Counter130 / 5.49f); // 1.4 second counter    
    Counter130 += (millis() - MessageTimer130);
    if (Counter130 > 1400) Counter130 -= 1400; 
    transmitter->Transmit(message); 
    delete message;
    MessageTimer130 = millis();  
}

// VIN Number (A537437)
void ECUCAS::SendMessage380()
{
    CANMessage* message = new CANMessage(0x380, 7);
    message->data[0] = 0x41;  
    message->data[1] = 0x35; 
    message->data[2] = 0x33; 
    message->data[3] = 0x37; 
    message->data[4] = 0x34; 
    message->data[5] = 0x33; 
    message->data[6] = 0x37;           
    transmitter->Transmit(message); 
    delete message;
}

//Ignition Status
void ECUCAS::SendMessage26E()
{     
    CANMessage* message = new CANMessage(0x26E, 8);       
    message->data[0] = EngineRunning ? 0x40 : 0x00;
    message->data[1] = IgnitionOn || EngineRunning ? 0x40 : 0x00;
    message->data[2] = IgnitionOn || EngineRunning ? 0x7F : 0x3F;
    message->data[3] = 0x50;
    message->data[4] = 0xFF;
    message->data[5] = 0xFF;
    message->data[6] = 0xFF;
    message->data[7] = 0xFF;
    transmitter->Transmit(message); 
    delete message;
    MessageTimer26E = millis();  
}

void ECUCAS::Run()
{
    if(millis() - MessageTimer26E >= 200) SendMessage26E();        
    if(millis() - MessageTimer130 >= 100) SendMessage130(); 
}
