#ifndef CarltonHapticMotor_h
#define CarltonHapticMotor_h
#include <Arduino.h>

#define DAC_PIN A21                     ///< DAC pin on the Teensy 3.6; writes a voltage which linearly corresponds to a current
#define MAX_VOLTAGE_PIN A15             ///< pin connected to a potentiometer that changes the variable @c _max_dac_value
#define TIME_BETWEEN_DECREMENTS_PIN A16 ///< pin connected to a potentiometer that changes the frequency of the sawtooth
#define VOLTAGE_OFFSET_READ_PIN A17     ///< pin connected to a potentiometer that sets the voltage offset of the waveform
#define VOLTAGE_OFFSET_WRITE_PIN A19    ///< pin that writes the voltage offset to the difference amplifier
#define ANALOG_RESOLUTION 10            ///< analog read / write resolution

class CarltonHapticMotor
{

public:
    /**
     * @brief Constructor.
     *
     * Sets the analog write / read resolution and various pins' modes, as well as zeros the motor.
     */
    CarltonHapticMotor();

    /**
     * @brief Sends a user-controllable sawtooth current waveform to the motor.
     *
     * The user can send a user-controllable sawtooth current profile. This is what was used in the paper and video demos.
     * Potentiometers are used to change the height of the sawtooth ( @c _max_dac_value), the frequency ( @c _decrement_amount),
     * and the offset of the waveform ( @c new_voltage_offset_value). This function works by first writing @c _max_dac_value and
     * then decrementing an amount ( @c _decrement_amount) based on the desired frequency and the value @c _time_between_decrements.
     * This resets once @c _decrement_counter becomes less than zero (and the @c time_on_high_microseconds value is surpassed).
     *
     * @param time_on_high_microseconds How many microseconds to write @c _max_dac_value before decrementing again.
     * @param time_between_decrements_microseconds How many microseconds to wait before decrementing a value of 1 from @c _decrement_counter;
     * essentially controls the frequency of the sawtooth wave.
     */
    void sawtooth(unsigned long time_on_high_microseconds = 1, float time_between_decrements_microseconds = -1);

    /**
     * @brief Sets the offset voltage and dac write value to the midpoint of the write values to stop the motor.
     */
    void zero();

private:
    float _max_pwm_pin_write_voltage = 3.3;       ///< [V] max voltage output from DAC
    float _max_diff_amplifier_voltage = 5.27;     ///< [V] maximum voltage before power amp (output of difference amp)
    float _diff_amp_gain = 15000.0 / 4700.0;      ///< difference amplifier gain (using 15k and 4.7k ohm resistors)
    int _analog_res = ANALOG_RESOLUTION;          ///< set the resolution
    int _num_counts = pow(2, _analog_res);        ///< number of bits of analog reads based on @c _analog_res
    int _max_dac_value = pow(2, _analog_res) - 1; ///< controls the peak-to-peak voltage of the sawtooth waveform written to the motor
    int _motor_zero_value = _max_dac_value / 2;   ///< half of the maximum DAC write value to get zero current flowing through the motor

    /* Sawtooth Wave Parameters */
    // these 3 parameters control the frequency of the sawtooth wave
    float _time_between_decrements = 50;    ///< [µs]
    float _decrement_counter = 0;           ///< the number of counts to write (corresponds to the current to send to the motor)
    float _decrement_amount = 1;            ///< how much to decrement the counter every '_time_between_decrements' steps
    elapsedMicros _decrement_voltage_timer; ///< to consistently write

    bool _write_high = false;          ///< boolean to control if we are writing high or not
    elapsedMicros _high_voltage_timer; ///< timer to keep track of how long we have written the @c _max_dac_value for

    // range of frequencies that can be written to the motor by turning the corresponding potentiometer
    float _sawtooth_min_frequency = 1;   ///< [Hz] - determines lower limit from frequency input potentiometer
    float _sawtooth_max_frequency = 100; ///< [Hz] - determines upper limit from frequency input potentiometer

    /**
     * @brief Potentiometer read that applies an offset to the difference amplifier to shift the sawtooth waveform up and down.
     */
    void _update_voltage_offset();

    /**
     * @brief Potentiometer read that controls the amplitude of the sawtooth waveform.
     */
    void _update_max_dac_value();

    /**
     * @brief Updates the variable @c _decrement_amount' which corresponds to the frequency of the sawtooth waveform.
     */
    void _update_sawtooth_frequency();
};
#endif