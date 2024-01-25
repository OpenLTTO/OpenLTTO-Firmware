#include "Arduino.h"
#include "openltto_ui.h"
extern "C" void app_main()
{
    initArduino();
    openltto_ui_init();
}
