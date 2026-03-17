#include "src/gateway.h"

void setup()
{
gateway_init();

}


void loop()
{

gateway_poll();

}