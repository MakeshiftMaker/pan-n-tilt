#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "stepper.h"

// Polls for new USART commands and processes them
void commandParserPoll(Stepper* pan, Stepper* tilt);

#endif
