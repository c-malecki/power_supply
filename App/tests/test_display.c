#include "common.h"
#include "tests.h"
#include "display_controller.h"

void Test_Display(App_t *app)
{
    char *yellow = "abcdefghijklm";
    _Error_Codes err = Display_Controller_Write_Yellow(yellow);
    if (err != ERROR_NONE) { }

    char *blue_top = "abcdefgh";
    err = Display_Controller_Write_Blue(blue_top, DISPLAY_BLUE_TOP);
    if (err != ERROR_NONE) { }

    char *blue_bot = "ijklmnop";
    err = Display_Controller_Write_Blue(blue_bot, DISPLAY_BLUE_BOT);
    if (err != ERROR_NONE) { }
}