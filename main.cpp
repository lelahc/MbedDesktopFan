#include "mbed.h"
#include "rtos.h"
#include "uLCD_4DGL.h"
#include "Motor.h"
#include "XNucleo53L0A1.h"
#include <stdio.h>

uLCD_4DGL uLCD(p13, p14, p15); // serial tx, serial rx, reset pin;
Mutex uLCD_mutex;
Motor mot(p25, p5, p6); // pwm, fwd, rev
AnalogIn TMP(p16);
AnalogIn pot(p20);
DigitalIn pb1(p23);
DigitalIn pb2(p22);
DigitalIn pb3(p21);
Serial blue(p9, p10);
DigitalOut shdn(p26);
#define VL53L0_I2C_SDA   p28
#define VL53L0_I2C_SCL   p27
static XNucleo53L0A1 *board=NULL;


float tempF;
int status;
uint32_t distance;

float adjust = 0.0;

enum FanMode {
    MOTION_DETECTION,
    TEMPERATURE,
    MANUAL
};
Mutex modeMutex;
FanMode currentMode = MANUAL;

void motionDetectionMode() {
    status = board->sensor_centre->get_distance(&distance);
    if (status == VL53L0X_ERROR_NONE) {
        /*
        uLCD_mutex.lock();
        uLCD.locate(0,6);
        uLCD.printf("Dist=%ld mm\r\n", distance);  
        uLCD_mutex.unlock();
        */
        if(distance <= 500){
            float speed = pot.read();
            speed += adjust;
            if (speed > 1){
                adjust -= 0.1;
                speed = 1;
            }
            else if (speed < 0){
                adjust = 0.0 - pot.read();
                speed = 0.0;
            }
            mot.speed(speed);
        }
        else{
            mot.speed(0);
        }   
    }
}

void temperatureMode() {
    if (tempF > 75){
        float speed = pot.read();
        speed += adjust;
        if (speed > 1){
            adjust -= 0.1;
            speed = 1;
        }
        else if (speed < 0){
            adjust = 0.0 - pot.read();
            speed = 0.0;
        }
        mot.speed(speed);
    }
    else{
        mot.speed(0);
    }
}


void manualMode() {
    // Allow the user to control the fan manually using potentiometer
    float speed = pot.read();
    speed += adjust;
    if (speed > 1){
        adjust -= 0.1;
        speed = 1;
    }
    else if (speed < 0){
        adjust = 0.0 - pot.read();
        speed = 0.0;
    }
    mot.speed(speed);
}

void bluetooth(void const *args) {
    char bnum=0;
    char bhit=0;
    while(1) {
        while (!blue.readable()) {
            Thread::wait(20);
        }
        uLCD_mutex.lock();
        if (blue.getc()=='!') {
            if (blue.getc()=='B') { //button data packet
                bnum = blue.getc(); //button number
                bhit = blue.getc(); //1=hit, 0=release
                if (blue.getc()==char(~('!' + 'B' + bnum + bhit))) { //checksum OK?
                    switch (bnum) {
                        case '1': //number button 1
                            if (bhit=='1') {
                                modeMutex.lock();
                                currentMode = MOTION_DETECTION;
                                modeMutex.unlock();
                            }
                            break;
                        case '2': //number button 2
                            if (bhit=='1') {
                                modeMutex.lock();
                                currentMode = TEMPERATURE;
                                modeMutex.unlock();
                            }
                            break;
                        case '3': //number button 3
                            if (bhit=='1') {
                                modeMutex.lock();
                                currentMode = MANUAL;
                                modeMutex.unlock();
                            }
                            break;
                        case '5': //up arrow
                            if (bhit=='1') {
                                modeMutex.lock();
                                adjust += 0.1;
                                modeMutex.unlock();
                            }
                            break;
                        case '6': //down arrow
                            if (bhit=='1') {
                                modeMutex.lock();
                                adjust -= 0.1;
                                modeMutex.unlock();
                            }
                            break;
                        case '7': //left arrow
                            if (bhit=='1') {
                                modeMutex.lock();
                                adjust = 0.0;
                                modeMutex.unlock();
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        uLCD_mutex.unlock();
    }
}

void updateLCD() {
    uLCD_mutex.lock();
    // Update LCD with temperature and current mode information
    uLCD.locate(0,0);
    uLCD.printf("Temp: %.2f F\n", tempF); 
    uLCD.locate(0,4);
    uLCD.printf("Mode: %s\n", currentMode == MOTION_DETECTION ? "Motion     " : (currentMode == TEMPERATURE ? "Temperature" : "Manual     "));
    uLCD_mutex.unlock();
}

int main()
{
    DevI2C *device_i2c = new DevI2C(VL53L0_I2C_SDA, VL53L0_I2C_SCL);
    //creates the 53L0A1 expansion board singleton obj 
    board = XNucleo53L0A1::instance(device_i2c, A2, D8, D2);
    shdn = 0; //must reset sensor for an mbed reset to work
    wait(0.1);
    shdn = 1;
    wait(0.1);
    // init the 53L0A1 board with default values 
    status = board->init_board();
    
    while (status) {
        uLCD_mutex.lock();
        uLCD.locate(0,6);
        uLCD.printf("Failed to init board! \r\n");
        uLCD_mutex.unlock();
        status = board->init_board();
    }
    
    Thread t1(bluetooth);

    pb1.mode(PullUp);
    pb2.mode(PullUp);
    pb3.mode(PullUp);

    while (1) {
        // Check pushbuttons and update mode accordingly
        modeMutex.lock();
        if (!pb1.read()) {
            currentMode = MOTION_DETECTION;
        } else if (!pb2.read() ) {
            currentMode = TEMPERATURE;
        } else if (!pb3.read()) {
            currentMode = MANUAL;
        }
        modeMutex.unlock();

        if (currentMode == MANUAL){
            manualMode();
        }
        else if (currentMode == TEMPERATURE){
            temperatureMode();
        }
        else if (currentMode == MOTION_DETECTION){
            motionDetectionMode();
        }
        
        float tempC;
        //conversion to degrees C - from sensor output voltage per LM61 data sheet
        tempC = ((TMP*3.3)-0.500)*100.0;
        //convert to degrees F
        tempF = (9.0*tempC)/5.0 + 32.0;
        // Update LCD with information
        updateLCD();
        Thread::wait(100);
    }

}

