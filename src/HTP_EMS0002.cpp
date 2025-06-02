/*
  ******************************************************
  ⚠️ تحذير – عدم تعديل إعدادات التردد أو توقيت التشغيل ⚠️
  ******************************************************

  تم تصميم هذه المكتبة للتحكم الآمن في وحدة تحفيز العضلات.

  ❗ يُمنع تعديل إعدادات التردد الداخلية أو توقيت التشغيل/الإيقاف ما لم تكن على دراية كاملة بتأثيرات ذلك.
  ❗ الإعدادات الحالية مصممة لتوفير تشغيل آمن وتجنب الضرر الجسدي أو التلف في الجهاز.
  ❗ أي تعديلات غير مصرح بها قد تؤدي إلى سلوك غير آمن أو فرط في التحفيز أو تلف في الأجهزة.

  ➤ استخدم فقط الدوال العامة المتاحة ضمن المكتبة مثل:
     setFrequency و setDutyCycle و setOnOffTimers،
     وضمن الحدود الموصى بها.

  أي تعديل يتم على مسؤوليتك الخاصة.

  ******************************************************
*/


/*
  ******************************************************
  ⚠️ WARNING – DO NOT MODIFY CORE SETTINGS ⚠️
  ******************************************************

  This library is designed to safely control a muscle stimulation module.

  ❗ Do NOT modify internal frequency settings or ON/OFF timer configurations unless you fully understand the safety implications.
  ❗ Frequency and timing are configured to ensure safe operation and prevent injury or device malfunction.
  ❗ Unauthorized changes may lead to unsafe behavior, overstimulation, or hardware damage.

  ➤ Use only the public methods provided by the library (setFrequency, setDutyCycle, setOnOffTimers) and within recommended limits.

  All modifications are at your own risk.

  ******************************************************
*/


#include "HTP_EMS0002.h"

HTP_EMS0002::HTP_EMS0002(uint8_t pin)
    : _pin(pin), _frequency(0), _dutyCycle(0), _isRunning(false),
      _lastToggleTime(0), _outputState(false),
      _useTimers(false), _onDuration(0), _offDuration(0),
      _lastCycleTime(0), _inOnPhase(true) {}

void HTP_EMS0002::begin() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void HTP_EMS0002::setFrequency(uint16_t frequency) {
    if (frequency > 0 && frequency <= 50) {
        _frequency = frequency;
    }
}

void HTP_EMS0002::setDutyCycle(uint8_t duty) {
    if (duty <= 100) {
        duty = map(duty, 0, 100, 0, 25);
        _dutyCycle = duty;
    }
}

void HTP_EMS0002::setOnOffTimers(unsigned long onTime, unsigned long offTime) {
    // Limit ON to 2000 ms and OFF to 5000 ms

    _onDuration = min(max(onTime, 100UL), 2000UL);
    _offDuration = min(offTime, 5000UL);
    _useTimers = true;
    _lastCycleTime = millis();
    _inOnPhase = true;
}

void HTP_EMS0002::start() {
    _isRunning = true;
    _lastToggleTime = millis();
    _outputState = true;
    digitalWrite(_pin, HIGH);
    _lastCycleTime = millis();
    _inOnPhase = true;
}

void HTP_EMS0002::stop() {
    _isRunning = false;
    digitalWrite(_pin, LOW);
}


void HTP_EMS0002::update() {
    if (!_isRunning || _frequency == 0 || _dutyCycle == 0) {
        digitalWrite(_pin, LOW);
        _outputState = false;
        return;
    }

    unsigned long now = millis();

    // Handle ON/OFF cycle timing
    if (_useTimers) {
        if (_inOnPhase && (now - _lastCycleTime >= _onDuration)) {
            _inOnPhase = false;
            _lastCycleTime = now;
            digitalWrite(_pin, LOW);
            _outputState = false;
            return;
        } else if (!_inOnPhase && (now - _lastCycleTime >= _offDuration)) {
            _inOnPhase = true;
            _lastCycleTime = now;
            _lastToggleTime = now;
            _outputState = true;
            digitalWrite(_pin, HIGH);
        }
        if (!_inOnPhase) return;  
    }

    // PWM signal update
    unsigned long period = 1000UL / _frequency;
    unsigned long onTime = (period * _dutyCycle) / 25;
    unsigned long offTime = period - onTime;

    if (_outputState && (now - _lastToggleTime >= onTime)) {
        digitalWrite(_pin, LOW);
        _outputState = false;
        _lastToggleTime = now;
    } else if (!_outputState && (now - _lastToggleTime >= offTime)) {
        digitalWrite(_pin, HIGH);
        _outputState = true;
        _lastToggleTime = now;
    }
}
