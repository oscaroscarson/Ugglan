#ifndef ATTITUDE_ESTIMATION_H
#define ATTITUDE_ESTIMATION_H

#include <cstdint>
#include <math.h>

inline const uint8_t ATT_EST_N_SAMPLES_GYRO_OFFSET_COMP = 20;

inline const double ATT_EST_CUT_OFF_FREQ = 0.2; // [Hz]

inline const double ATT_EST_TAU_ROLL = 1 / (2 * M_PI * ATT_EST_CUT_OFF_FREQ);
inline const double ATT_EST_TAU_PITCH = 1 / (2 * M_PI * ATT_EST_CUT_OFF_FREQ);
inline const double ATT_EST_TAU_YAW = 1 / (2 * M_PI * ATT_EST_CUT_OFF_FREQ);

inline const double ATT_EST_MODULO_ROLL = M_PI;
inline const double ATT_EST_MODULO_PITCH = M_PI / 2;
inline const double ATT_EST_MODULO_YAW = M_PI;

inline const double ATT_EST_HARD_IRON_OFFSET_X = 0.131;
inline const double ATT_EST_HARD_IRON_OFFSET_Y = 0.143;
inline const double ATT_EST_HARD_IRON_OFFSET_Z = -0.144;

struct AttEstInput {
    double acc_x;       // [m/s^2]
    double acc_y;       // [m/s^2]
    double acc_z;       // [m/s^2]
    double ang_rate_x;  // [rad/s]
    double ang_rate_y;  // [rad/s]
    double ang_rate_z;  // [rad/s]
    double mag_field_x; // [gauss]
    double mag_field_y; // [gauss]
    double mag_field_z; // [gauss]
};

struct AttEstimate {
    double roll;       // [rad]
    double pitch;      // [rad]
    double yaw;        // [rad]
    double roll_rate;  // [rad/s]
    double pitch_rate; // [rad/s]
    double yaw_rate;   // [rad/s]
};

class AttitudeEstimation
{
public:
    AttitudeEstimation(double input_sample_rate_s);

    void update(AttEstInput input);

    AttEstimate get_estimate();

    bool is_calibrated();
private:
    const double _sample_rate_s;

    AttEstInput _in = {0};
    AttEstimate _est = {0};

    double _gyro_offset_x = 0;
    double _gyro_offset_y = 0;
    double _gyro_offset_z = 0;

    bool _is_gyro_offset_comp = false;
    uint8_t _samples_gyro_offset_comp = 0;

    void _gyro_offset_comp();
    void _hard_iron_offset_comp();

    void _update_roll();
    void _update_pitch();
    void _update_yaw();

    double _complementary_filter(double y_old, double u, double up, double tau);
    double _modulo_angle(double angle, double limit);
};

#endif /* ATTITUDE_ESTIMATION_H */
