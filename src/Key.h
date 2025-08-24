#include <windows.h>

#ifndef KEY_H
#define KEY_H

void sendKeyMessageDown(int key, HWND hwnd)
{
    keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(key, 0, 0, 0);
    // SendMessage(hwnd, WM_KEYUP, key, 0);
    // SendMessage(hwnd, WM_KEYDOWN, key, 0);
}

void sendKeyMessageUp(int key, HWND hwnd)
{
    keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
    // SendMessage(hwnd, WM_KEYUP, key, 0);
}

int keyList[36] = {
    188, 76, 190, 186, 191, 73, 57, 79, 48, 80, 189, 219,
    90, 83, 88, 68, 67, 86, 71, 66, 72, 78, 74, 77,
    81, 50, 87, 51, 69, 82, 53, 84, 54, 89, 55, 85
};



// int keyList[36] = {
//     188, 76, 190, 186, 191, 73, 57, 79, 48, 80, 189, 219,
//     65, 83, 68, 70, 74, 75, 76, 186, 222, 221, 220, 71,
//     81, 50, 87, 51, 69, 82, 53, 84, 54, 89, 55, 85};





#endif // KEY_H