#include "Arduino.h"
#include "openltto_ui.h"
#include "OpenLTTO_Hardware.h"

extern "C" void app_main()
{
    initArduino();
    openltto_hardware_init();
    openltto_ui_init();
}
