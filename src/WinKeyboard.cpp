#include <IKeyboard.hpp>
#include <CleanWin.hpp>
#include <WinWindow.hpp>

using enum SKeyCodes;

static const SKeyCodes WinKeyMap[] = {
	Default, Default, Default, Default, Default, Default, Default, Default,
	BackSpace, Tab,
	Default, Default, Default,
	Enter,
	Default, Default,
	Shift, Ctrl, Alt,
	Default,
	CapsLock,
	Default, Default, Default, Default, Default, Default,
	Esc,
	Default, Default, Default, Default,
	SpaceBar, PageUp, PageDown, End, Home,
	LeftArrow, UpArrow, RightArrow, DownArrow,
	Default, Default, Default,
	PrintScreen, Ins, Del,
	Default,
	Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
	Default, Default, Default, Default, Default, Default, Default,
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	Super,
	Default, Default, Default, Default,
	ZeroNumpad, OneNumpad, TwoNumpad, ThreeNumpad, FourNumpad, FiveNumpad,
	SixNumpad, SevenNumpad, EightNumpad, NineNumpad,
	Multiply, Add,
	Default,
	Subtract, Decimal, Divide,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	Default, Default, Default, Default, Default, Default,
	Default, Default, Default, Default, Default, Default,
	Default, Default, Default, Default, Default, Default,
	Default, Default,
	NumLock, ScrollLock,
	Default, Default, Default, Default, Default, Default,
	Default, Default, Default, Default, Default, Default,
	Default, Default,
	ShiftLeft, ShiftRight, CtrlLeft, CtrlRight, AltLeft, AltRight,
	Default, Default, Default, Default, Default, Default,
	Default, Default, Default, Default, Default, Default,
	Default, Default, Default, Default, Default, Default,
	Default, Default,
	SemiColonUS, Plus, Comma, Hyphen, Period, SlashUS, TildeUS,
	Default, Default, Default, Default, Default, Default,
	Default, Default, Default, Default, Default, Default,
	Default, Default, Default, Default, Default, Default,
	Default, Default, Default, Default, Default, Default,
	Default, Default,
	BraceStartUS, BackSlashUS, BraceEndUS, QuoteUS,
	Default, Default, Default, Default, Default, Default,
	Default, Default, Default, Default, Default, Default,
	Default, Default, Default, Default, Default, Default,
	Default, Default, Default, Default, Default, Default,
	Default, Default, Default, Default, Default, Default,
	Default, Default
};

SKeyCodes GetSKeyCodes(std::uint16_t nativeKeycode) {
	return WinKeyMap[nativeKeycode];
}
