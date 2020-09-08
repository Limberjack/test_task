#include "serialport.h"

SerialPort::SerialPort()
{
    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    tty.c_cc[VTIME] = 3;    // Wait for up to 0.3s (3 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 9600
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

}

int SerialPort::begin(int portName)
{
    if(serial_port >= 0)
        close(serial_port);
    char name[] = "/dev/ttyUSB_";
    name[11] = portName + '0';
    serial_port = open(name, O_RDWR);
    if(tcgetattr(serial_port, &tty) != 0) {
        throw std::runtime_error(strerror(errno));
    }

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        throw std::runtime_error(strerror(errno));
    }
    return serial_port;
}
void SerialPort::send(char const * const command)
{
    write(serial_port, command, sizeof(command));
}

char* SerialPort::reseave(){
    memset(&message, '\0', sizeof(message));
    read(serial_port, message, sizeof(message));
    if (read(serial_port, &message, sizeof(message)) < 0) {
        throw std::runtime_error(strerror(errno));
    }
    return message;
}
char* SerialPort::reseave(char* message, int size){
    memset(&this->message, '\0', sizeof(this->message));
    read(serial_port, this->message, sizeof(this->message));
    if (read(serial_port, &this->message, sizeof(this->message)) < 0) {
        throw std::runtime_error(strerror(errno));
    }
    for(int i = 0; this->message[i] != '\0' && i < size - 1; i++)
            message[i] = this->message[i];
    message[size - 1] = '\0';
    return this->message;
}

SerialPort::~SerialPort(){
    close(serial_port);
}

void SerialPort::stop(){
    close(serial_port);
}
