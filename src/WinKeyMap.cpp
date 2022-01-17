#include <InputManager.hpp>
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

SKeyCodes GetSKeyCodes(std::uint16_t nativeKeycode) noexcept {
	return WinKeyMap[nativeKeycode];
}

static constexpr std::uint16_t mouseReleaseFlag = 0x2AAu;
static constexpr std::uint16_t mousePressFlag = 0x155u;

std::pair<std::uint8_t, std::uint8_t> ProcessMouseRawButtons(
	std::uint16_t newState
) noexcept {
	std::uint16_t onFlags = newState & mousePressFlag;
	std::uint16_t offFlags = (newState & mouseReleaseFlag) >> 1u;

	std::uint8_t mapOn = 0u;
	std::uint8_t mapOff = 0u;

	for (size_t index = 0, index1 = 0; index < 16u; index += 2, ++index1) {
		mapOn |= static_cast<bool>(onFlags & (1u << index)) << index1;
		mapOff |= static_cast<bool>(offFlags & (1u << index)) << index1;
	}

	return { mapOn, mapOff };
}

std::uint16_t ProcessGamepadRawButtons(std::uint16_t state) noexcept {
	std::uint16_t map = 0u;

	for (size_t index = 0; index < 12u; ++index)
		map |= static_cast<bool>(state & (1u << index)) << index;

	for (size_t index = 12, index1 = 10; index < 16u; ++index, ++index1)
		map |= static_cast<bool>(state & (1u << index)) << index1;

	return map;
}
