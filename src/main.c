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
#define LEN 103

/**
 * Some global settings.
 * These values can be set in a configuration file (settings.ini)
 */
char layout[100];                    // keyboard layout (default: neo)
bool quoteAsMod3R = false;           // use quote/ä as right level 3 modifier
int scanCodeMod3R = 43;              // this scan code depends on quoteAsMod3R
bool shiftLockEnabled = false;       // enable (allow) shift locks
bool qwertzForShortcuts = false;     // use QWERTZ when Ctrl, Alt or Win is involved
bool swapLeftCtrlAndLeftAlt = false; // swap left Ctrl and left Alt key
bool supportLevels5and6 = false;     // support levels five and six (greek letters and mathematical symbols)

/**
 * True if no mapping should be done
 */
bool bypassMode = false;
extern void toggleBypassMode();

/**
 * States of some keys and shift lock.
 */
bool shiftLeftPressed = false;
bool shiftRightPressed = false;
bool shiftLockActive = false;

bool level3modLeftPressed = false;
bool level3modRightPressed = false;

bool level4modLeftPressed = false;
bool level4modRightPressed = false;

bool ctrlLeftPressed = false;
bool ctrlRightPressed = false;
bool altLeftPressed = false;
bool winLeftPressed = false;
bool winRightPressed = false;

/**
 * Mapping tables for four levels.
 * They will be defined in initLayout().
 */
TCHAR mappingTableLevel1[LEN];
TCHAR mappingTableLevel2[LEN];
TCHAR mappingTableLevel3[LEN];
TCHAR mappingTableLevel4[LEN];
TCHAR mappingTableLevel5[LEN];
TCHAR mappingTableLevel6[LEN];


BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType) {
		// Handle the Ctrl-c signal.
		case CTRL_C_EVENT:
			printf("\nCtrl-c detected!\n");
			printf("Please quit using the tray icon!\n\n");
			return TRUE;

		default:
			return FALSE;
	}
}

void mapLevels_2_5_6(TCHAR * mappingTableOutput, TCHAR * newChars)
{
	TCHAR * l1_lowercase = L"abcdefghijklmnopqrstuvwxyzäöüß.,";

	TCHAR *ptr;
	for (int i = 0; i < LEN; i++) {
		ptr = wcschr(l1_lowercase, mappingTableLevel1[i]);
		if (ptr != NULL && ptr < &l1_lowercase[32]) {
			//printf("i = %d: mappingTableLevel1[i] = %c; ptr = %d; ptr = %s; index = %d\n", i, mappingTableLevel1[i], ptr, ptr, ptr-l1_lowercase+1);
			mappingTableOutput[i] = newChars[ptr-l1_lowercase];
		}
	}
}

void initLayout()
{
	// initialize the mapping tables
	for (int i = 0; i < LEN; i++) {
		mappingTableLevel1[i] = 0;
		mappingTableLevel2[i] = 0;
		mappingTableLevel3[i] = 0;
		mappingTableLevel4[i] = 0;
		if (supportLevels5and6) {
			mappingTableLevel5[i] = 0;
			mappingTableLevel6[i] = 0;
		}
	}

	// same for all layouts
	wcscpy(mappingTableLevel1 +  2, L"1234567890-`");

	wcscpy(mappingTableLevel2 + 41, L"̌");  // key to the left of the "1" key
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

	// layout dependent
	if (strcmp(layout, "adnw") == 0) {
		wcscpy(mappingTableLevel1 + 16, L"kuü.ävgcljf´");
		wcscpy(mappingTableLevel1 + 30, L"hieaodtrnsß");
		wcscpy(mappingTableLevel1 + 44, L"xyö,qbpwmz");

	} else if (strcmp(layout, "adnwzjf") == 0) {
		wcscpy(mappingTableLevel1 + 16, L"kuü.ävgclßz´");
		wcscpy(mappingTableLevel1 + 30, L"hieaodtrnsf");
		wcscpy(mappingTableLevel1 + 44, L"xyö,qbpwmj");

	} else if (strcmp(layout, "koy") == 0) {
		wcscpy(mappingTableLevel1 + 16, L"k.o,yvgclßz´");
		wcscpy(mappingTableLevel1 + 30, L"haeiudtrnsf");
		wcscpy(mappingTableLevel1 + 44, L"xqäüöbpwmj");

	} else if (strcmp(layout, "kou") == 0) {
		wcscpy(mappingTableLevel1 + 16, L"k.ouävgclfz´");
		wcscpy(mappingTableLevel1 + 30, L"haeiybtrnsß");
		wcscpy(mappingTableLevel1 + 44, L"qx,üöpdwmj");

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
	}

	// same for all layouts
	wcscpy(mappingTableLevel1 + 27, L"´");
	wcscpy(mappingTableLevel2 + 27, L"~");

	// map letters of level 2
	TCHAR * charsLevel2;
	if (strcmp(layout, "kou") == 0)
		charsLevel2 = L"ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜẞ!–";
	else
		charsLevel2 = L"ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜẞ•–";
	mapLevels_2_5_6(mappingTableLevel2, charsLevel2);

	if (supportLevels5and6) {
		// map levels 5 and 6
		TCHAR * charsLevel5 = L"αβχδεφγψιθκλμνοπϕρστuvωξυζηϵüςϑϱ";  // a-zäöüß.,
		mapLevels_2_5_6(mappingTableLevel5, charsLevel5);
		TCHAR * charsLevel6 = L"∀⇐ℂΔ∃ΦΓΨ∫Θ⨯Λ⇔ℕ∈ΠℚℝΣ∂⊂√ΩΞ∇ℤℵ∩∪∘↦⇒";  // a-zäöüß.,
		mapLevels_2_5_6(mappingTableLevel6, charsLevel6);
	}

	// if quote/ä is the right level 3 modifier, copy symbol of quote/ä key to backslash/# key
	if (quoteAsMod3R) {
		mappingTableLevel1[43] = mappingTableLevel1[40];
		mappingTableLevel2[43] = mappingTableLevel2[40];
		mappingTableLevel3[43] = mappingTableLevel3[40];
		mappingTableLevel4[43] = mappingTableLevel4[40];
		if (supportLevels5and6) {
			mappingTableLevel5[43] = mappingTableLevel5[40];
			mappingTableLevel6[43] = mappingTableLevel6[40];
		}
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
		case 5:
			return mappingTableLevel5[in];
		case 6:
			return mappingTableLevel6[in];
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
			sendChar(L'̃', keyInfo);  // perispomene (Tilde)
			return true;
		case 41:
			sendChar(L'̌', keyInfo);  // caron, wedge, háček (Hatschek)
			return true;
		default:
			return false;
	}

}

bool handleLayer3SpecialCases(KBDLLHOOKSTRUCT keyInfo)
{
	switch(keyInfo.scanCode) {
		case 13:
			sendChar(L'̊', keyInfo);  // overring
			return true;
		case 20:
			sendChar(L'^', keyInfo);
			keybd_event(VK_SPACE, 0, 0, 0);
			return true;
		case 27:
			sendChar(L'̷', keyInfo);  // bar (diakritischer Schrägstrich)
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
			sendChar(L'¨', keyInfo);  // diaeresis, umlaut
			return true;
		case 27:
			sendChar(L'˝', keyInfo);  // double acute (doppelter Akut)
			return true;
		case 41:
			sendChar(L'̇', keyInfo);  // dot above (Punkt, darüber)
			return true;
	}

	// A second level 4 mapping table for special (non-unicode) keys.
	// Maybe this could be included in the global TCHAR mapping table or level 4!?
	CHAR mappingTable[LEN];
	for (int i = 0; i < LEN; i++)
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
		// If arrow key, page up/down, home or end,
		// send flag 0x01 (bit 0 = extended).
		// This in necessary for selecting text with shift + arrow.
		if (mappingTable[keyInfo.scanCode]==VK_LEFT
			|| mappingTable[keyInfo.scanCode]==VK_RIGHT
			|| mappingTable[keyInfo.scanCode]==VK_UP
			|| mappingTable[keyInfo.scanCode]==VK_DOWN
			|| mappingTable[keyInfo.scanCode]==VK_PRIOR
			|| mappingTable[keyInfo.scanCode]==VK_NEXT
			|| mappingTable[keyInfo.scanCode]==VK_HOME
			|| mappingTable[keyInfo.scanCode]==VK_END)
			keybd_event(mappingTable[keyInfo.scanCode], 0, 0x01, 0);
		else
			keybd_event(mappingTable[keyInfo.scanCode], 0, 0, 0);
		return true;
	}
	return false;
}

bool isShift(KBDLLHOOKSTRUCT keyInfo)
{
	return keyInfo.vkCode == VK_SHIFT
	    || keyInfo.vkCode == VK_LSHIFT
	    || keyInfo.vkCode == VK_RSHIFT;
}

bool isMod3(KBDLLHOOKSTRUCT keyInfo)
{
	return keyInfo.vkCode == VK_CAPITAL
	    || keyInfo.scanCode == scanCodeMod3R;
}

bool isMod4(KBDLLHOOKSTRUCT keyInfo)
{
	return keyInfo.vkCode == VK_RMENU
	    || keyInfo.vkCode == VK_OEM_102; // |<> key
}

bool isSystemKeyPressed() {
	return ctrlLeftPressed || ctrlRightPressed
	    || altLeftPressed
	    || winLeftPressed || winRightPressed;
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
			keyName = "(Alt right)";
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

		if (keyInfo.flags & LLKHF_INJECTED) {
			// process injected events like normal, because most probably we are injecting them
			logKeyEvent("injected", keyInfo);
			return CallNextHookEx(NULL, code, wparam, lparam);
		}
	}

	if (code == HC_ACTION && wparam == WM_KEYDOWN &&
		shiftPressed && keyInfo.scanCode == 69) {
		// Shift + Pause
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
			if (keyInfo.scanCode == scanCodeMod3R)
				level3modRightPressed = false;
			else  // VK_CAPITAL (CapsLock)
				level3modLeftPressed = false;
			mod3Pressed = level3modLeftPressed | level3modRightPressed;
			return -1;
		} else if (isMod4(keyInfo)) {
			if (keyInfo.vkCode == VK_OEM_102)
				level4modLeftPressed = false;
			else  // VK_RMENU (AltGr)
				level4modRightPressed = false;
			mod4Pressed = level4modLeftPressed | level4modRightPressed;
			return -1;
		}

		// Check also the scan code because AltGr sends VK_LCONTROL with scanCode 541
		if (keyInfo.vkCode == VK_LCONTROL && keyInfo.scanCode == 29) {
			if (swapLeftCtrlAndLeftAlt) {
				altLeftPressed = false;
				keybd_event(VK_LMENU, 0, KEYEVENTF_KEYUP, 0);
			} else {
				ctrlLeftPressed = false;
				keybd_event(VK_LCONTROL, 0, KEYEVENTF_KEYUP, 0);
			}
			return -1;
		} else if (keyInfo.vkCode == VK_RCONTROL) {
			ctrlRightPressed = false;
		} else if (keyInfo.vkCode == VK_LMENU) {
			if (swapLeftCtrlAndLeftAlt) {
				ctrlLeftPressed = false;
				keybd_event(VK_LCONTROL, 0, KEYEVENTF_KEYUP, 0);
			} else {
				altLeftPressed = false;
				keybd_event(VK_LMENU, 0, KEYEVENTF_KEYUP, 0);
			}
			return -1;
		} else if (keyInfo.vkCode == VK_LWIN) {
			winLeftPressed = false;
		} else if (keyInfo.vkCode == VK_RWIN) {
			winRightPressed = false;
		}
	}

	else if (code == HC_ACTION && (wparam == WM_SYSKEYDOWN || wparam == WM_KEYDOWN)) {
		logKeyEvent("\nkey down", keyInfo);

		// Check also the scan code because AltGr sends VK_LCONTROL with scanCode 541
		if (keyInfo.vkCode == VK_LCONTROL && keyInfo.scanCode == 29) {
			if (swapLeftCtrlAndLeftAlt) {
				altLeftPressed = true;
				keybd_event(VK_LMENU, 0, 0, 0);
			} else {
				ctrlLeftPressed = true;
				keybd_event(VK_LCONTROL, 0, 0, 0);
			}
			return -1;
		} else if (keyInfo.vkCode == VK_RCONTROL) {
			ctrlRightPressed = true;
		} else if (keyInfo.vkCode == VK_LMENU) {
			if (swapLeftCtrlAndLeftAlt) {
				ctrlLeftPressed = true;
				keybd_event(VK_LCONTROL, 0, 0, 0);
			} else {
				altLeftPressed = true;
				keybd_event(VK_LMENU, 0, 0, 0);
			}
			return -1;
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
			if (supportLevels5and6 && level == 2)
				level = 5;
			else
				level = 3;
		if (mod4Pressed)
			if (supportLevels5and6 && level == 3)
				level = 6;
			else
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
			return -1;
		} else if (isMod3(keyInfo)) {
			if (keyInfo.scanCode == scanCodeMod3R)
				level3modRightPressed = true;
			else  // VK_CAPITAL (CapsLock)
				level3modLeftPressed = true;
			mod3Pressed = level3modLeftPressed | level3modRightPressed;
			return -1;
		} else if (isMod4(keyInfo)) {
			if (keyInfo.vkCode == VK_OEM_102) {
				level4modLeftPressed = true;
			} else { // VK_RMENU (AltGr)
				level4modRightPressed = true;
				/* ALTGR triggers two keys: LCONTROL and RMENU
				   we don't want to have any of those two here effective but return -1 seems
				   to change nothing, so we simply send keyup here.  */
				keybd_event(VK_RMENU, 0, KEYEVENTF_KEYUP, 0);
			}
			mod4Pressed = level4modLeftPressed | level4modRightPressed;
			return -1;
		} else if (level == 2 && handleLayer2SpecialCases(keyInfo)) {
			return -1;
		} else if (level == 3 && handleLayer3SpecialCases(keyInfo)) {
			return -1;
		} else if (level == 4 && handleLayer4SpecialCases(keyInfo)) {
			return -1;
		} else if (!(qwertzForShortcuts && isSystemKeyPressed())) {
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

bool fileExists(LPCSTR szPath)
{
	DWORD dwAttrib = GetFileAttributesA(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES
	        && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int main(int argc, char *argv[])
{
	/**
	* find settings.ini (in same folder as neo-llkh.exe)
	*/
	// get path of neo-llkh.exe
	char ini[256];
	GetModuleFileNameA(NULL, ini, 256);
	//printf("exe: %s\n", ini);
	char * pch;
	// find last \ in path
	pch = strrchr(ini, '\\');
	// replace neo-llkh.exe by settings.ini
	strcpy(pch+1, "settings.ini");
	//printf("ini: %s\n", ini);

	/**
	* If settings.ini exists, read in settings.
	* Otherwise check for command line parameters.
	*/
	if (fileExists(ini)) {
		char returnValue[100];

		GetPrivateProfileStringA("Settings", "layout", "neo", layout, 100, ini);

		GetPrivateProfileStringA("Settings", "symmetricalLevel3Modifiers", "0", returnValue, 100, ini);
		quoteAsMod3R = (strcmp(returnValue, "1") == 0);

		GetPrivateProfileStringA("Settings", "shiftLockEnabled", "0", returnValue, 100, ini);
		shiftLockEnabled = (strcmp(returnValue, "1") == 0);

		GetPrivateProfileStringA("Settings", "qwertzForShortcuts", "0", returnValue, 100, ini);
		qwertzForShortcuts = (strcmp(returnValue, "1") == 0);

		GetPrivateProfileStringA("Settings", "swapLeftCtrlAndLeftAlt", "0", returnValue, 100, ini);
		swapLeftCtrlAndLeftAlt = (strcmp(returnValue, "1") == 0);

		GetPrivateProfileStringA("Settings", "supportLevels5and6", "0", returnValue, 100, ini);
		supportLevels5and6 = (strcmp(returnValue, "1") == 0);

		printf("Einstellungen aus %s:\n", ini);
		printf("Layout: %s\n", layout);
		printf("quoteAsMod3R: %d\n", quoteAsMod3R);
		printf("shiftLockEnabled: %d\n", shiftLockEnabled);
		printf("qwertzForShortcuts: %d\n", qwertzForShortcuts);
		printf("swapLeftCtrlAndLeftAlt: %d\n", swapLeftCtrlAndLeftAlt);
		printf("supportLevels5and6: %d\n\n", supportLevels5and6);

		if (argc >= 2)
			printf("Kommandozeilenparameter werden ignoriert, da eine settings.ini gefunden wurde!\n\n");

	} else {
		printf("Keine Einstellungen gefunden: %s\n", ini);

		// the first parameter sets the layout (optional):
		// neo (default), adnw, adnwzjf, koy, kou
		if (argc >= 2)
			//layout = argv[1];
			strcpy(layout, argv[1]);
		else
			//layout = "neo";
			strcpy(layout, "neo");

		// if the second parameter is 1, quote and backslash (ä and #) will be swapped
		if (argc >= 3 && strcmp(argv[2], "1") == 0) {
			quoteAsMod3R = true;
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
	}

	if (quoteAsMod3R)
		// ä/quote key instead of #/backslash key for the right level 3 modifier
		scanCodeMod3R = 40;

	if (swapLeftCtrlAndLeftAlt)
		// catch ctrl-c because it will send keydown for ctrl
		// but then keyup for alt. Then ctrl would be locked.
		SetConsoleCtrlHandler(CtrlHandler, TRUE);

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
