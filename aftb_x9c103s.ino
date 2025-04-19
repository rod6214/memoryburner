#include "aftb_x9c103s.h"
#include "utils.h"


POTX9_t potx9;

static int16_t x9c103s_value = 0; 

// #define VPP_5V0   0xFF
// #define VPP_9V0   0      ------------- 42
// #define VPP_9V5   1      ------------- 45
// #define VPP_10V0  2      ------------- 48
// #define VPP_10V5  3      ------------- 51
// #define VPP_11V0  4      ------------- 54
// #define VPP_11V5  5      ------------- 57
// #define VPP_12V0  6      ------------- 60
// #define VPP_12V5  7      ------------- 63
// #define VPP_13V0  8      ------------- 66
// #define VPP_13V5  9      ------------- 69
// #define VPP_14V0  10     ------------- 72
// #define VPP_14V5  11     ------------- 75
// #define VPP_15V0  12     ------------- 77
// #define VPP_15V5  13     ------------- 80
// #define VPP_16V0  14     ------------- 83
// #define VPP_16V5  15     ------------- 85
// #define VPP_15V7  16     ------------- 81
// #define VPP_5V0   17     ------------- 0
/*
    POT note: you should calibrate the onboard potentiometer by powering mt3608
    with 3.3v at the input then try to look for 19.86v at the output, to do so you'll need to 
    connect a 10K Ohm resistance between feedback and ground before start to calibrate.
*/

const uint8_t cfgCalibration[] PROGMEM =
{
      42, 45, 48, 51, 54, 57, 60, 63, 66, 69, 72, 75, 77, 80, 83, 85, 81, 0
};

const float cfgRealCalibration[] PROGMEM =
{
      9.0f, 9.5f, 10.0f, 10.5f, 11.0f, 11.5f, 12.0f, 12.5f, 13.0f, 13.5f, 14.0f, 14.5f, 15.0f, 15.5f, 16.0f, 16.5f, 15.7f, 4.8f
};

float wiperSetPoints[WIPER_VOLTS];


uint16_t x9c103s_reg(uint16_t value) {
    int16_t current = static_cast<int16_t>(value);
    digitalWrite(POT_CS, LOW);
    if (x9c103s_value > current) {
        digitalWrite(POT_UD, LOW);
        for(;x9c103s_value > current; x9c103s_value--) {
            digitalWrite(POT_INC, LOW);
            digitalWrite(POT_INC, HIGH);
            delay(20);
        }
    }
    else if (x9c103s_value < current) {
        digitalWrite(POT_UD, HIGH);
        for(;x9c103s_value < current; x9c103s_value++) {
            digitalWrite(POT_INC, LOW);
            digitalWrite(POT_INC, HIGH);
            delay(20);
        }
    }
    /**
     * NOTE: We just didn't save anything to avoid waste cycle limits,
     * because this POT has a permanent memory of 100,000 write cycles.
     */ 
    
    // digitalWrite(POT_CS, HIGH); //save value in the potentiometer
    return 0;
}

float x9c103s_read() {
    return convertToFloat(x9c103s_value) * 6;
}

void x9c103s_write(float value) {
    float volts = 0.0f;
    int counter = MAX_WIPER_POS;
    do {
        x9c103s_reg(counter);
        int16_t s_volts = analogRead(A0);
        volts = convertToFloat(s_volts) * 6;
        Serial.print(volts, 2);
        Serial.print("\t");
        Serial.print(value);
        Serial.print("\t");
        Serial.print(counter);
        Serial.print("\n");
        if (value >= volts) return; // round always below due to error conversion
        counter--;
    }while(counter > 0);
}

void x9c103s_init() {
    pinMode(POT_CS, OUTPUT);
    pinMode(POT_INC, OUTPUT);
    pinMode(POT_UD, OUTPUT);

    digitalWrite(POT_INC, HIGH);
    digitalWrite(POT_CS, HIGH); //unselect the POT's SPI bus
}

uint8_t x9c103s_detect() {
    int test_counter = 0;
    bool reverse = false;
    
    digitalWrite(POT_CS, HIGH);
    do {

        // Inc/Dec wiper to sync up with the counter
        x9c103s_reg(test_counter);

        if (!reverse) {
            test_counter++;
        }
        else {
            test_counter--;
        }

        // POT x9c103s only has 100 steps, refer to datasheet
        if ((test_counter == MAX_WIPER_POS) || (test_counter == 0)) {
            if (test_counter == MAX_WIPER_POS)
                potx9.max_wiper = varVppMeasureVpp(0);
            reverse=!reverse;
        }
        delay(5);
    } while (test_counter > 0);

    float volts = getFixedVoltage(potx9.max_wiper);

#if 0
    Serial.print(volts, 2);
    Serial.print("\n");
#endif

    if (volts < 18)
        return FAIL;

    memcpy_P(wiperSetPoints, cfgRealCalibration, sizeof(wiperSetPoints));
  return OK;
}

