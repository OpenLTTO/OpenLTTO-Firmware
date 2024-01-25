#include "openltto_ui.h"
#include "openltto_ui_callbacks.h"

static const char *TAG = "openltto-ui-callbacks";

void on_reset_confirmed()
{
    openltto_ui_event_emitter(OPENLTTO_UI_EVENT_RESET_CONFIRMED);
}
