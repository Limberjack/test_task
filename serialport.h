#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <error.h>

#include <stdio.h>
#include <string.h>
#include <iostream>

class SerialPort
{
    unsigned char serial_port = -1;
    struct termios tty;
    char message[512];

public:
    SerialPort();
    ~SerialPort();
    void send(const char * const command);
    char* reseave();
    char* reseave(char* message, int size);
    int begin(int portName);
    void stop();
};

#endif // SERIALPORT_H
