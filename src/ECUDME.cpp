#include "Car.h"

ECUDME::ECUDME(Car* car, CAN* transmitter):ECU(car, transmitter)
{
    MessageTimer1A6 = 4;  
    Counter1A6 = 0;
    MessageTimer0AA = 6;
    MessageTimer0A8 = 18;
    InjectorCounter = 0;
    Counter1D0_B2 = 0;
    MessageTimer1D0 = 20;
    MessageTimer349 = 24;
    MessageTimer3B4 = 28;

    RPM = 900;
    Torque = 2; 
    InjectorVolume = 7;
    BatteryVolts = 12;  
    EngineTemperature = 20;     

}

ECUDME::~ECUDME()
{
}

//KOMBI Speed
void ECUDME::SendMessage1A6() 
{    
    uint16_t KMH_A = ((((millis() - MessageTimer1A6) / 50) * this->car->KMH)) / 3;  //Division by 3 added by Robert
    uint16_t KMH_2A = KMH_A + lastKMH;
    lastKMH = KMH_2A;  
    CANMessage* message = new CANMessage(0x1A6, 8);       
    message->data[0] = lowByte(KMH_2A);  
    message->data[1] = highByte(KMH_2A);    
    message->data[2] = lowByte(KMH_2A);   
    message->data[3] = highByte(KMH_2A);  
    message->data[4] = lowByte(KMH_2A); 
    message->data[5] = highByte(KMH_2A);  
    message->data[6] = lowByte(Counter1A6);   
    message->data[7] = highByte(Counter1A6) | B11110000;          
    Counter1A6 += 200;        
    transmitter->Transmit(message); 
    delete message;
    MessageTimer1A6 = millis();    
}

//Engine RPM
void ECUDME::SendMessage0AA() 
{    
    uint16_t RPM_A = this->RPM * 4;        
    CANMessage* message = new CANMessage(0x0AA, 8);         
    message->data[0] = 0xFE;  
    message->data[1] = 0xFE;  
    message->data[2] = 0xFF;  
    message->data[3] = 0x00;  
    message->data[4] = lowByte(RPM_A);
    message->data[5] = highByte(RPM_A);
    message->data[6] = 0xFE;  
    message->data[7] = 0x99;  
    transmitter->Transmit(message); 
    delete message;
    MessageTimer0AA = millis();
} 

//Torque Clutch & Brake
void ECUDME::SendMessage0A8() 
{
    CANMessage* message = new CANMessage(0x0A8, 8);
    uint16_t Torque = this->Torque * 32;
    uint8_t byte1 = lowByte(Torque);
    uint8_t byte2 = highByte(Torque);    
    message->data[0] = 84;      
    message->data[1] = byte1;
    message->data[2] = byte2;
    message->data[3] = byte1;
    message->data[4] = byte2;
    message->data[5] = this->car->ClutchPressed ? 241 : 240;
    message->data[6] = 0x03;   
    message->data[7] = this->car->Brake != 0 ? 32 : 2;        
    transmitter->Transmit(message); 
    delete message;
    MessageTimer0A8 = millis();
}

//Engine Temperature
void ECUDME::SendMessage1D0() 
{  
    CANMessage* message = new CANMessage(0x1D0, 8);
    uint8_t byte0 = this->EngineTemperature + 48;        
    message->data[0] = byte0;  
    message->data[1] = byte0;    
    message->data[2] = Counter1D0_B2;    
    message->data[3] = 0xCB;    
    message->data[4] = lowByte(InjectorCounter);
    message->data[5] = highByte(InjectorCounter);
    message->data[6] = this->car->ClutchPressed ? 0xCD : 0xCC;        
    message->data[7] = 0x90;  
    transmitter->Transmit(message); 
    delete message;
    InjectorCounter += this->InjectorVolume;    
    Counter1D0_B2++;        
}

// Check Engine Light - on fr 17 seconds with 1 sending
void ECUDME::SendMessage592(uint8_t errorCode) 
{  
    CANMessage* message = new CANMessage(0x592, 8);    
    message->data[0] = 0x40;  
    message->data[1] = errorCode;
    /* Error ID
    0x01 Cruise Malfunction
    0x03 Cruise Malfunction + !
    0x04 Burnt Light, 1
    0x0E Front right door open
    0x0F Front left door open    
    0x10 Left Rear Door Open
    0x11 Right Rear Door open
    0x12 Bonnet Open
    0x13 Boot Open
    0x15 Red Car Error
    0x18 ABS failure
    0x1A Diesel Heater
    0x1E Emission mulfunction - multiple errors resulting in red emissions error   
    0x1F Check Engine  
    0x20 Close Gas Cap  
    0x21 Check Engine     
    0x22 Check Engine  LED
    0x23 DSC Error
    0x24 DSC Off
    0x28 Red Engine Overheating
    0x29 Red Triangle
    0x2A ABS
    0x2B Shock Absorbers
    0x2D Air Suspension
    0x2E Red Seatbelt
    0x31 DPF 
    0x32 Flat Tyre
    0x37 Red Handbrake (while driving)
    0x3C Red Car Service Icon (CAN Error)
    0x3E Red Limit
    0x3F Red Flat Tyre
    0x41 Batery Key
    0x42 Crossed out Key
    0x44 TX Bateery
    0x45 Cruise error
    0x46 Steering error
    0x47 Brake Red Error
    0x49 Steering with LED
    0x4B Red ABS
    0x4C Yellow Car Service
    0x4D Seatbelt LED
    0x4E Yellow Limit
    0x4F Ice Warning
    0x55 Cruise Error
    0x56 Side Light On
    0x57 Fog Light Error
    0x58 Bulb Error 1
    0x5b Red SeatBelt
    0x5C Yellow Airbag
    0x61 Red Airbag
    0x67 Gearbox Too Hot Yellow
    0x68 Gearbox Too Hot Red
    0x6C Red Airbag
    0x6F Bulb Failed
    0x71 Side Light On
    0x78 Headlight Overheated
    0x80 Headlight Error
    0x81 Rear Foglight Error
    0x8B Front Left Low tyre
    0x8c Rear Right Low tyre
    0x8d Rear Left Low tyre
    0x8f Front right low tyre
    0x93 All Low Tyres 
    0x95 Tyre Pressure Sensor Inactive
    0x98 Red PlugIn Error
    0x99 Revolver Red Exclamation Error
    0xA1 Yellow Recharge Battery
    0xA2 Beacon Light Yellow
    0xA4 Windscreen Washer Low
    0xA5 Ice Warning
    0xA6 Tank Yellow
    0xA7 Reset Time
    0xA8 Gearbox Yellow Warning
    0xA9 Exclamation Yellow
    0xB1 Cruise Control Error
    0xB5 Airbag Malfunction LED
    0xB8 DSC Off LED
    0xBA Gearbox Malfunction
    0xBB Steering Lock
    0xBC Sterring Calibration Error
    0xC0 Flat Tyre
    0xC2 PDC Malfunction
    0xC4 Light Bulb Burnt
    0xD1 Key Error
    0xD2 Handbrake on Yellow
    0xD7 DSC LED
    0xD9 Recharge Battery Red
    0xDA Low Oil / Service Car Yellow
    0xE7 Service Car Red
    0xEC ABS Fault
    0xF3 DSC Fault
    0xF4 Press Brake (shifting)
    0xFC Exclamation Mark yellow
    */
    message->data[2] = 0x00;
    message->data[3] = 0x31; // 31 = CEL On, 30 = CEL Off, 
    message->data[4] = 0xFF;
    message->data[5] = 0xFF;
    message->data[6] = 0xFF;  
    message->data[7] = 0xFF;
    transmitter->Transmit(message); 
    delete message;     
}



//Fuel Level Sensors
void ECUDME::SendMessage349()
{    
    uint16_t fuel = this->car->Fuel * 160;
    CANMessage* message = new CANMessage(0x349, 5);
    message->data[0] = lowByte(fuel);   //Sensor 1 L
    message->data[1] = highByte(fuel);  //Sensor 1 H
    message->data[2] = lowByte(fuel);   //Sensor 2 L
    message->data[3] = highByte(fuel);  //Sensor 2 H    
    message->data[4] = 0x00;  
    transmitter->Transmit(message); 
    delete message;       
    MessageTimer349 = millis();      
}  

//Battery Voltage and Charge Status
void ECUDME::SendMessage3B4() 
{ 
    CANMessage* message = new CANMessage(0x3B4, 8);    
    message->data[0] = (this->BatteryVolts * 68) % 256;
    message->data[1] = ((this->BatteryVolts * 68) / 256) + 240;
    message->data[2] = this->car->CAS->EngineRunning ? 0 : 9;
    message->data[3] = 0xFF;
    message->data[4] = 0xFF;
    message->data[5] = 0xFF;
    message->data[6] = 0xFF;
    message->data[7] = 0xFF;  
    transmitter->Transmit(message); 
    delete message;          
    MessageTimer3B4 = millis();  
}

void ECUDME::Run()
{
    if (this->car->CAS->IgnitionOn)
    {
        if(millis() - MessageTimer0A8 >= 10) SendMessage0A8();           
        if(millis() - MessageTimer0AA >= 10) SendMessage0AA();           
        if(millis() - MessageTimer1A6 >= 100) SendMessage1A6();           
        if(millis() - MessageTimer1D0 >= 200) SendMessage1D0();           
        if(millis() - MessageTimer349 >= 200) SendMessage349();             
        if(millis() - MessageTimer3B4 >= 4000) SendMessage3B4();
    }
}
