#include "Car.h"

ECUAirbag::ECUAirbag(Car* car, CAN* transmitter):ECU(car, transmitter)
{
    MessageTimer34F = 16;
    MessageTimer0D7 = 10;
    Counter0D7 = 0;
    MessageTimer581 = 14;

    bool SeatBeltOn = true;
    bool HandBrakeOn = true;
}

ECUAirbag::~ECUAirbag()
{
}

//HandBrake Status
void ECUAirbag::SendMessage34F()
{
    CANMessage* message = new CANMessage(0x34F, 2);
    message->data[0] = HandBrakeOn ? 254 : 253;
    message->data[1] = 0xFF;           
    transmitter->Transmit(message); 
    delete message;    
    MessageTimer34F = millis();   
}

//Airbag/Seatbelt Counter
void ECUAirbag::SendMessage0D7()
{
    CANMessage* message = new CANMessage(0x0D7, 2);
    message->data[0] = Counter0D7;
    message->data[1] = 0xFF;           
    transmitter->Transmit(message); 
    delete message;    
    MessageTimer0D7 = millis();   
    Counter0D7++;
    if (Counter0D7 == 255) Counter0D7 = 0;
}

//SeatBelt Status
void ECUAirbag::SendMessage581()
{
    CANMessage* message = new CANMessage(0x581, 8); 
    message->data[0] = 0x40;
    message->data[1] = 0x4D; 
    message->data[2] = 0x00;
    message->data[3] = (SeatBeltOn ? 0x28 : 0x29);
    message->data[4] = 0xFF;
    message->data[5] = 0xFF;
    message->data[6] = 0xFF;
    message->data[7] = 0xFF;
    transmitter->Transmit(message); 
    delete message; 
    MessageTimer581 = millis();   
}


void ECUAirbag::Run()
{
    if (this->car->CAS->IgnitionOn)
    {
        if(millis() - MessageTimer0D7 >= 200) SendMessage0D7();            
        if(millis() - MessageTimer581 >= 5000) SendMessage581();            
        if(millis() - MessageTimer34F >= 1000) SendMessage34F();
    }
}
