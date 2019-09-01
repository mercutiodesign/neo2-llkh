#define UNICODE
/**
 * Alternative Windows driver for the Neo2 based keyboard layouts:
 * Neo2, (www.neo-layout.org)
 * AdNW, AdNWzjßf, KOY (www.adnw.de)
 */

#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <stdbool.h>
#include "trayicon.h"
#include "resources.h"

HHOOK keyhook = NULL;
#define APPNAME "neo-llkh"
#define len 103


// use quote/ä as right level 3 modifier:
bool quoteAsMod3R = false;
int scanCodeMod3R = 43;
bool shiftLockEnabled = false;
bool qwertzForShortcuts = false;

/**
 * True if no mapping should be done
 */
bool bypassMode = false;
extern void toggleBypassMode();
char *layout;

bool shiftLeftPressed = false;
bool shiftRightPressed = false;
bool shiftLockActive = false;

bool ctrlLeftPressed = false;
bool ctrlRightPressed = false;
bool altLeftPressed = false;
bool winLeftPressed = false;
bool winRightPressed = false;

TCHAR mappingTableLevel1[len];
TCHAR mappingTableLevel2[len];
TCHAR mappingTableLevel3[len];
TCHAR mappingTableLevel4[len];



void initLayout()
{
	for (int i = 0; i < len; i++) {
		mappingTableLevel1[i] = 0;
		mappingTableLevel2[i] = 0;
		mappingTableLevel3[i] = 0;
		mappingTableLevel4[i] = 0;
	}

	// same for all layouts
	wcscpy(mappingTableLevel1 +  2, L"1234567890-`");

	wcscpy(mappingTableLevel2 + 41, L"̌");  // key to the left of "1" key
	wcscpy(mappingTableLevel2 +  2, L"°§ℓ»«$€„“”—̧");

	wcscpy(mappingTableLevel3 + 41, L"^");
	wcscpy(mappingTableLevel3 +  2, L"¹²³›‹¢¥‚‘’—̊");
	wcscpy(mappingTableLevel3 + 16, L"…_[]^!<>=&ſ̷");
	wcscpy(mappingTableLevel3 + 30, L"\\/{}*?()-:@");
	wcscpy(mappingTableLevel3 + 44, L"#$|~`+%\"';");

	wcscpy(mappingTableLevel4 + 41, L"̇");
	wcscpy(mappingTableLevel4 +  2, L"ªº№⋮·£¤0/*-¨");
	wcscpy(mappingTableLevel4 + 21, L"¡789+−˝");
	wcscpy(mappingTableLevel4 + 35, L"¿456,.");
	wcscpy(mappingTableLevel4 + 49, L":123;");

	if (strcmp(layout, "adnw") == 0) {
		wcscpy(mappingTableLevel1 + 16, L"küu.ävgcljf´");
		wcscpy(mappingTableLevel1 + 30, L"hieaodtrnsß");
		wcscpy(mappingTableLevel1 + 44, L"xyö,qbpwmz");

		wcscpy(mappingTableLevel2 + 16, L"KÜU•ÄVGCLJF~");
		wcscpy(mappingTableLevel2 + 30, L"HIEAODTRNSẞ");
		wcscpy(mappingTableLevel2 + 44, L"XYÖ–QBPWMZ");

	} else if (strcmp(layout, "adnwzjf") == 0) {
		wcscpy(mappingTableLevel1 + 16, L"küu.ävgclßz´");
		wcscpy(mappingTableLevel1 + 30, L"hieaodtrnsf");
		wcscpy(mappingTableLevel1 + 44, L"xyö,qbpwmj");

		wcscpy(mappingTableLevel2 + 16, L"KÜU•ÄVGCLẞZ~");
		wcscpy(mappingTableLevel2 + 30, L"HIEAODTRNSF");
		wcscpy(mappingTableLevel2 + 44, L"XYÖ–QBPWMJ");

	} else if (strcmp(layout, "koy") == 0) {
		wcscpy(mappingTableLevel1 + 16, L"k.o,yvgclßz´");
		wcscpy(mappingTableLevel1 + 30, L"haeiudtrnsf");
		wcscpy(mappingTableLevel1 + 44, L"xqäüöbpwmj");

		wcscpy(mappingTableLevel2 + 16, L"K•O–YVGCLẞZ~");
		wcscpy(mappingTableLevel2 + 30, L"HAEIUDTRNSF");
		wcscpy(mappingTableLevel2 + 44, L"XQÄÜÖBPWMJ");

	} else if (strcmp(layout, "kou") == 0) {
		wcscpy(mappingTableLevel1 + 16, L"k.ouävgclfz´");
		wcscpy(mappingTableLevel1 + 30, L"haeiybtrnsß");
		wcscpy(mappingTableLevel1 + 44, L"qx,üöpdwmj");

		wcscpy(mappingTableLevel2 + 16, L"K!OUÄVGCLFZ~");
		wcscpy(mappingTableLevel2 + 30, L"HAEIYBTRNSẞ");
		wcscpy(mappingTableLevel2 + 44, L"QX–ÜÖPDWMJ");

		wcscpy(mappingTableLevel3 + 16, L"→%{}^•<>=&€̷");
		wcscpy(mappingTableLevel3 + 32, L"[]*?()-:@");
		wcscpy(mappingTableLevel3 + 50, L"_\"';");

		wcscpy(mappingTableLevel4 +  4, L"✔✘·£¤0/*-¨");
		wcscpy(mappingTableLevel4 + 21, L":789+−˝");
		wcscpy(mappingTableLevel4 + 35, L"-456,;");
		wcscpy(mappingTableLevel4 + 49, L"_123.");

	} else { // neo
		wcscpy(mappingTableLevel1 + 16, L"xvlcwkhgfqß´");
		wcscpy(mappingTableLevel1 + 30, L"uiaeosnrtdy");
		wcscpy(mappingTableLevel1 + 44, L"üöäpzbm,.j");

		wcscpy(mappingTableLevel2 + 16, L"XVLCWKHGFQẞ~");
		wcscpy(mappingTableLevel2 + 30, L"UIAEOSNRTDY");
		wcscpy(mappingTableLevel2 + 44, L"ÜÖÄPZBM–•J");
	}

	// if quote/ä is the right level 3 modifier, copy symbol of quote/ä key to backslash/# key
	if (quoteAsMod3R) {
		mappingTableLevel1[43] = mappingTableLevel1[40];
		mappingTableLevel2[43] = mappingTableLevel2[40];
		mappingTableLevel3[43] = mappingTableLevel3[40];
		mappingTableLevel4[43] = mappingTableLevel4[40];
	}
}

/**
 * Map a key scancode to the char that should be displayed after typing
 **/
TCHAR mapScanCodeToChar(unsigned level, char in)
{
	switch (level) {
		case 2:
			return mappingTableLevel2[in];
		case 3:
			return mappingTableLevel3[in];
		case 4:
			return mappingTableLevel4[in];
		default: // level 1
			return mappingTableLevel1[in];
	}
}

void sendUnicodeChar(TCHAR key)
{
	KEYBDINPUT kb={0};
	INPUT Input={0};

	kb.wScan = key;
	kb.dwFlags = KEYEVENTF_UNICODE;
	Input.type = INPUT_KEYBOARD;
	Input.ki = kb;
	SendInput(1, &Input, sizeof(Input));
}

/**
 * Sends a char using emulated keyboard input
 *
 * This works for most cases, but not for dead keys etc
 **/
void sendChar(TCHAR key, KBDLLHOOKSTRUCT keyInfo)
{
	SHORT keyScanResult = VkKeyScanEx(key, GetKeyboardLayout(0));

	if (keyScanResult == -1 || shiftLockActive) {
		// key not found in the current keyboard layout or shift lock is active
		//
		// If shiftLockActive is true, a unicode letter will be sent. This implies
		// that shortcuts don't work in shift lock mode. That's good, because
		// people might not be aware that they would send Ctrl-S instead of
		// Ctrl-s. Sending a unicode letter makes it possible to undo shift
		// lock temporarily by holding one shift key because that way the
		// shift key won't be sent.
		sendUnicodeChar(key);
	} else {
		keyInfo.vkCode = keyScanResult;
		char modifiers = keyScanResult >> 8;
		bool shift = ((modifiers & 1) != 0);
		bool alt = ((modifiers & 2) != 0);
		bool ctrl = ((modifiers & 4) != 0);
		bool altgr = alt && ctrl;
		if (altgr) {
			ctrl = false;
			alt = false;
		}

		if (altgr)
			keybd_event(VK_RMENU, 0, 0, 0);
		if (ctrl)
			keybd_event(VK_CONTROL, 0, 0, 0);
		if (alt)
			keybd_event(VK_MENU, 0, 0, 0); // ALT
		if (shift)
			keybd_event(VK_SHIFT, 0, 0, 0);

		keyInfo.vkCode = keyScanResult;
		keybd_event(keyInfo.vkCode, keyInfo.scanCode, keyInfo.flags, keyInfo.dwExtraInfo);

		if (altgr)
			keybd_event(VK_RMENU, 0, KEYEVENTF_KEYUP, 0);
		if (ctrl)
			keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
		if (alt)
			keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0); // ALT
		if (shift)
			keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
	}
}

bool handleLayer2SpecialCases(KBDLLHOOKSTRUCT keyInfo)
{
	switch(keyInfo.scanCode) {
		case 27:
			sendChar(L'̃', keyInfo);
			return true;
		case 41:
			sendChar(L'̌', keyInfo);
			return true;
		default:
			return false;
	}

}

bool handleLayer3SpecialCases(KBDLLHOOKSTRUCT keyInfo)
{
	switch(keyInfo.scanCode) {
		case 13:
			sendChar(L'̊', keyInfo);
			return true;
		case 20:
			sendChar(L'^', keyInfo);
			keybd_event(VK_SPACE, 0, 0, 0);
			return true;
		case 27:
			sendChar(L'̷', keyInfo);
			return true;
		case 48:
			sendChar(L'`', keyInfo);
			keybd_event(VK_SPACE, 0, 0, 0);
			return true;
		default:
			return false;
	}

}

bool handleLayer4SpecialCases(KBDLLHOOKSTRUCT keyInfo)
{
	switch(keyInfo.scanCode) {
		case 13:
			sendChar(L'¨', keyInfo);
			return true;
		case 27:
			sendChar(L'˝', keyInfo);
			return true;
		case 41:
			sendChar(L'̇', keyInfo);
			return true;
	}
	CHAR mappingTable[len];
	for (int i = 0; i < len; i++)
		mappingTable[i] = 0;

	mappingTable[16] = VK_PRIOR;
	mappingTable[17] = VK_BACK;
	mappingTable[18] = VK_UP;
	mappingTable[19] = VK_DELETE;
	mappingTable[20] = VK_NEXT;
	mappingTable[30] = VK_HOME;
	mappingTable[31] = VK_LEFT;
	mappingTable[32] = VK_DOWN;
	mappingTable[33] = VK_RIGHT;
	mappingTable[34] = VK_END;
	if (strcmp(layout, "kou") == 0) {
		mappingTable[44] = VK_INSERT;
		mappingTable[45] = VK_TAB;
		mappingTable[46] = VK_RETURN;
		mappingTable[48] = VK_ESCAPE;
	} else {
		mappingTable[44] = VK_ESCAPE;
		mappingTable[45] = VK_TAB;
		mappingTable[46] = VK_INSERT;
		mappingTable[47] = VK_RETURN;
	}
	mappingTable[57] = '0';

	if (mappingTable[keyInfo.scanCode] != 0) {
		keybd_event(mappingTable[keyInfo.scanCode], 0, 0, 0);
		return true;
	}
	return false;
}

bool isShift(KBDLLHOOKSTRUCT keyInfo)
{
	return keyInfo.vkCode == VK_SHIFT || keyInfo.vkCode == VK_LSHIFT
	    || keyInfo.vkCode == VK_RSHIFT;
}

bool isMod3(KBDLLHOOKSTRUCT keyInfo)
{
	return keyInfo.vkCode == VK_CAPITAL || keyInfo.scanCode == scanCodeMod3R;
}

bool isMod4(KBDLLHOOKSTRUCT keyInfo)
{
	return keyInfo.vkCode == VK_RMENU 
         || keyInfo.vkCode == VK_OEM_102; // |<> -Key
}

bool isSystemKeyPressed() {
	return ctrlLeftPressed || ctrlRightPressed
		|| altLeftPressed || winLeftPressed || winRightPressed;
}

void logKeyEvent(char *desc, KBDLLHOOKSTRUCT keyInfo)
{
	char *keyName;
	switch (keyInfo.vkCode) {
		case VK_LSHIFT:
			keyName = "(Shift left)";
			break;
		case VK_RSHIFT:
			keyName = "(Shift right)";
			break;
		case VK_CAPITAL:
			keyName = "(M3 left)";
			break;
		case 0xde:  // ä
			keyName = quoteAsMod3R ? "(M3 right)" : "";
			break;
		case 0xbf:  // #
			keyName = quoteAsMod3R ? "" : "(M3 right)";
			break;
		case VK_CONTROL:
			keyName = "(Ctrl)";
			break;
		case VK_LCONTROL:
			keyName = "(Ctrl left)";
			break;
		case VK_RCONTROL:
			keyName = "(Ctrl right)";
			break;
		case VK_MENU:
			keyName = "(Alt)";
			break;
		case VK_LMENU:
			keyName = "(Alt left)";
			break;
		case VK_RMENU:
			keyName = "(Alt Right)";
			break;
		case VK_LWIN:
			keyName = "(Win left)";
			break;
		case VK_RWIN:
			keyName = "(Win right)";
			break;
		default:
			keyName = "";
	}
	char *shiftLockInfo = shiftLockActive ? " [shift lock active]" : "";
	printf("%-10s sc %u vk 0x%x 0x%x %d %s%s\n", desc, keyInfo.scanCode, keyInfo.vkCode,
	       keyInfo.flags, keyInfo.dwExtraInfo, keyName, shiftLockInfo);
}

__declspec(dllexport)
LRESULT CALLBACK keyevent(int code, WPARAM wparam, LPARAM lparam)
{
	static bool shiftPressed = false;
	static bool mod3Pressed = false;
	static bool mod4Pressed = false;

	KBDLLHOOKSTRUCT keyInfo;
	if (code == HC_ACTION
	    && (wparam == WM_SYSKEYUP || wparam == WM_KEYUP || wparam == WM_SYSKEYDOWN
		|| wparam == WM_KEYDOWN)) {
		keyInfo = *((KBDLLHOOKSTRUCT *) lparam);

		if (keyInfo.flags & LLKHF_INJECTED) {	// process injected events like normal, because most probably we are injecting them
			logKeyEvent("injected", keyInfo);
			return CallNextHookEx(NULL, code, wparam, lparam);
		}
	}

	if (code == HC_ACTION && wparam == WM_KEYDOWN &&
		shiftPressed && keyInfo.scanCode == 69) {
		toggleBypassMode();
		return -1;
	}

	if (bypassMode)
		return CallNextHookEx(NULL, code, wparam, lparam);

	if (code == HC_ACTION && (wparam == WM_SYSKEYUP || wparam == WM_KEYUP)) {
		logKeyEvent("key up", keyInfo);

		if (isShift(keyInfo)) {
			shiftPressed = false;  // correct?
			if (keyInfo.vkCode == VK_RSHIFT) {
				shiftRightPressed = false;
				if (shiftLockEnabled && shiftLeftPressed) {
					shiftLockActive = !shiftLockActive;
					printf("Shift lock %s!\n", shiftLockActive ? "activated" : "deactivated");
				}
				keybd_event(VK_RSHIFT, 0, KEYEVENTF_KEYUP, 0);
			} else {
				shiftLeftPressed = false;
				if (shiftLockEnabled && shiftRightPressed) {
					shiftLockActive = !shiftLockActive;
					printf("Shift lock %s!\n", shiftLockActive ? "activated" : "deactivated");
				}
				keybd_event(VK_LSHIFT, 0, KEYEVENTF_KEYUP, 0);
			}
			return -1;
		} else if (isMod3(keyInfo)) {
			mod3Pressed = false;
			return -1;
		} else if (isMod4(keyInfo)) {
			mod4Pressed = false;
			return -1;
		}
		if (keyInfo.vkCode == VK_LCONTROL) {
			ctrlLeftPressed = false;
		} else if (keyInfo.vkCode == VK_RCONTROL) {
			ctrlRightPressed = false;
		} else if (keyInfo.vkCode == VK_LMENU) {
			altLeftPressed = false;
		} else if (keyInfo.vkCode == VK_LWIN) {
			winLeftPressed = false;
		} else if (keyInfo.vkCode == VK_RWIN) {
			winRightPressed = false;
		}
	}

	else if (code == HC_ACTION && (wparam == WM_SYSKEYDOWN || wparam == WM_KEYDOWN)) {
		printf("\n");
		logKeyEvent("key down", keyInfo);

		if (keyInfo.vkCode == VK_LCONTROL) {
			ctrlLeftPressed = true;
		} else if (keyInfo.vkCode == VK_RCONTROL) {
			ctrlRightPressed = true;
		} else if (keyInfo.vkCode == VK_LMENU) {
			altLeftPressed = true;
		} else if (keyInfo.vkCode == VK_LWIN) {
			winLeftPressed = true;
		} else if (keyInfo.vkCode == VK_RWIN) {
			winRightPressed = true;
		}

		unsigned level = 1;
		if (shiftPressed != shiftLockActive)
			// (shiftPressed and no shiftLockActive) or (shiftLockActive and no shiftPressed) (XOR)
			level = 2;
		if (mod3Pressed)
			level = 3;
		if (mod4Pressed)
			level = 4;

		if (isShift(keyInfo)) {
			shiftPressed = true;
			if (keyInfo.vkCode == VK_RSHIFT) {
				shiftRightPressed = true;
				keybd_event(VK_RSHIFT, 0, 0, 0);
			} else {
				shiftLeftPressed = true;
				keybd_event(VK_LSHIFT, 0, 0, 0);
			}
			//keybd_event(VK_SHIFT, 0, 0, 0);
			return -1;
		} else if (isMod3(keyInfo)) {
			mod3Pressed = true;
			return -1;
		} else if (isMod4(keyInfo)) {
			/* ALTGR triggers two keys: LCONTROL and RMENU
			   we don't want to have any of those two here effective but return -1 seems 
			   to change nothing, so we simply send keyup here.  */
			keybd_event(VK_RMENU, 0, KEYEVENTF_KEYUP, 0);
			mod4Pressed = true;
			return -1;
		} else if (level == 2 && handleLayer2SpecialCases(keyInfo)) {
			return -1;
		} else if (level == 3 && handleLayer3SpecialCases(keyInfo)) {
			return -1;
		} else if (level == 4 && handleLayer4SpecialCases(keyInfo)) {
			return -1;
		} else if (!isSystemKeyPressed()) {
			TCHAR key = mapScanCodeToChar(level, keyInfo.scanCode);
			if (key != 0 && (keyInfo.flags & LLKHF_INJECTED) == 0) {
				// if key must be mapped
				printf("Mapped %d->%c (level %u)\n", keyInfo.scanCode, key, level);
				//BYTE state[256];
				//GetKeyboardState(state);
				sendChar(key, keyInfo);
				//SetKeyboardState(state);
				return -1;
			}
		}
	}
	return CallNextHookEx(NULL, code, wparam, lparam);
}

DWORD WINAPI hookThreadMain(void *user)
{
	HINSTANCE base = GetModuleHandle(NULL);
	MSG msg;

	if (!base) {
		if (!(base = LoadLibrary((wchar_t *) user))) {
			return 1;
		}
	}

	keyhook = SetWindowsHookEx(WH_KEYBOARD_LL, keyevent, base, 0);

	while (GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(keyhook);

	return 0;
}

void exitApplication()
{
	trayicon_remove();
	PostQuitMessage(0);
}

void toggleBypassMode()
{
	bypassMode = !bypassMode;

	HINSTANCE hInstance = GetModuleHandle(NULL);
	HICON icon;
	if (bypassMode)
		icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON_DISABLED));
	else
		icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));

	trayicon_change_icon(icon);
	printf("%i bypass mode \n", bypassMode);
}

int main(int argc, char *argv[])
{
	// the first parameter sets the layout (optional):
	// neo (default), adnw, adnwzjf, koy, kou
	if (argc >= 2)
		layout = argv[1];
	else
		layout = "neo";

	// if the second parameter is 1, quote and backslash (ä and #) will be swapped
	if (argc >= 3 && strcmp(argv[2], "1") == 0) {
		quoteAsMod3R = true;
		scanCodeMod3R = 40;
	}

	// If the third parameter is 1, shift lock is enabled.
	// Toggle by pressing both shift keys at the same time.
	if (argc >= 4 && strcmp(argv[3], "1") == 0) {
		shiftLockEnabled = true;
	}

	// If the fourth parameter is 1, "qwertz for shortcuts" is enabled.
	if (argc >= 5 && strcmp(argv[4], "1") == 0) {
		qwertzForShortcuts = true;
	}

	initLayout();

	setbuf(stdout, NULL);

	DWORD tid;

	HINSTANCE hInstance = GetModuleHandle(NULL);
	trayicon_init(LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON)), APPNAME);
	trayicon_add_item(NULL, &toggleBypassMode);
	trayicon_add_item("Exit", &exitApplication);

	HANDLE thread = CreateThread(0, 0, hookThreadMain, argv[0], 0, &tid);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
