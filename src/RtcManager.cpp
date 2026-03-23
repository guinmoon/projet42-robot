#include "RtcManager.hpp"

#include "proj42.hpp"


Proj42 *RtcManager::proj42;

#define countof(a) (sizeof(a) / sizeof(a[0]))

RtcManager::RtcManager(Proj42 *_proj42)  {
    proj42 = _proj42;
}

void RtcManager::printDateTime(const RtcDateTime& dt) {
    char datestring[26];    
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
    
    
}

void RtcManager::setProj42DateTime(const RtcDateTime& dt){
    snprintf_P(proj42->timeStr, 
            countof(proj42->timeStr),
            PSTR("%02u:%02u"),
            dt.Hour(),
            dt.Minute());
}

bool RtcManager::wasError(const char* errorTopic) {
    uint8_t error = rtc->LastError();
    if (error != 0) {
        // we have a communications error
        // see https://www.arduino.cc/reference/en/language/functions/communication/wire/endtransmission/
        // for what the number means
        Serial.print("[");
        Serial.print(errorTopic);
        Serial.print("] WIRE communications error (");
        Serial.print(error);
        Serial.print(") : ");

        switch (error) {
        case Rtc_Wire_Error_None:
            Serial.println("(none?!)");
            break;
        case Rtc_Wire_Error_TxBufferOverflow:
            Serial.println("transmit buffer overflow");
            break;
        case Rtc_Wire_Error_NoAddressableDevice:
            Serial.println("no device responded");
            break;
        case Rtc_Wire_Error_UnsupportedRequest:
            Serial.println("device doesn't support request");
            break;
        case Rtc_Wire_Error_Unspecific:
            Serial.println("unspecified error");
            break;
        case Rtc_Wire_Error_CommunicationTimeout:
            Serial.println("communications timed out");
            break;
        }
        return true;
    }
    return false;
}

void RtcManager::setup() {
    rtc = new RtcDS3231<TwoWire>(Wire1);
    //--------RTC SETUP ------------
    // if you are using ESP-01 then uncomment the line below to reset the pins to
    // the available pins for SDA, SCL
    // Wire.begin(0, 2); // due to limited pins, use pin 0 and 2 for SDA, SCL
    
    // rtc->Begin();
#if defined(WIRE_HAS_TIMEOUT)
    Wire.setWireTimeout(3000 /* us */, true /* reset_on_timeout */);
#endif

    // RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    // printDateTime(compiled);
    Serial.println();

    if (!rtc->IsDateTimeValid()) {
        Serial.println("!rtc->IsDateTimeValid()");
        // if (!wasError("setup IsDateTimeValid"))
        // {
        //     // Common Causes:
        //     //    1) first time you ran and the device wasn't running yet
        //     //    2) the battery on the device is low or even missing

        //     Serial.println("RTC lost confidence in the DateTime!");

        //     // following line sets the RTC to the date & time this sketch was compiled
        //     // it will also reset the valid flag internally unless the Rtc device is
        //     // having an issue

        //     rtc->SetDateTime(compiled);
        // }
    }

    if (!rtc->GetIsRunning()) {
        Serial.println("!rtc->GetIsRunning()");
        if (!wasError("setup GetIsRunning")) {
            Serial.println("RTC was not actively running, starting now");
            rtc->SetIsRunning(true);
        }
    }

    RtcDateTime now = rtc->GetDateTime();
    if (!wasError("setup GetDateTime")) {
         Serial.println("OK: GetDateTime");
        // if (now < compiled)
        // {
        //     Serial.println("RTC is older than compile time, updating DateTime");
        //     rtc->SetDateTime(compiled);
        // }
        // else if (now > compiled)
        // {
        //     Serial.println("RTC is newer than compile time, this is expected");
        // }
        // else if (now == compiled)
        // {
        //     Serial.println("RTC is the same as compile time, while not expected all is still fine");
        // }
    }

    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    rtc->Enable32kHzPin(false);
    wasError("setup Enable32kHzPin");
    rtc->SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
    wasError("setup SetSquareWavePin");

    Serial.println(F("DS3231M. good."));
}



void RtcManager::SetDateTime(const char* dateTimeStr) {
    Serial.print("Setting RTC DateTime: ");
    Serial.println(dateTimeStr);
    
    // Parse the date and time string
    // Expected format: "DD.MM.YYYY HH:MM:SS"
    int day, month, year, hour, minute, second;
    
    if (sscanf(dateTimeStr, "%d.%d.%d %d:%d:%d", &day, &month, &year, &hour, &minute, &second) == 6) {
        RtcDateTime newDateTime = RtcDateTime(year, month, day, hour, minute, second);
        
        if (newDateTime.IsValid()) {
            rtc->SetDateTime(newDateTime);
            Serial.println("RTC DateTime set successfully");
        } else {
            Serial.println("Invalid DateTime");
        }
    } else {
        Serial.println("Failed to parse DateTime string");
    }
}

void RtcManager::SetDateTime(char* dateTimeStr) {
    SetDateTime(const_cast<const char*>(dateTimeStr));
}

void RtcManager::loop() {
    if (!rtc->IsDateTimeValid()) {
        Serial.println("!rtc->IsDateTimeValid()");
        if (!wasError("loop IsDateTimeValid")) {
            // Common Causes:
            //    1) the battery on the device is low or even missing and the power line was disconnected
            Serial.println("RTC lost confidence in the DateTime!");
        }
    }

     if (!rtc->GetIsRunning()) {
        Serial.println("!rtc->GetIsRunning()");
        if (!wasError("setup GetIsRunning")) {
            Serial.println("RTC was not actively running, starting now");
            rtc->SetIsRunning(true);
        }
    }

    if (!wasError("Some Error")){
        RtcDateTime now = rtc->GetDateTime();
        Serial.println("Printing DT");
        delay(500);
        if (!wasError("loop GetDateTime")) {
            printDateTime(now);        
            Serial.println();
            setProj42DateTime(now);
        }
    }

    // RtcTemperature temp = rtc->GetTemperature();
    // if (!wasError("loop GetTemperature")) {
    //     temp.Print(Serial);
    //     // you may also get the temperature as a float and print it
    //     // Serial.print(temp.AsFloatDegC());
    //     Serial.println("C");
    // }
}