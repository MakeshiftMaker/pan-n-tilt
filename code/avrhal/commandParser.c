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
    while (isspace((unsigned char)*str)) str++;

    // Trim trailing
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';
}

// Example command handler
static void handleCommand(const char *cmd, Stepper *pan, Stepper *tilt)
{
    if (strcasecmp(cmd, "STATUS") == 0)
    {
        usartPrint("System OK\n\r");
        return;
    }

    int az = -1;  // -1 means "not specified"
    int el = -1;

    // Look for AZ and EL in any order
    const char *az_ptr = strcasestr(cmd, "AZ");
    const char *el_ptr = strcasestr(cmd, "EL");

    if (az_ptr && sscanf(az_ptr + 2, "%d", &az) != 1)
    {
        usartPrint("Invalid AZ format\n\r");
        return;
    }

    if (el_ptr && sscanf(el_ptr + 2, "%d", &el) != 1)
    {
        usartPrint("Invalid EL format\n\r");
        return;
    }

    if (az == -1 && el == -1)
    {
        usartPrint("No AZ or EL specified\n\r");
        return;
    }

    // Get current positions if one axis is omitted
    int angles[4];
    stepper_get_angles(pan, tilt, angles);
    if (az == -1) az = angles[0];
    if (el == -1) el = angles[2];

    stepper_goto_position(pan, tilt, az, el);
}


// Call this function repeatedly in main loop
void commandParserPoll(Stepper *pan, Stepper *tilt)
{
    char buffer[CMD_BUFFER_SIZE];
    //usartPrint("test0");
    if (usartReadLine(buffer, CMD_BUFFER_SIZE))
    {
        trim(buffer);
        //usartPrint("test1");
        if (strlen(buffer) > 0 || 1 == 1)
        {
            //usartPrint("test2");
            handleCommand(buffer, pan, tilt);
        }
    }
}
