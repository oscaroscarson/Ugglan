/*
LSM303D accelerometer and magnetometer, see data sheet
https://www.pololu.com/file/download/LSM303D.pdf?file_id=0J703
*/

#ifndef LSM303D_H
#define LSM303D_H

#include <cstdint>
#include <science_const.h>
#include <pololu_alt_imu.h>

static const uint8_t LSM303D_I2C_ADDRESS = 0x1D;

// Register addresses
static const uint8_t LSM303D_OUT_X_L_M     = 0x08;
static const uint8_t LSM303D_OUT_X_H_M     = 0x09;
static const uint8_t LSM303D_OUT_Y_L_M     = 0x0A;
static const uint8_t LSM303D_OUT_Y_H_M     = 0x0B;
static const uint8_t LSM303D_OUT_Z_L_M     = 0x0C;
static const uint8_t LSM303D_OUT_Z_H_M     = 0x0D;
static const uint8_t LSM303D_CTRL1         = 0x20;
static const uint8_t LSM303D_CTRL2         = 0x21;
static const uint8_t LSM303D_CTRL5         = 0x24;
static const uint8_t LSM303D_CTRL6         = 0x25;
static const uint8_t LSM303D_CTRL7         = 0x26;
static const uint8_t LSM303D_OUT_X_L_A     = 0x28;
static const uint8_t LSM303D_OUT_X_H_A     = 0x29;
static const uint8_t LSM303D_OUT_Y_L_A     = 0x2A;
static const uint8_t LSM303D_OUT_Y_H_A     = 0x2B;
static const uint8_t LSM303D_OUT_Z_L_A     = 0x2C;
static const uint8_t LSM303D_OUT_Z_H_A     = 0x2D;

// Buffer indices and size
static const uint8_t LSM303D_BUF_ACC_SIZE  = 6;
static const uint8_t LSM303D_BUF_OUT_X_L_A = 0x00;
static const uint8_t LSM303D_BUF_OUT_X_H_A = 0x01;
static const uint8_t LSM303D_BUF_OUT_Y_L_A = 0x02;
static const uint8_t LSM303D_BUF_OUT_Y_H_A = 0x03;
static const uint8_t LSM303D_BUF_OUT_Z_L_A = 0x04;
static const uint8_t LSM303D_BUF_OUT_Z_H_A = 0x05;

static const uint8_t LSM303D_BUF_MAG_SIZE  = 6;
static const uint8_t LSM303D_BUF_OUT_X_L_M = 0x00;
static const uint8_t LSM303D_BUF_OUT_X_H_M = 0x01;
static const uint8_t LSM303D_BUF_OUT_Y_L_M = 0x02;
static const uint8_t LSM303D_BUF_OUT_Y_H_M = 0x03;
static const uint8_t LSM303D_BUF_OUT_Z_L_M = 0x04;
static const uint8_t LSM303D_BUF_OUT_Z_H_M = 0x05;

// Scaling and resolution
static const double LSM303D_ACC_SCALE = 4 * GRAVITY_EARTH; // [m/s]
static const double LSM303D_ACC_RESOLUTION = 32767; // 2^15-1 (16-bit signed integer)

static const double LSM303D_MAG_SCALE = 4; // [gauss]
static const double LSM303D_MAG_RESOLUTION = 32767; // 2^15-1 (16-bit signed integer)

// Config and read map
static const ConfigMap LSM303D_CONFIG_MAP = {
    {LSM303D_CTRL1, 0b01110111}, // Enable XYZ accelerometer axis for continues output at 200 Hz
    {LSM303D_CTRL2, 0b00001000}, // Accelerometer scale selection, ± 4 g
    {LSM303D_CTRL5, 0b01110000}, // Enable magnetometer at 50 Hz in high resolution mode
    {LSM303D_CTRL6, 0b00100000}, // Magnetometer scale selection, ± 4 gauss
    {LSM303D_CTRL7, 0b00000000}  // Magnetometer normal, continuous-conversion mode
    };

static const ReadMap LSM303D_READ_MAP = {
    {LSM303D_OUT_X_L_A, LSM303D_BUF_ACC_SIZE},
    {LSM303D_OUT_X_L_M, LSM303D_BUF_MAG_SIZE},
    };

class Lsm303d : public PololuAltImu
{
public:
    Lsm303d(I2cConn* i2c_conn);

    double get_acceleration_x();
    double get_acceleration_y();
    double get_acceleration_z();

    double get_magnetic_field_x();
    double get_magnetic_field_y();
    double get_magnetic_field_z();
};

#endif
