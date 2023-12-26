#ifndef CLOUD_FLIGHT_H
#define CLOUD_FLIGHT_H

#include <stdint.h>
#include <stdbool.h>
#include "hidapi/hidapi.h"

#define CLOUD_FLIGHT_VENDOR_ID 0x0951
#define CLOUD_FLIGHT_PRODUCT_IDS_COUNT 2
#define CLOUD_FLIGHT_BATTERY_TRIGGER_PACKET_SIZE 20

#define CLOUD_FLIGHT_LOW_CHARGE 0x0e
#define CLOUD_FLIGHT_HIGH_CHARGE 0x0f
#define CLOUD_FLIGHT_POWER_PACKET 0x64
#define CLOUD_FLIGHT_MICROPHONE_PACKET 0x65
#define CLOUD_FLIGHT_POWER_ON 0x01
#define CLOUD_FLIGHT_POWER_OFF 0x03
#define CLOUD_FLIGHT_VOLUME_DOWN 0x02
#define CLOUD_FLIGHT_VOLUME_UP 0x01
#define CLOUD_FLIGHT_MUTED 0x04
#define CLOUD_FLIGHT_UNMUTED 0x00
#define CLOUD_FLIGHT_BATTERY 0x10
#define CLOUD_FLIGHT_BATTERY1 0x11

extern const uint16_t CLOUD_FLIGHT_PRODUCT_IDS[CLOUD_FLIGHT_PRODUCT_IDS_COUNT];
extern const uint8_t CLOUD_FLIGHT_BATTERY_TRIGGER_PACKET[CLOUD_FLIGHT_BATTERY_TRIGGER_PACKET_SIZE];

typedef enum
{
    CLOUD_FLIGHT_EVENT_BATTERY,
    CLOUD_FLIGHT_EVENT_BATTERY_CHARGING,
    CLOUD_FLIGHT_EVENT_VOLUME_UP,
    CLOUD_FLIGHT_EVENT_VOLUME_DOWN,
    CLOUD_FLIGHT_EVENT_MUTED,
    CLOUD_FLIGHT_EVENT_UNMUTED,
    CLOUD_FLIGHT_EVENT_POWER_OFF,
    CLOUD_FLIGHT_EVENT_POWER_ON,
    CLOUD_FLIGHT_EVENT_IGNORED
} CloudFlightEvent;

typedef struct
{
    bool powered;
    bool muted;
    bool charging;
    uint8_t battery;
} CloudFlightState;

typedef struct
{
    CloudFlightState state;
    hid_device *device;
} CloudFlight;

CloudFlight *cloud_flight_new();
void cloud_flight_free(CloudFlight *cf);
CloudFlightEvent cloud_flight_read(CloudFlight *cf);
void cloud_flight_send_battery_trigger_packet(CloudFlight *cf);

#endif
