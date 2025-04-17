#include "aftb_x9c103s.h"

#define MAX_WIPPER_POS 100

POTX9_t potx9;

static int16_t x9c103s_value = 0; 

uint16_t x9c103s_reg(uint16_t value) {
    int16_t current = static_cast<int16_t>(value);
    digitalWrite(POT_CS, LOW);
    if (x9c103s_value > current) {
        digitalWrite(POT_UD, LOW);
        for(;x9c103s_value > current; x9c103s_value--) {
            digitalWrite(POT_INC, LOW);
            digitalWrite(POT_INC, HIGH);
            delay(2);
        }
    }
    else if (x9c103s_value < current) {
        digitalWrite(POT_UD, HIGH);
        for(;x9c103s_value < current; x9c103s_value++) {
            digitalWrite(POT_INC, LOW);
            digitalWrite(POT_INC, HIGH);
            delay(2);
        }
    }
    /**
     * NOTE: We just didn't save anything to avoid waste cycle limits,
     * because this POT has a permanent memory of 100,000 write cycles.
     */ 
    
    // digitalWrite(POT_CS, HIGH); //save value in the potentiometer
    return 0;
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
    uint16_t s_volts = 0;
    
    analogReference(ANALOG_REF_EXTERNAL);
    digitalWrite(POT_CS, HIGH);
    do {

        uint16_t s_volts = analogRead(VPP);

        // Inc/Dec wiper to sync up with the counter
        x9c103s_reg(test_counter);

        if (!reverse) {
            test_counter++;
        }
        else {
            test_counter--;
        }

        // POT x9c103s only has 100 steps, refer to datasheet
        if ((test_counter == MAX_WIPPER_POS) || (test_counter == 0)) {
            if (test_counter == MAX_WIPPER_POS)
                potx9.max_wiper = s_volts;
            else
                potx9.min_wiper = s_volts;
            reverse=!reverse;
        }
        delay(1);
    } while (test_counter > 0);


    float volts = convertToFloat(potx9.max_wiper) * 6;

#if 0
    Serial.println("finish");
    Serial.print("volts: \t");
    Serial.print(volts, 2);
    Serial.print("\n\r");
#endif

    if (volts < 18)
        return FAIL;

  return OK;
}

