#include "button.h"
#include "hittest.h"
#include "audio.h"'
#include "cprocessing.h"

struct Button init_button(float button_x, float button_y, float button_width, float button_height, CP_Color button_color, const char* text, float text_x, float text_y, CP_Color text_color, float text_size) {
    struct Rectangle rect = {
        .width = button_width,
        .height = button_height,
        .pos = CP_Vector_Set(button_x, button_y)
    };

    struct Button button = {
        .rect = rect,
        .state = BUTTON_STATE_NONE,
        .rectColor = button_color,
        .text = text,
        .textColor = text_color,
        .textPos = CP_Vector_Set(text_x, text_y),
        .textSize = text_size
    };

    return button;
}

void render_button(struct Button button) {
    CP_Settings_RectMode(CP_POSITION_CENTER);

    CP_Settings_Fill(button.rectColor);

    CP_Graphics_DrawRect(button.rect.pos.x, button.rect.pos.y, button.rect.width, button.rect.height);

    CP_Settings_TextSize(button.textSize);
    CP_Settings_Fill(button.textColor);
    CP_Font_DrawText(button.text, button.textPos.x, button.textPos.y);
}

void update_button_state(struct Button* button, float mouse_x, float mouse_y) {
    if (is_mouse_on_rect(button->rect, mouse_x, mouse_y)) {
        // Trigger once
        if (button->state != BUTTON_STATE_HOVERED && button->state != BUTTON_STATE_CLICKED) {
            play_sound(UI_BUTTON_HOVER);
            // Anything else
        }

        button->state = BUTTON_STATE_HOVERED;
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT)) {
            play_sound(UI_BUTTON_CONFIRM_CLICK); // TODO (If possible): check if button is positive (confirm) or negative (back/cancel)
            button->state = BUTTON_STATE_CLICKED;
        }
    }
    else
        button->state = BUTTON_STATE_NONE;
}