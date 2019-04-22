#include "Car.h"

Car::Car()
{
    this->transmitter = new CAN(D4, CAN_100KBPS);        
}

bool Car::Init()
{ 
    ClutchPressed = false; 
    Brake = 0;   
    KMH = 20;
    Fuel = 51;
    if (!this->transmitter->Init()) return false;       
    
    CAS = new ECUCAS(this, transmitter);
    DME = new ECUDME(this, transmitter);
    NFRM = new ECUNFRM(this, transmitter);
    Airbag = new ECUAirbag(this, transmitter);
    Others = new ECUOthers(this, transmitter);

    Others->SendMessage39E();
    DME->SendMessage1A6();
    DME->SendMessage0AA();
    
    CAS->SendMessage130();
    delay(100);   
    CAS->SendMessage380();
    delay(50);
    CAS->SendMessage380();
    delay(50);
    CAS->SendMessage380();
    delay(50);

    return true;
}

void Car::Run()
{
    CAS->Run();  
    DME->Run();  
    NFRM->Run();
    Airbag->Run();
    Others->Run();
}
