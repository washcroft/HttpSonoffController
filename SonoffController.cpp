/*
MIT License

Copyright (c) 2017 Warren Ashcroft

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "SonoffController.h"

SonoffController::SonoffController()
{
  this->_buttonDebouncer = new Bounce();
}

SonoffController::~SonoffController()
{
}

void SonoffController::setup()
{
  // Configure input pins and debouncing
  if (ENABLE_BUTTON_CONTROL) {
    pinMode(SONOFF_BUTTON_INPUT_PIN, INPUT_PULLUP);
    this->_buttonDebouncer->attach(SONOFF_BUTTON_INPUT_PIN);
    this->_buttonDebouncer->interval(5);
  }

  // Configure output pins
  pinMode(SONOFF_LED_OUTPUT_PIN, OUTPUT);
  digitalWrite(SONOFF_LED_OUTPUT_PIN, LED_BUILTIN_OFF);

  pinMode(SONOFF_RELAY_OUTPUT_PIN, OUTPUT);

  // Read persistent storage
  EEPROM.begin(512);
  this->_configLastRelayState = EEPROM.read(0); // EEPROM Byte 0 = Last Relay State

  // Set inital relay state
  bool initalRelayState = false;

  switch (RELAY_POWERUP_STATE) {      
    case 1:
      initalRelayState = true;
      break;

    case 2:
      initalRelayState = (bool)this->_configLastRelayState;
      break;
      
    case 0:
    default:
      initalRelayState = false;
      break;
  }

  this->switchRelay(initalRelayState);
}

void SonoffController::loop()
{
  // Handle debouncing
  if (ENABLE_BUTTON_CONTROL) {
    LOGPRINTLN_TRACE("Calling _debouncers.update()");
    this->_buttonDebouncer->update();

    // Handle button presses
    LOGPRINTLN_TRACE("Reading button debouncer");
    this->_buttonState = !(bool)this->_buttonDebouncer->read();

    if (this->_buttonState && this->_buttonState != this->_lastButtonState) {
      this->switchRelay(!this->RelayState);
    }

    this->_lastButtonState = this->_buttonState;
  }
}

void SonoffController::switchRelay(bool state)
{
  LOGPRINTLN_VERBOSE("Entered switchRelay()");
  bool currentState = (bool)digitalRead(SONOFF_RELAY_OUTPUT_PIN);
  this->RelayState = currentState;

  if (currentState != state) {
    LOGPRINT_INFO("\nOUTPUT: Switching relay state to: ");
    LOGPRINTLN_INFO(state);

    // EEPROM Byte 0 = Last State
    this->_configLastRelayState = (uint8_t)state;

    EEPROM.write(0, this->_configLastRelayState);
    EEPROM.commit();

    this->RelayState = state;
    digitalWrite(SONOFF_RELAY_OUTPUT_PIN, state);
    digitalWrite(SONOFF_LED_OUTPUT_PIN, (state ? LED_BUILTIN_ON : LED_BUILTIN_OFF));
  }
}

void SonoffController::getJsonStatus(char *const dest, size_t destSize)
{
  LOGPRINTLN_VERBOSE("Entered getJsonStatus()");
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &jsonRoot = jsonBuffer.createObject();

  jsonRoot["result"] = 200;
  jsonRoot["success"] = true;
  jsonRoot["message"] = "OK";
  jsonRoot["relay-state"] = this->RelayState;

  jsonRoot.printTo(dest, destSize);
}

uint16_t SonoffController::requestHandler(Client &client, const char *requestMethod, const char *requestUrl, HashMap<char *, char *, 24> &requestQuery)
{
  LOGPRINTLN_VERBOSE("Entered _requestHandler()");

  if (strcmp(requestMethod, "GET") == 0) {
    if (strcasecmp(requestUrl, "/controller") == 0) {
      char jsonStatus[256];
      this->getJsonStatus(jsonStatus, sizeof(jsonStatus));
      HttpWebServer::send_response(client, 200, (uint8_t *)jsonStatus, strlen(jsonStatus));
      return 0;

    } else {
      return 404;
    }
  } else if (strcmp(requestMethod, "PUT") == 0) {
    if ((strcasecmp(requestUrl, "/controller/relay/on") == 0) || (strcasecmp(requestUrl, "/controller/relay/off") == 0)) {
      bool state = false;

      if (striendswith(requestUrl, "/on")) {
        state = true;
      } else if (striendswith(requestUrl, "off")) {
        state = false;
      }

      // Switch the relay
      this->switchRelay(state);
      return 202;

    } else {
      return 404;
    }
  } else {
    return 405;
  }
}
