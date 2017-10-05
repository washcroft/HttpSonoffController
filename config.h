#ifndef CONFIG_H
#define CONFIG_H

// Controller Config
const char MDNS_NAME[] = "sonoffcontroller-kitchenplinth";
const char FRIENDLY_NAME[] = "Kitchen Plinth Lighting";
const char FRIENDLY_TYPE[] = "Light";
const bool ENABLE_BUTTON_CONTROL = true;
const uint8_t RELAY_POWERUP_STATE = 1;

// ADVANCED - DEFAULTS ARE NORMALLY OK
// Network Config
const uint16_t HTTP_SERVER_PORT = 80;
const uint16_t HTTP_REQUEST_TIMEOUT = 4000;
const uint16_t HTTP_REQUEST_BUFFER_SIZE = 512;
const uint16_t WIFI_CONNECTION_TIMEOUT = 10000;

// OAuth Config
const uint16_t OAUTH_NONCE_SIZE = 24;
const uint16_t OAUTH_NONCE_HISTORY = 255;
const uint16_t OAUTH_TIMESTAMP_VALIDITY_WINDOW = 300000;

#endif // CONFIG

