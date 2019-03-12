#include "Car.h"

ECUOthers::ECUOthers(Car* car, CAN* transmitter):ECU(car, transmitter)
{
    MessageTimer335 = 30;
    Counter335 = 12;
}

ECUOthers::~ECUOthers()
{
}

//Set Time & Date
void ECUOthers::SendMessage39E() 
{
    CANMessage* message = new CANMessage(0x39E, 8);    
    message->data[0] = 12; // Hours
    message->data[1] = 30; // Minutes
    message->data[2] = 5;  // Seconds
    message->data[3] = 18; // Day
    message->data[4] = 8;  // Month
    message->data[5] = lowByte(2019); // Year
    message->data[6] = highByte(2019);  
    message->data[7] = 0xF2;
    transmitter->Transmit(message); 
    delete message;   
}

void ECUOthers::SendMessage335()
{
    CANMessage* message = new CANMessage(0x335, 8); 
    message->data[0] = 0x00;
    message->data[1] = Counter335; 
    message->data[2] = 0x69;
    message->data[3] = 0x46;
    message->data[4] = 0x46;
    message->data[5] = 0x93;
    message->data[6] = 0x83;
    message->data[7] = 0x50;
    transmitter->Transmit(message); 
    delete message; 
    MessageTimer335 = millis(); 
    Counter335 += 16;
    if(Counter335 > 236) Counter335 = 12;    
}



void ECUOthers::Run()
{
    if (this->car->CAS->IgnitionOn)
    {
        if(millis() - MessageTimer335 >= 1000) SendMessage335();
    }
}
