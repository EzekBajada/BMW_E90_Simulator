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
GUIImage dash(gui.GetTFT(), gui.GetTS(), 0, 0, 320, 134, E90DashImage, E90DashImageLength);
GUIGauge speedometer(gui.GetTFT(), gui.GetTS(), 32, 24, 40, 140, 400, 0b0000000000000000, 0b1111111111111111, 0, 260, 0, false);
GUIGauge revCounter(gui.GetTFT(), gui.GetTS(), 214, 26, 40, 140, 400, 0b0000000000000000, 0b1111111111111111, 0, 70, 0, false);


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
}

void loop() 
{
    gui.Run();
    car.KMH = speedometer.Value;
    car.DME->RPM = revCounter.Value;
    car.Run();      
    yield();
}
