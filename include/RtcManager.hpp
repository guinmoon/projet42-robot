#ifndef RTC_MANAGER_HPP
#define RTC_MANAGER_HPP

#include <Wire.h>
#include <RtcDS3231.h>

class Proj42;

class RtcManager {
private:
    RtcDS3231<TwoWire>* rtc;
    
    void printDateTime(const RtcDateTime& dt);
    void setProj42DateTime(const RtcDateTime& dt);
    bool wasError(const char* errorTopic = "");
    
public:
    static Proj42 *proj42;
    RtcManager(Proj42 *_proj42);
    void setup();
    void loop();
    void SetDateTime(const char* dateTimeStr);
    void SetDateTime(char* dateTimeStr);
};

#endif // RTC_MANAGER_HPP