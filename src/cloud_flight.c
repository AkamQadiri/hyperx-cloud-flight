#include "cloud_flight.h"
#include <stdio.h>
#include <stdlib.h>

const uint16_t CLOUD_FLIGHT_PRODUCT_IDS[CLOUD_FLIGHT_PRODUCT_IDS_COUNT] = {0x1723, 0x16c4};
const uint8_t CLOUD_FLIGHT_BATTERY_TRIGGER_PACKET[CLOUD_FLIGHT_BATTERY_TRIGGER_PACKET_SIZE] = {0x21, 0xff, 0x05};

typedef struct
{
    uint16_t below;
    uint8_t percent;
} BatteryRange;

/* Raw value thresholds mapped to a percentage; 256 caps the last range. */
static const BatteryRange LOW_CHARGE_RANGES[] = {
    {90, 10}, {120, 15}, {149, 20}, {160, 25}, {170, 30}, {180, 35},
    {190, 40}, {200, 45}, {210, 50}, {220, 55}, {240, 60}, {256, 65},
};

static const BatteryRange HIGH_CHARGE_RANGES[] = {
    {20, 70}, {50, 75}, {70, 80}, {100, 85}, {120, 90}, {130, 95}, {256, 100},
};

static void print_hid_error(const char *message, hid_device *device)
{
    const wchar_t *detail = hid_error(device);
    fprintf(stderr, "%s: %ls\n", message, detail ? detail : L"unknown error");
}

static uint8_t get_battery_percentage(uint8_t charge_state, uint8_t value)
{
    const BatteryRange *range = charge_state == CLOUD_FLIGHT_HIGH_CHARGE ? HIGH_CHARGE_RANGES : LOW_CHARGE_RANGES;

    while (value >= range->below)
    {
        range++;
    }

    return range->percent;
}

CloudFlight *cloud_flight_new(void)
{
    if (hid_init())
    {
        fprintf(stderr, "Failed to initialize HIDAPI\n");
        return NULL;
    }

    CloudFlight *cf = calloc(1, sizeof *cf);
    if (cf == NULL)
    {
        perror("Failed to allocate memory");
        hid_exit();
        return NULL;
    }

    for (uint8_t i = 0; cf->device == NULL && i < CLOUD_FLIGHT_PRODUCT_IDS_COUNT; i++)
    {
        cf->device = hid_open(CLOUD_FLIGHT_VENDOR_ID, CLOUD_FLIGHT_PRODUCT_IDS[i], NULL);
    }

    if (cf->device == NULL)
    {
        fprintf(stderr, "Failed to open HID device\n");
        free(cf);
        hid_exit();
        return NULL;
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
    }

    free(cf);
    hid_exit();
}

CloudFlightEvent cloud_flight_read(CloudFlight *cf)
{
    uint8_t buf[32];
    int bytes = hid_read_timeout(cf->device, buf, sizeof(buf), 250);
    if (bytes < 0)
    {
        print_hid_error("Failed to read from HID device", cf->device);
        return CLOUD_FLIGHT_EVENT_ERROR;
    }

    if (bytes == 2)
    {
        uint8_t packet = buf[0];
        uint8_t value = buf[1];

        if (packet == CLOUD_FLIGHT_POWER_PACKET)
        {
            if (value == CLOUD_FLIGHT_POWER_ON)
            {
                cf->state.powered = true;
                if (cloud_flight_send_battery_trigger_packet(cf) < 0)
                {
                    return CLOUD_FLIGHT_EVENT_ERROR;
                }
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

        if (packet == CLOUD_FLIGHT_LOW_CHARGE || packet == CLOUD_FLIGHT_HIGH_CHARGE)
        {
            cf->state.charging = false;
            cf->state.battery = get_battery_percentage(packet, value);
            return CLOUD_FLIGHT_EVENT_BATTERY;
        }
    }

    return CLOUD_FLIGHT_EVENT_IGNORED;
}

int cloud_flight_send_battery_trigger_packet(CloudFlight *cf)
{
    int bytes = hid_write(cf->device, CLOUD_FLIGHT_BATTERY_TRIGGER_PACKET, CLOUD_FLIGHT_BATTERY_TRIGGER_PACKET_SIZE);
    if (bytes < 0)
    {
        print_hid_error("Failed to write to HID device", cf->device);
        return -1;
    }

    return 0;
}
