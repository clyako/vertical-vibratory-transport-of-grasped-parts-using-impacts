#include "CarltonHapticMotor.h"

CarltonHapticMotor::CarltonHapticMotor()
{
    analogWriteResolution(_analog_res);
    analogReadResolution(_analog_res);
    pinMode(DAC_PIN, OUTPUT);
    pinMode(VOLTAGE_OFFSET_READ_PIN, INPUT);
    pinMode(VOLTAGE_OFFSET_WRITE_PIN, OUTPUT);
    zero();
}

void CarltonHapticMotor::sawtooth(unsigned long time_on_high_microseconds, float time_between_decrements_microseconds)
{
    // controls if frequency is fixed (set by user) or controlled via a potentiometer
    if (time_between_decrements_microseconds != -1)
    {
        _time_between_decrements = time_between_decrements_microseconds;
    }
    else
    {
        _update_sawtooth_frequency();
    }
    _update_max_dac_value();
    _update_voltage_offset();

    if ((float)_decrement_voltage_timer >= _time_between_decrements && _decrement_counter > 0)
    {
        _decrement_counter = _decrement_counter - _decrement_amount;
        analogWrite(DAC_PIN, _decrement_counter);
        _decrement_voltage_timer = 0;
    }
    else if (_decrement_counter <= 0)
    {
        if (!_write_high)
        {
            _write_high = true;
            _high_voltage_timer = 0;
            analogWrite(DAC_PIN, _max_dac_value);
        }
        if (_high_voltage_timer >= time_on_high_microseconds)
        {
            _write_high = false;
            _decrement_counter = (float)_max_dac_value;
        }
    }
}

void CarltonHapticMotor::zero()
{
    analogWrite(DAC_PIN, _motor_zero_value);
    analogWrite(VOLTAGE_OFFSET_WRITE_PIN, _motor_zero_value);
}

void CarltonHapticMotor::_update_sawtooth_frequency()
{
    float frequency_potentiometer = analogRead(TIME_BETWEEN_DECREMENTS_PIN);
    float new_waveform_frequency = (1 - frequency_potentiometer / _num_counts) * (_sawtooth_max_frequency - _sawtooth_min_frequency) + _sawtooth_min_frequency;
    float microseconds_per_cycle = 1e6 / new_waveform_frequency;
    float decrements_per_cycle = microseconds_per_cycle / _time_between_decrements;
    _decrement_amount = _max_dac_value / decrements_per_cycle;
}

void CarltonHapticMotor::_update_voltage_offset()
{
    int new_voltage_offset_value = analogRead(VOLTAGE_OFFSET_READ_PIN);
    analogWrite(VOLTAGE_OFFSET_WRITE_PIN, new_voltage_offset_value);
}

void CarltonHapticMotor::_update_max_dac_value()
{
    int new_max_dac_value = analogRead(MAX_VOLTAGE_PIN);
    _max_dac_value = new_max_dac_value;
}
