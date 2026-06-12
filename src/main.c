#include "cloud_flight.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SEND_BATTERY_TRIGGER_PACKET_INTERVAL_MS 180000
#define BATTERY_REPORT_TIMEOUT_MS 1000

static int64_t monotonic_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static void print_usage(FILE *stream, const char *name)
{
    fprintf(stream,
            "Usage: %s [-r] [-h]\n"
            "  -r  print the current battery status once and exit\n"
            "  -h  show this help\n"
            "Without options, monitor events until the headset powers off.\n",
            name);
}

static bool handle_event(const CloudFlight *cf, CloudFlightEvent event)
{
    switch (event)
    {
    case CLOUD_FLIGHT_EVENT_BATTERY_CHARGING:
        printf("Battery: Charging\n");
        break;
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
        return false;
    case CLOUD_FLIGHT_EVENT_POWER_ON:
        printf("Power: On\n");
        break;
    default:
        break;
    }

    return true;
}

static int read_battery_once(CloudFlight *cf)
{
    if (cloud_flight_send_battery_trigger_packet(cf) < 0)
    {
        return EXIT_FAILURE;
    }

    int64_t deadline = monotonic_ms() + BATTERY_REPORT_TIMEOUT_MS;

    while (monotonic_ms() < deadline)
    {
        CloudFlightEvent event = cloud_flight_read(cf);

        if (event == CLOUD_FLIGHT_EVENT_ERROR)
        {
            return EXIT_FAILURE;
        }

        if (event == CLOUD_FLIGHT_EVENT_BATTERY || event == CLOUD_FLIGHT_EVENT_BATTERY_CHARGING)
        {
            handle_event(cf, event);
            return EXIT_SUCCESS;
        }
    }

    fprintf(stderr, "Timed out waiting for a battery report\n");
    return EXIT_FAILURE;
}

static int monitor(CloudFlight *cf)
{
    /* Backdated so the first iteration sends a trigger immediately. */
    int64_t last_battery_trigger_ms = monotonic_ms() - SEND_BATTERY_TRIGGER_PACKET_INTERVAL_MS;

    for (;;)
    {
        int64_t now = monotonic_ms();
        if (now - last_battery_trigger_ms >= SEND_BATTERY_TRIGGER_PACKET_INTERVAL_MS)
        {
            if (cloud_flight_send_battery_trigger_packet(cf) < 0)
            {
                return EXIT_FAILURE;
            }
            last_battery_trigger_ms = now;
        }

        CloudFlightEvent event = cloud_flight_read(cf);

        if (event == CLOUD_FLIGHT_EVENT_ERROR)
        {
            return EXIT_FAILURE;
        }

        if (!handle_event(cf, event))
        {
            return EXIT_SUCCESS;
        }
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
            print_usage(stdout, argv[0]);
            return EXIT_SUCCESS;
        default:
            print_usage(stderr, argv[0]);
            return EXIT_FAILURE;
        }
    }

    setvbuf(stdout, NULL, _IOLBF, 0);

    CloudFlight *cf = cloud_flight_new();
    if (cf == NULL)
    {
        return EXIT_FAILURE;
    }

    int status = rflag ? read_battery_once(cf) : monitor(cf);

    cloud_flight_free(cf);

    return status;
}
