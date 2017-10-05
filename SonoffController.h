#ifndef SONOFFCONTROLLER_H
#define SONOFFCONTROLLER_H

// ARDUINO LIBRARIES
#include <Arduino.h>
#include <Client.h>
#include <EEPROM.h>

#include <Bounce2.h>
#include <ArduinoJson.h>

// INCLUDES
#include "compile.h"
#include "config.h"
#include "secret.h"

#include "src/HashMap.h"
#include "src/Utilities/Utilities.h"
#include "src/HttpWebServer/HttpWebServer.h"

#define SONOFF_BUTTON_INPUT_PIN  0
#define SONOFF_RELAY_OUTPUT_PIN  12
#define SONOFF_LED_OUTPUT_PIN    13

#undef  LED_BUILTIN
#define LED_BUILTIN SONOFF_LED_OUTPUT_PIN

class SonoffController
{
  public:
    SonoffController();
    ~SonoffController();

    void setup();
    void loop();
    void switchRelay(bool state);
    void getJsonStatus(char *const dest, size_t destSize);
    uint16_t requestHandler(Client &client, const char *requestMethod, const char *requestUrl, HashMap<char *, char *, 24> &requestQuery);

    bool RelayState;

  private:
    Bounce *_buttonDebouncer;

    bool _buttonState;
    bool _lastButtonState;
    uint8_t _configLastRelayState;
};

#endif // SONOFFCONTROLLER_H

