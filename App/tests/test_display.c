#include "common.h"
#include "tests.h"
#include "display_controller.h"
#include "ssd1306_fonts.h"
#include <stdint.h>
#include <stdio.h>

void Test_Display(App_t *app)
{
    printf("TEST DISPLAY: begin\n**********\r\n\n");

    char *t1 = "ABCD";
    char *t2 = "DEFG";
    char *t3 = "HIJK";
    char *t4 = "LMNO";

    char *bl1 = " 3.300";
    char *br1 = "   V";

    char *bl2 = "50.000";
    char *br2 = "  mA";

    for (uint8_t i = 0; i < DSP_COUNT; i++) {
        ssd1306_SetCursor(DSP_TOP_COL_1, DSP_TOP_ROW);
        ssd1306_WriteString(t1, Font_7x10, White);

        ssd1306_SetCursor(DSP_TOP_COL_2, DSP_TOP_ROW);
        ssd1306_WriteString(t2, Font_7x10, White);

        ssd1306_SetCursor(DSP_TOP_COL_3, DSP_TOP_ROW);
        ssd1306_WriteString(t3, Font_7x10, White);

        ssd1306_SetCursor(DSP_TOP_COL_4, DSP_TOP_ROW);
        ssd1306_WriteString(t4, Font_7x10, White);

        ssd1306_SetCursor(DSP_BOT_COL_1, DSP_BOT_ROW_1);
        ssd1306_WriteString(bl1, Font_11x18, White);

        ssd1306_SetCursor(DSP_BOT_COL_2, DSP_BOT_ROW_1);
        ssd1306_WriteString(br1, Font_11x18, White);

        ssd1306_SetCursor(DSP_BOT_COL_1, DSP_BOT_ROW_2);
        ssd1306_WriteString(bl2, Font_11x18, White);

        ssd1306_SetCursor(DSP_BOT_COL_2, DSP_BOT_ROW_2);
        ssd1306_WriteString(br2, Font_11x18, White);

        ssd1306_UpdateScreen(app->display_controller.oleds[i].i2c_addr);
    }

    printf("TEST DISPLAY: end\n**********\r\n\n");
}