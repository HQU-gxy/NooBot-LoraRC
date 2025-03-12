#include "Display.h"
#include "TFT_eSPI.h"
#include "Arduino.h"
#include "lvgl.h"

#include "config.h"

namespace Display
{
    static TFT_eSPI screen;
    static lv_color16_t draw_buf[TFT_WIDTH * TFT_HEIGHT / 10];

    constexpr auto BL_PWM_FREQ = 1e4;
    constexpr auto BL_PWM_RES = 8;

    static lv_obj_t *speedValLabel;

    static bool speedUpdated = false;
    static bool voltageUpdated = false;

    static float speedVal = 0;
    static float voltageVal = 0;

    /* LVGL calls it when a rendered image needs to copied to the display*/
    static void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
    {

        uint32_t w = lv_area_get_width(area);
        uint32_t h = lv_area_get_height(area);
        screen.startWrite();
        screen.setAddrWindow(area->x1, area->y1, w, h);
        screen.pushColors((uint16_t *)px_map, w * h, true);
        screen.endWrite();

        /*Call it to tell LVGL you are ready*/
        lv_display_flush_ready(disp);
    }

    static void updateLabels()
    {
        if (speedUpdated)
        {
            auto buf = String(speedVal, 2) + " m/s";
            lv_label_set_text(speedValLabel, buf.c_str());
            speedUpdated = false;
        }
    }

    static void lvglTask(void *)
    {
        while (1)
        {
            updateLabels();
            lv_timer_periodic_handler();
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }

    void begin()
    {
        ledcAttach(SCREEN_BL_PIN, BL_PWM_FREQ, BL_PWM_RES);
        screen.init();
        screen.setRotation(1);
        setBrightness(200);

        lv_init();

        /*Set a tick source so that LVGL will know how much time elapsed. */
        lv_tick_set_cb(xTaskGetTickCount);

        auto disp = lv_display_create(TFT_HEIGHT, TFT_WIDTH);
        lv_display_set_flush_cb(disp, my_disp_flush);
        lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

        auto speedHintLabel = lv_label_create(lv_screen_active());
        lv_label_set_text(speedHintLabel, "Speed: ");
        lv_obj_align(speedHintLabel, LV_ALIGN_TOP_LEFT, 10, 8);

        speedValLabel = lv_label_create(lv_screen_active());
        lv_label_set_text(speedValLabel, "0.0 m/s");
        lv_obj_align(speedValLabel, LV_ALIGN_TOP_RIGHT, -10, 8);

        xTaskCreate(lvglTask, "lvglTask", 4096, NULL, 10, NULL);
    }

    void setBrightness(uint8_t brightness)
    {
        ledcWrite(SCREEN_BL_PIN, brightness);
    }

    void test()
    {
        screen.fillScreen(TFT_RED);
        delay(1000);
        screen.fillScreen(TFT_GREEN);
        delay(1000);
        screen.fillScreen(TFT_BLUE);
        delay(1000);
        screen.fillScreen(TFT_WHITE);
        delay(1000);
    }

    void updateSpeed(float speed)
    {
        speedVal = speed;
        speedUpdated = true;
    }
    void updateVoltage(float voltage)
    {
    }

} // namespace Display