#ifndef HTP_EMS0002_H
#define HTP_EMS0002_H

#include <Arduino.h>

class HTP_EMS0002 {
public:
    MuscleStim(uint8_t pin);
    void begin();
    void setFrequency(uint16_t frequency); // in Hz
    void setDutyCycle(uint8_t duty);       // 0–100%
    void setOnOffTimers(unsigned long onTime, unsigned long offTime); // in ms
    void start();
    void stop();
    void update();

private:
    uint8_t _pin;
    uint16_t _frequency;
    uint8_t _dutyCycle;
    bool _isRunning;
    unsigned long _lastToggleTime;
    bool _outputState;

    // ON/OFF timer logic
    bool _useTimers;
    unsigned long _onDuration;
    unsigned long _offDuration;
    unsigned long _lastCycleTime;
    bool _inOnPhase;
};

#endif
