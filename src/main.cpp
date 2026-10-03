#include <Arduino.h>
#include <debounce.h>

/// @brief Enum for LED states.
enum LedState {
  OFF,
  ON
};

/// @brief Enum for LED states.
enum LedMode{
  BLINKING = 0,
  ALWAYS_OFF = 1,
  ALWAYS_ON = 2
};

/// @brief Class containing application settings and constants.
class APP_SETTINGS{
  public:
    static constexpr int LED_PIN_12 = 12;
    static constexpr int LED_PIN_13 = 13;
    static constexpr int DELAY_TIME_FAST = 1000;
    static constexpr int DELAY_TIME_SLOW = 2000;
    static constexpr int SPEED_ITERATION_COUNTER = 1000;
    static constexpr int BUTTON_PIN = 4;
};



/// @brief Class representing an LED and its state.
class Led{
    int pin;
    LedState state;
  public:
    void init() {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
    }
    void set(LedState newState) {
      state = newState;
      digitalWrite(pin, newState == LedState::ON ? HIGH : LOW);
    }
    void setPin(int pinNumber) {
      pin = pinNumber;
    }
    LedState getState() {
      return state;
    }
};




volatile long int timeCounter = 0;
Led myLed;

unsigned long loopTime = 0;
unsigned long previousLoopTime = 0;
unsigned long loopTimeCounter = 0;

volatile bool buttonFlag = false;
LedMode buttonState;


void measureLoopTime() {
  unsigned long loopTimeNow = micros();
  loopTime = loopTime + loopTimeNow - previousLoopTime;
  previousLoopTime = loopTimeNow;

}

void loopTimeReset() {
  loopTime = 0;
  loopTimeCounter = 0;
}

void IRAM_ATTR handleButtonPress() {
  buttonFlag = true;
}

void doBlinking() {
  const unsigned long newTime = millis();
  if (newTime - timeCounter >= APP_SETTINGS::DELAY_TIME_FAST) {
    if(myLed.getState() == LedState::ON) {
      myLed.set(LedState::OFF);
    } else {
      myLed.set(LedState::ON);
    }
    timeCounter = newTime;
  }
}

void updateButtonState() {
    buttonState = static_cast<LedMode>((static_cast<int>(buttonState) + 1) % 3);
}

void setup() {
  Serial.begin(115200);
  myLed.setPin(APP_SETTINGS::LED_PIN_12);
  myLed.init();
  previousLoopTime = micros();
  buttonState = LedMode::BLINKING;

  pinMode(APP_SETTINGS::BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(APP_SETTINGS::BUTTON_PIN), handleButtonPress, FALLING);
}

void loop() {
  loopTimeCounter++;
  
  if(buttonFlag) {
    debounce(updateButtonState, 50);
    buttonFlag = false;
  }

  if(buttonState == LedMode::BLINKING) {
    doBlinking();
  }else if(buttonState == LedMode::ALWAYS_ON) {
    myLed.set(LedState::ON);
  }else if(buttonState == LedMode::ALWAYS_OFF) {
    myLed.set(LedState::OFF);
  }


  measureLoopTime();
  if(loopTimeCounter >= APP_SETTINGS::SPEED_ITERATION_COUNTER) {
    // Serial.println(loopTime / APP_SETTINGS::SPEED_ITERATION_COUNTER);
    loopTimeReset();
  };

}

