extern "C" 
{
    #include "user_interface.h"
}

#include "Car.h"
#include "Images.h"

/* PINS CAN   TFT   TOUCH
 * D0         D/C
 * D1               IRQ
 * D2               CS
 * D3         
 * D4   CS
 * D5   SCK   SCK
 * D6   MISO  MISO
 * D7   MOSI  MOSI
 * D8         CS
 */


Car car;
GUI gui(&car);

void ClickHandler(uint8_t imageCode)
{
    Serial.println("IMAGE CLICK " + String(imageCode));
    switch(imageCode)
    {
        case 4: car.Airbag->SeatBeltOn = !car.Airbag->SeatBeltOn; break;            
        case 7: car.NFRM->MainBeam = !car.NFRM->MainBeam; break;
    }
}

// Images
GUIImage dash(gui.GetTFT(), gui.GetTS(), 0, 0, 320, 134, E90DashImage, E90DashImageLength, NULL, 1);
GUIImage absImage(gui.GetTFT(), gui.GetTS(), 0, 136, 53, 53, ABS, ABSImageLength, &ClickHandler, 2);
GUIImage tracImage(gui.GetTFT(), gui.GetTS(), 55, 136, 53, 53, Traction, TractionLength, &ClickHandler, 3);
GUIImage seatbeltImage(gui.GetTFT(), gui.GetTS(), 110, 136, 53, 53, SeatBelt, SeatbeltLength, &ClickHandler, 4);
GUIImage steeringImage(gui.GetTFT(), gui.GetTS(), 165, 136, 53, 53, Steering, SteeringLength, &ClickHandler, 5);
GUIImage airbagImage(gui.GetTFT(), gui.GetTS(), 220, 136, 53, 53, Airbag, AirbagLength, &ClickHandler, 6);
GUIImage beamImage(gui.GetTFT(), gui.GetTS(), 110, 190, 53, 53, Beam, BeamLength, &ClickHandler, 7);
GUIImage handbrakeImage(gui.GetTFT(), gui.GetTS(), 0, 190, 53, 53, HandBrake, HandBrakeLength, &ClickHandler, 8);
GUIImage tyreImage(gui.GetTFT(), gui.GetTS(), 55, 190, 53, 53, Tyre, TyreLength, &ClickHandler, 9);

GUIGauge speedometer(gui.GetTFT(), gui.GetTS(), 32, 24, 40, 140, 400, true, 0b0000000000000000, 0b1111111111111111, 0, 260, 0, false);
GUIGauge revCounter(gui.GetTFT(), gui.GetTS(), 214, 26, 40, 140, 400, true, 0b0000000000000000, 0b1111111111111111, 0, 70, 0, false);

void setup() 
{
    system_update_cpu_freq(160);    
    Serial.begin(115200);
    Serial.println();
    Serial.println();
    Serial.println("BMW E90 Simulator");   
    Serial.println();    
    Serial.print("Initialising Car...");
    if (!car.Init())
    {
        Serial.println("Failed!");
        while(true) yield();
    }       
    Serial.println("OK");  
    gui.RegisterElement(&dash);    
    gui.RegisterElement(&speedometer);    
    gui.RegisterElement(&revCounter);    
    gui.RegisterElement(&absImage);    
    gui.RegisterElement(&tracImage);    
    gui.RegisterElement(&seatbeltImage);    
    gui.RegisterElement(&steeringImage);    
    gui.RegisterElement(&airbagImage);    
    gui.RegisterElement(&beamImage);    
    gui.RegisterElement(&handbrakeImage);    
    gui.RegisterElement(&tyreImage);    
}

void loop() 
{
    gui.Run();
    car.KMH = speedometer.Value;
    car.DME->RPM = revCounter.Value * 100;
    car.Run();     
    yield();
}
