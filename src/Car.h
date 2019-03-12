#pragma once

#include <SPI.h>
#include "mcp_can.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "XPT2046_Touchscreen.h"

class CAN;
class CANMessage;
class Car;
class ECU;
class ECUCAS;
class ECUDME;
class ECUOthers;
class ECUNFRM;
class ECUAirbag;
class GUI;

class CANMessage
{
    public:
        CANMessage(uint32_t id, uint8_t length) 
        {
            this->id = id;
            this->length = length;
            this->timestamp = 0;
            this->isExtended = 0;
            this->isRemoteRequest = 0;
        };
        uint64_t timestamp;
        uint32_t id;
        uint8_t data[8];  
        uint8_t length;
        bool isExtended;
        bool isRemoteRequest;
};

class Car
{
    public:
    Car();  
    bool Init();
    void Run();     
    uint16_t KMH;
    uint8_t Fuel;

    bool ClutchPressed;
    uint8_t Brake; 

    ECUCAS* CAS;
    ECUDME* DME;
    ECUNFRM* NFRM;
    ECUAirbag* Airbag;
    ECUOthers* Others;

    
    private:
    CAN* transmitter;
  
};


class CAN
{
    public:
        CAN(int32_t Pin, uint8_t Speed);
        bool Init();
        bool Transmit(CANMessage* message);

    private:
        int Pin;
        uint8_t Speed;
        MCP_CAN* interface = NULL;   
        bool initialised = false;
};

class ECU
{ 
    public:
        ECU(Car* car, CAN* transmitter);
        ~ECU();
        virtual void Run() = 0;
        
    protected:  
        Car* car;
        CAN* transmitter;
};

class ECUCAS:public ECU
{
    public:
        ECUCAS(Car* car, CAN* transmitter);
        ~ECUCAS();
        void Run();
        void SendMessage130();
        void SendMessage380();
        void SendMessage26E();
        bool KeyInSlot;
        bool IgnitionOn;
        bool EngineRunning;
        bool ClutchPressed;
        
    private:   
        uint32_t MessageTimer130;
        uint16_t Counter130 = 0;
        uint64_t MessageTimer26E; 
};

class ECUDME:public ECU
{
    public:
        ECUDME(Car* car, CAN* transmitter);
        ~ECUDME();
        void Run();
        uint16_t RPM;
        uint16_t Torque;
        uint8_t EngineTemperature;
        int16_t BatteryVolts;
        uint8_t InjectorVolume;    

        void SendMessage1A6();
        void SendMessage0AA();
        void SendMessage0A8();
        void SendMessage1D0();
        void SendMessage349();
        void SendMessage3B4();
        void SendMessage592(uint8_t errorCode);
        
    private:  
        uint64_t MessageTimer0AA;
        uint64_t MessageTimer1A6; 
        uint64_t MessageTimer0A8;
        uint64_t MessageTimer1D0;
        uint64_t MessageTimer349;
        uint64_t MessageTimer3B4;
        uint16_t InjectorCounter;
        uint16_t Counter1A6;
        uint16_t lastKMH;
        uint8_t Counter1D0_B2;

};

class ECUOthers:public ECU
{
    public:
        ECUOthers(Car* car, CAN* transmitter);
        ~ECUOthers();
        void Run();
        void SendMessage39E();
        void SendMessage335();

   private:
        uint64_t MessageTimer335;
        uint8_t Counter335;
};

class ECUNFRM:public ECU
{
    public:
        ECUNFRM(Car* car, CAN* transmitter);
        ~ECUNFRM();
        void Run();  

        bool BrakeLight;
        bool SideLight;
        bool DippedBeam;
        bool MainBeam;
        bool FrontFogLights;
        bool RearFogLights;
        bool InteriorLight;
        bool ExteriorLight;

        void SendMessage21A();

   private:
        uint64_t MessageTimer21A;

};

class ECUAirbag:public ECU
{
    public:
        ECUAirbag(Car* car, CAN* transmitter);
        ~ECUAirbag();
        void Run();  
        bool SeatBeltOn;
        bool HandBrakeOn;

        void SendMessage34F();
        void SendMessage0D7();
        void SendMessage581();


   private:
        uint64_t MessageTimer34F;
        uint64_t MessageTimer0D7;
        uint8_t Counter0D7;
        uint64_t MessageTimer581;

};

class GUIElement
{
    public:
        GUIElement(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
        virtual ~GUIElement() {};
        Adafruit_ILI9341* tft;
        XPT2046_Touchscreen* touch;
        uint16_t X;
        uint16_t Y;
        uint16_t Width;
        uint16_t Height;
        virtual void Run(TS_Point* clickPoint) = 0;
};

class GUIImage: public GUIElement
{
    public:
        GUIImage(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, const uint8_t* Image, uint16_t ImageLength);
        ~GUIImage() {};
        const uint8_t* Image;
        uint16_t ImageLength;
        void Run(TS_Point* clickPoint);

    private:
        bool needsRedrawing = true;
};

class GUIGauge: public GUIElement
{
    public:
        GUIGauge(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t X, uint16_t Y, uint16_t Radius, uint16_t DegreesStart, uint16_t DegreesStop, uint16_t BackgroundColor, uint16_t ForegroundColor, uint16_t MinValue, uint16_t MaxValue, uint16_t Value, bool IsTransparent);
        ~GUIGauge() {};
        uint16_t Radius;
        uint16_t DegreesStart;
        uint16_t DegreesStop;
        uint16_t BackgroundColor = 0b0000000000011111;  
        uint16_t ForegroundColor = 0b1111111111111111; 
        uint16_t MinValue = 0;
        uint16_t MaxValue = 0;
        uint16_t Value = 0;
        uint16_t MovementSpeed = 200; // ms per update
        bool IsTransparent = false;
        void Run(TS_Point* clickPoint);
        
     private:
        bool needsRedrawing = true;
        TS_Point* clickStartPoint = NULL;
        uint64_t lastMovement = 0;
};

class GUI
{
    public:
        GUI(Car* car);
        ~GUI();
        Adafruit_ILI9341* GetTFT() { return this->tft; };
        XPT2046_Touchscreen* GetTS() { return this->touch; };       
        void RegisterElement(GUIElement* element);
        void Run();

    private:
        Car* car = NULL;
        Adafruit_ILI9341* tft;
        XPT2046_Touchscreen* touch;
        char page = 'S';
        uint16_t vi1 = 65535, vj1 = 65535, vi2 = 65535, vj2 = 65535; // Touch Screen Calibration
        void drawScreen();
        const uint16_t CAL_MARGIN = 20;
        void calibratePoint(uint16_t x, uint16_t y, uint16_t &vi, uint16_t &vj);
        void calibrateTouchScreen();
        void applyCalibration(TS_Point &p);
        GUIElement** elements = NULL; // Array of pointers
        uint16_t elementCount = 0;
};
