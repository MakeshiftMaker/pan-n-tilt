#include "commandParser.h"
#include "stepper.h"
#include "usart.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

#define CMD_BUFFER_SIZE 64

// Helper: Trim leading/trailing whitespace
static void trim(char *str)
{
    // Trim leading
    while (isspace((unsigned char)*str))
        str++;

    // Trim trailing
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    *(end + 1) = '\0';
}

static void handleCommand(const char *cmd, Stepper *pan, Stepper *tilt)
{
    int az = -1;             // -1 means "not specified"
    int el = -1;
    int angles[4];
    const char *az_ptr = NULL;
    const char *el_ptr = NULL;
    bool valid = false;

    // Check for status command first
    if (strcasecmp(cmd, "STATUS") == 0)
    {
        stepper_get_angles(pan, tilt, angles);
        usartPrint("P:%ld/%d T:%ld/%d p:%d t:%d\r\n",
                   pan->steps_taken, pan->steps_remaining,
                   tilt->steps_taken, tilt->steps_remaining,
                   angles[0], angles[2]);
        return;
    }

    // Check for AZ? command (query current azimuth)
    if (strcasecmp(cmd, "AZ?") == 0)
    {
        stepper_get_angles(pan, tilt, angles);
        usartPrint("Current AZ: %d\r\n", angles[0]);
        return;
    }

    // Check for EL? command (query current elevation)
    if (strcasecmp(cmd, "EL?") == 0)
    {
        stepper_get_angles(pan, tilt, angles);
        usartPrint("Current EL: %d\r\n", angles[2]);
        return;
    }

    az_ptr = strcasestr(cmd, "AZ");
    el_ptr = strcasestr(cmd, "EL");

    if (az_ptr)
    {
        if (sscanf(az_ptr + 2, "%d", &az) == 1)
        {
            valid = true;
        }
        else
        {
            usartPrint("Invalid AZ format\r\n");
            return;
        }
    }

    if (el_ptr)
    {
        if (sscanf(el_ptr + 2, "%d", &el) == 1)
        {
            valid = true;
        }
        else
        {
            usartPrint("Invalid EL format\r\n");
            return;
        }
    }

    if (!valid)
    {
        usartPrint("Unknown command: %s\r\n", cmd);
        return;
    }

    stepper_get_angles(pan, tilt, angles);
    if (az == -1)
        az = angles[0];
    if (el == -1)
        el = angles[2];

    stepper_goto_position(pan, tilt, az, el);
}





// Call this function repeatedly in main loop
void commandParserPoll(Stepper *pan, Stepper *tilt)
{
    char buffer[CMD_BUFFER_SIZE];
    // usartPrint("test0");
    if (usartReadLine(buffer, CMD_BUFFER_SIZE))
    {
        trim(buffer);
        // usartPrint("test1");
        if (strlen(buffer) > 0 || 1 == 1)
        {
            // usartPrint("test2");
            handleCommand(buffer, pan, tilt);
        }
    }
}
