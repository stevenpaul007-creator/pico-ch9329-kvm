#pragma once
#include <stdint.h>
#include <Arduino.h>
#include "CH9329.h"

#define MAX_SCREEN_WIDTH 1366
#define MAX_SCREEN_HEIGHT 768

enum KVM_MODE
{
    KVM_MODE_NONE = 0,
    KVM_MODE_LEFT_ONLY,
    KVM_MODE_RIGHT_ONLY,
    KVM_MODE_ON_LEFT,
    KVM_MODE_ON_RIGHT
};

enum SCREEN
{
    SCREEN_LEFT = 0,
    SCREEN_RIGHT
};

struct current_status
{
    SCREEN active_screen = SCREEN_LEFT;
    KVM_MODE mode = KVM_MODE_NONE;
    uint16_t max_height = MAX_SCREEN_HEIGHT;
    uint16_t max_width = MAX_SCREEN_WIDTH;
    int16_t current_mouse_x_in_dual_screen = 0;
    int16_t current_mouse_x = 0;
    int16_t current_mouse_y = 0;
} current_status;

static void judge_kvm_mode(bool leftUSBConnected,
                           bool rightUSBConnected,
                           uint8_t mouseRelX,
                           uint8_t mouseRelY)
{

    DBG_printf("leftUSBConnected = %02X, rightUSBConnected= %02X\r\n",
                  leftUSBConnected, rightUSBConnected);

    if (!leftUSBConnected && !rightUSBConnected)
    {
        current_status.mode = KVM_MODE_NONE;
        return;
    }
    if (!leftUSBConnected && rightUSBConnected)
    {
        current_status.active_screen = SCREEN_RIGHT;
        current_status.mode = KVM_MODE_RIGHT_ONLY;
        return;
    }
    if (leftUSBConnected && !rightUSBConnected)
    {
        current_status.active_screen = SCREEN_LEFT;
        current_status.mode = KVM_MODE_LEFT_ONLY;
        return;
    }
    if (leftUSBConnected && rightUSBConnected)
    {
        DBG_printf("leftUSBConnected = %02X, rightUSBConnected= %02X mouseRelX= %02X mouseRelY= %02X\r\n",
                      leftUSBConnected, rightUSBConnected, mouseRelX, mouseRelY);
        if (mouseRelX >= 0x80)
        {
            current_status.current_mouse_x_in_dual_screen -= (uint8_t)(0 - mouseRelX);
        }
        else
        {
            current_status.current_mouse_x_in_dual_screen += mouseRelX;
        }
        if (mouseRelY >= 0x80)
        {
            current_status.current_mouse_y -= (uint8_t)(0 - mouseRelY);
        }
        else
        {
            current_status.current_mouse_y += mouseRelY;
        }

        if (current_status.current_mouse_x_in_dual_screen <= 0)
        {
            current_status.current_mouse_x_in_dual_screen = 0;
        }
        if (current_status.current_mouse_y <= 0)
        {
            current_status.current_mouse_y = 0;
        }
        if (current_status.current_mouse_y >= MAX_SCREEN_HEIGHT)
        {
            current_status.current_mouse_y = MAX_SCREEN_HEIGHT;
        }
        if (current_status.current_mouse_x_in_dual_screen > MAX_SCREEN_WIDTH * 2)
        {
            current_status.current_mouse_x_in_dual_screen = MAX_SCREEN_WIDTH * 2;
        }
        if (current_status.current_mouse_x_in_dual_screen > MAX_SCREEN_WIDTH)
        {
            current_status.current_mouse_x = current_status.current_mouse_x_in_dual_screen - MAX_SCREEN_WIDTH;
            current_status.active_screen = SCREEN_RIGHT;
            current_status.mode = KVM_MODE_ON_RIGHT;
        }
        else
        {
            current_status.current_mouse_x = current_status.current_mouse_x_in_dual_screen;
            current_status.active_screen = SCREEN_LEFT;
            current_status.mode = KVM_MODE_ON_LEFT;
        }
    }
    DBG_printf("current_mouse_x = %d, current_mouse_y= %d active_screen=%02X, mode=%02X \r\n",
                  current_status.current_mouse_x, current_status.current_mouse_y, current_status.active_screen, current_status.mode);
}

static uint16_t convert_to_abs_pos_x(uint16_t x)
{
    uint16_t ret = 4096.0f * x / MAX_SCREEN_WIDTH;
    DBG_printf("convert_to_abs_pos_x(%d)=%d\r\n", x, ret);
    return ret;
}
static uint16_t convert_to_abs_pos_y(uint16_t y)
{
    uint16_t ret = 4096.0f * y / MAX_SCREEN_HEIGHT;
    DBG_printf("convert_to_abs_pos_y(%d)=%d\r\n", y, ret);
    return ret;
}