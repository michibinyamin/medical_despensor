#include <EncButton.h>

#define BBTN_PIN 3
#define TBTN_PIN 2
#define pogo_Sensor_PIN 6
#define optical_Sensor_PIN 4

Button buttomBtn(BBTN_PIN);
Button topBtn(TBTN_PIN);
Button pogoBtn(pogo_Sensor_PIN);
Button opticalBtn(optical_Sensor_PIN);

// Button b;

void setup()
{
    Serial.begin(115200);

    // pinMode(4, OUTPUT);
    // pinMode(5, OUTPUT);
    // pinMode(6, OUTPUT);
    // pinMode(7, OUTPUT);
    // pinMode(8, OUTPUT);
    // pinMode(9, OUTPUT);
    // pinMode(10, OUTPUT);
    // pinMode(11, OUTPUT);
    // pinMode(12, OUTPUT);
    // pinMode(13, OUTPUT);
    // pinMode(14, OUTPUT);
    // pinMode(15, OUTPUT);
    // pinMode(16, OUTPUT);

    // b.setHoldTimeout(500);

    // pinMode();
    buttomBtn.setBtnLevel(1);
    topBtn.setBtnLevel(1);
    pogoBtn.setBtnLevel(1);
    opticalBtn.setBtnLevel(1);
}

void loop()
{
    // b.tick();
    buttomBtn.tick();
    topBtn.tick();
    pogoBtn.tick();
    opticalBtn.tick();

    Serial.println(buttomBtn.press());
    Serial.println(topBtn.press());
    Serial.println(pogoBtn.press());
    Serial.println(opticalBtn.press());

    //   digitalWrite(16, b.press());
    //   digitalWrite(15, b.pressing());
    //   digitalWrite(14, b.hold());
    //   digitalWrite(13, b.holding());
    //   digitalWrite(12, b.step());
    //   digitalWrite(11, b.release());
    //   digitalWrite(10, b.click());
    //   digitalWrite(9, b.waiting());
    //   digitalWrite(8, b.hasClicks());
    //   digitalWrite(7, b.releaseHold());
    //   digitalWrite(6, b.releaseStep());
    //   digitalWrite(5, b.busy());
    //   digitalWrite(4, b.timeout(500));

    // if (b.pressFor(10000))
    // {
    //     for (int i = 0; i < 16; i++)
    //     {
    //         digitalWrite(i, HIGH);
    //     }
    // }
    // else{
    //   Serial.println(b.holdFor());
    // }
}