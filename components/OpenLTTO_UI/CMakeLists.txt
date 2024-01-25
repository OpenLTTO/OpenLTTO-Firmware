file(GLOB_RECURSE UI_SRCS "lib/openltto_ui/*.c")
idf_component_register(SRCS "openltto_ui.c" "openltto_ui_callbacks.c" ${UI_SRCS}
                    INCLUDE_DIRS "include" "lib/openltto_ui"
                    REQUIRES driver esp_event lvgl esp_lvgl_port esp_lcd_touch esp_lcd_touch_ft5x06
                    PRIV_REQUIRES esp_lcd
                    )

target_compile_options(
    ${COMPONENT_LIB}
    PRIVATE
    "-Wno-int-conversion"
    "-Wno-unused-function"
    "-Wno-unused-variable"
)
