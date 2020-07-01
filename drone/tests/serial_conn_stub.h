#ifndef SERIAL_CONN_STUB_H
#define SERIAL_CONN_STUB_H

#include <serial_conn.h>

#include <stdio.h>
#include <fcntl.h>
#include <cstdint>
#include <cstring>
#include <cassert>

static const uint32_t MAX_BUF_SIZE = 4096;

class SerialConn
{
public:
    SerialConn();

    void set_read_buf(uint8_t* buf);

    bool open(Mode mode, ControlFlags flags);

    uint32_t read(uint8_t* buf);
private:
    uint8_t _read_buf[MAX_BUF_SIZE] = {0};
    uint32_t _read_buf_size = 0;
};

#endif /* SERIAL_CONN_STUB_H */