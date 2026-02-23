#ifndef ONLINE_DATA_H
#define ONLINE_DATA_H

#include <Arduino.h>

class OnlineData {
public:
    OnlineData(const char* ssid, const char* password, const char* scriptId);
    bool sendData(const char* timestamp, float temperature, float humidity, const char* waterState, const char* fanStatus, const char* imageLink);

private:
    const char* _ssid;
    const char* _password;
    const char* _scriptId;
    void connectWiFi();
};

#endif // ONLINE_DATA_H
