#include "cloud_flight.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SEND_BATTERY_TRIGGER_PACKET_INTERVAL 180

void handle_event(CloudFlight *cf, CloudFlightEvent event, bool *keepRunning)
{
    switch (event)
    {
    case CLOUD_FLIGHT_EVENT_BATTERY_CHARGING:
    case CLOUD_FLIGHT_EVENT_BATTERY:
        printf("Battery: %i%%\n", cf->state.battery);
        break;
    case CLOUD_FLIGHT_EVENT_VOLUME_UP:
        printf("Volume: Increased\n");
        break;
    case CLOUD_FLIGHT_EVENT_VOLUME_DOWN:
        printf("Volume: Decreased\n");
        break;
    case CLOUD_FLIGHT_EVENT_MUTED:
    case CLOUD_FLIGHT_EVENT_UNMUTED:
        printf("Muted: %s\n", cf->state.muted ? "Yes" : "No");
        break;
    case CLOUD_FLIGHT_EVENT_POWER_OFF:
        printf("Power: Off\n");
        *keepRunning = false;
        break;
    case CLOUD_FLIGHT_EVENT_POWER_ON:
        printf("Power: On\n");
        break;
    default:
        break;
    }
}

int main(int argc, char *argv[])
{
    int opt, rflag = 0;

    while ((opt = getopt(argc, argv, "rh")) != -1)
    {
        switch (opt)
        {
        case 'r':
            rflag = 1;
            break;
        case 'h':
            printf("Usage: %s\n"
                   "[-r] read and quit\n",
                   argv[0]);
            return 0;
        }
    }

    if (hid_init())
    {
        fprintf(stderr, "Failed to initialize HIDAPI\n");
        return EXIT_FAILURE;
    }

    CloudFlight *cf = cloud_flight_new();
    time_t lastBatteryTriggerTime = 0;
    bool keepRunning = !rflag;

    do
    {
        time_t currentTime = time(NULL);
        if (currentTime - lastBatteryTriggerTime >= SEND_BATTERY_TRIGGER_PACKET_INTERVAL)
        {
            cloud_flight_send_battery_trigger_packet(cf);
            lastBatteryTriggerTime = currentTime;
        }

        handle_event(cf, cloud_flight_read(cf), &keepRunning);
    } while (keepRunning);

    cloud_flight_free(cf);
    hid_close(cf->device);
    hid_exit();

    return 0;
}