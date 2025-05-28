#include "cloud_flight.h"
#include <stdio.h>
#include <stdlib.h>

const uint16_t CLOUD_FLIGHT_PRODUCT_IDS[CLOUD_FLIGHT_PRODUCT_IDS_COUNT] = {0x1723, 0x16c4};
const uint8_t CLOUD_FLIGHT_BATTERY_TRIGGER_PACKET[CLOUD_FLIGHT_BATTERY_TRIGGER_PACKET_SIZE] = {0x21, 0xff, 0x05};

uint8_t get_battery_percentage(uint8_t charge_state, uint8_t value)
{
    if (charge_state == CLOUD_FLIGHT_LOW_CHARGE)
    {
        if (value < 90)
            return 10;
        if (value < 120)
            return 15;
        if (value < 150)
            return 20;
        if (value < 160)
            return 25;
        if (value < 170)
            return 30;
        if (value < 180)
            return 35;
        if (value < 190)
            return 40;
        if (value < 200)
            return 45;
        if (value < 210)
            return 50;
        if (value < 220)
            return 55;
        if (value < 240)
            return 60;
        return 65;
    }
    else if (charge_state == CLOUD_FLIGHT_HIGH_CHARGE)
    {
        if (value < 20)
            return 70;
        if (value < 50)
            return 75;
        if (value < 70)
            return 80;
        if (value < 100)
            return 85;
        if (value < 120)
            return 90;
        if (value < 130)
            return 95;
        return 100;
    }

    return 0;
}

CloudFlight *cloud_flight_new()
{
    CloudFlight *cf = (CloudFlight *)malloc(sizeof(CloudFlight));
    if (cf == NULL)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    cf->state.powered = false;
    cf->state.muted = false;
    cf->state.charging = false;
    cf->state.battery = 0;

    uint8_t pii = 0;
    uint8_t products = CLOUD_FLIGHT_PRODUCT_IDS_COUNT;
    while (cf->device == NULL && pii < products)
    {
        cf->device = hid_open(CLOUD_FLIGHT_VENDOR_ID, CLOUD_FLIGHT_PRODUCT_IDS[pii], NULL);
        pii++;
    }

    if (cf->device == NULL)
    {
        fprintf(stderr, "Failed to open HID device\n");
        exit(EXIT_FAILURE);
    }

    return cf;
}

void cloud_flight_free(CloudFlight *cf)
{
    if (cf == NULL)
    {
        return;
    }

    if (cf->device != NULL)
    {
        hid_close(cf->device);
        hid_exit();
    }

    free(cf);
}

CloudFlightEvent cloud_flight_read(CloudFlight *cf)
{
    uint8_t buf[32];
    int bytes = hid_read_timeout(cf->device, buf, sizeof(buf), 250);
    if (bytes < 0)
    {
        fprintf(stderr, "Error reading from HID device\n");
        return CLOUD_FLIGHT_EVENT_IGNORED;
    }

    if (bytes == 2)
    {
        uint8_t packet = buf[0];
        uint8_t value = buf[1];

        if (packet == CLOUD_FLIGHT_POWER_PACKET)
        {
            if (value == CLOUD_FLIGHT_POWER_ON)
            {
                cloud_flight_send_battery_trigger_packet(cf);
                cf->state.powered = true;
                return CLOUD_FLIGHT_EVENT_POWER_ON;
            }
            else if (value == CLOUD_FLIGHT_POWER_OFF)
            {
                cf->state.powered = false;
                return CLOUD_FLIGHT_EVENT_POWER_OFF;
            }
        }
        else if (packet == CLOUD_FLIGHT_MICROPHONE_PACKET)
        {
            if (value == CLOUD_FLIGHT_MUTED)
            {
                cf->state.muted = true;
                return CLOUD_FLIGHT_EVENT_MUTED;
            }
            else if (value == CLOUD_FLIGHT_UNMUTED)
            {
                cf->state.muted = false;
                return CLOUD_FLIGHT_EVENT_UNMUTED;
            }
        }
    }
    else if (bytes == 5)
    {
        uint8_t value = buf[1];

        if (value == CLOUD_FLIGHT_VOLUME_UP)
        {
            return CLOUD_FLIGHT_EVENT_VOLUME_UP;
        }
        else if (value == CLOUD_FLIGHT_VOLUME_DOWN)
        {
            return CLOUD_FLIGHT_EVENT_VOLUME_DOWN;
        }
    }
    else if (bytes == 20)
    {
        uint8_t packet = buf[3];
        uint8_t value = buf[4];

        if (packet == CLOUD_FLIGHT_BATTERY || packet == CLOUD_FLIGHT_BATTERY1)
        {
            cf->state.charging = true;

            if (value >= 20)
            {
                return CLOUD_FLIGHT_EVENT_BATTERY_CHARGING;
            }

            cf->state.battery = 100;
            return CLOUD_FLIGHT_EVENT_BATTERY;
        }

        uint8_t charge_state = buf[3];
        uint8_t batteryPercent = get_battery_percentage(charge_state, value);
        cf->state.charging = false;
        cf->state.battery = batteryPercent;
        return CLOUD_FLIGHT_EVENT_BATTERY;
    }

    return CLOUD_FLIGHT_EVENT_IGNORED;
}

void cloud_flight_send_battery_trigger_packet(CloudFlight *cf)
{
    int bytes = hid_write(cf->device, CLOUD_FLIGHT_BATTERY_TRIGGER_PACKET, CLOUD_FLIGHT_BATTERY_TRIGGER_PACKET_SIZE);
    if (bytes < 0)
    {
        fprintf(stderr, "Error writing to HID device\n");
        exit(EXIT_FAILURE);
    }
}