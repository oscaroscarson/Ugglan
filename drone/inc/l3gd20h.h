/*
L3GD20H gyro sensor, see data sheet
https://www.pololu.com/file/download/L3GD20H.pdf?file_id=0J731
*/

#ifndef L3GD20H_H
#define L3GD20H_H

#include <cstdint>
#include <pololu_alt_imu.h>

static const uint8_t L3GD20H_I2C_ADDRESS = 0x6B;

// Register addresses
static const uint8_t L3GD20H_CTRL1       = 0x20;
static const uint8_t L3GD20H_CTRL4       = 0x23;
static const uint8_t L3GD20H_OUT_X_L     = 0x28;
static const uint8_t L3GD20H_OUT_X_H     = 0x29;
static const uint8_t L3GD20H_OUT_Y_L     = 0x2A;
static const uint8_t L3GD20H_OUT_Y_H     = 0x2B;
static const uint8_t L3GD20H_OUT_Z_L     = 0x2C;
static const uint8_t L3GD20H_OUT_Z_H     = 0x2D;

// Buffer indices and size
static const uint8_t L3GD20H_BUF_SIZE    = 6;
static const uint8_t L3GD20H_BUF_OUT_X_L = 0x00;
static const uint8_t L3GD20H_BUF_OUT_X_H = 0x01;
static const uint8_t L3GD20H_BUF_OUT_Y_L = 0x02;
static const uint8_t L3GD20H_BUF_OUT_Y_H = 0x03;
static const uint8_t L3GD20H_BUF_OUT_Z_L = 0x04;
static const uint8_t L3GD20H_BUF_OUT_Z_H = 0x05;

// Scaling and resolution
static const double L3GD20H_GYRO_SCALE = 8.7267; // (500 / 180 * pi) [rad/s]
static const double L3GD20H_GYRO_RESOLUTION = 32767; // 2^15-1 (16-bit signed integer)

// Config and read map
static const ConfigMap L3GD20H_CONFIG_MAP = {
    {L3GD20H_CTRL1, 0b01011111}, // Enable XYZ gyro axis at 200 Hz, normal mode
    {L3GD20H_CTRL4, 0b00010000}  // Continues data with scaling 500 deg/s
    };

static const ReadMap L3GD20H_READ_MAP = {
    {L3GD20H_OUT_X_L, L3GD20H_BUF_SIZE}
    };

class L3gd20h : public PololuAltImu
{
public:
    L3gd20h(I2cConn* i2c_conn);

    double get_angular_rate_x();
    double get_angular_rate_y();
    double get_angular_rate_z();
};

#endif
