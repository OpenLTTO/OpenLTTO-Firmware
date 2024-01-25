SET(SOURCES screens/ui_BootScreen.c
    screens/ui_ModeSelectionScreen.c
    screens/ui_TaggerHomeScreen.c
    screens/ui_TaggerConfigScreen.c
    screens/ui_HostHomeScreen.c
    screens/ui_HostConfigScreen.c
    screens/ui_ZoneHomeScreen.c
    screens/ui_ZoneConfigScreen.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_openltto_logo_png.c)

add_library(ui ${SOURCES})
