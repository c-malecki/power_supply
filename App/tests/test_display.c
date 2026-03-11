#include "common.h"
#include "tests.h"
#include "display_controller.h"

void Test_Display(App_t *app)
{
    char *yellow = "abcdefghijklm";
    Display_Controller_Write_Yellow(&app->display_controller, yellow);

    char *blue_top = "abcdefghi";
    Display_Controller_Write_Blue(&app->display_controller, blue_top, DISPLAY_BLUE_TOP);

    char *blue_bot = "jklmnopqr";
    Display_Controller_Write_Blue(&app->display_controller, blue_bot, DISPLAY_BLUE_BOT);
}