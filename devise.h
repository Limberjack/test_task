#ifndef DEVISE_H
#define DEVISE_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <array>


#include "serialport.h"

#define IMSI_SIZE 16
#define AT_QENG_SIZE 512

class Devise
{
    SerialPort port;
    char IMSI[IMSI_SIZE];
    char AT_QENG[AT_QENG_SIZE];
    std::mutex mut;
    std::condition_variable cond;
public:
    Devise();
    void begin();
    void stop();

private:
    void display_info();
};

#endif // DEVISE_H
