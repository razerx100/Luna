#include <bitset>
#include <array>
#include <concepts>
#include <Gamepad.hpp>
#include <Mouse.hpp>
#include <InputManager.hpp>
#include <CleanWin.hpp>
#include <WinWindow.hpp>
#include <Xinput.h>

using enum SKeyCodes;

static constexpr std::array WinKeyMap
{
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
	Default, Default, Default
};

SKeyCodes GetSKeyCodes(std::uint16_t nativeKeycode) noexcept
{
	return WinKeyMap[nativeKeycode];
}

static constexpr std::array pressChecks
{
	RI_MOUSE_BUTTON_1_DOWN,
	RI_MOUSE_BUTTON_2_DOWN,
	RI_MOUSE_BUTTON_3_DOWN,
	RI_MOUSE_BUTTON_4_DOWN,
	RI_MOUSE_BUTTON_5_DOWN
};

static constexpr std::array releaseChecks
{
	RI_MOUSE_BUTTON_1_UP,
	RI_MOUSE_BUTTON_2_UP,
	RI_MOUSE_BUTTON_3_UP,
	RI_MOUSE_BUTTON_4_UP,
	RI_MOUSE_BUTTON_5_UP
};

template<std::integral T, size_t ArraySize>
[[nodiscard]]
static std::uint16_t MapBit(
	size_t state, const std::array<T, ArraySize>& checks, size_t index
) noexcept {
	const auto check = static_cast<size_t>(checks[index]);

	return static_cast<std::uint16_t>(((state & check) == check) << index);
}

std::pair<std::uint8_t, std::uint8_t> ProcessMouseRawButtons(std::uint16_t newState) noexcept
{
	std::uint8_t pressFlags   = 0u;
	std::uint8_t releaseFlags = 0u;

	constexpr auto mouseButtonCount = static_cast<size_t>(MouseButtons::Invalid);

	for (size_t index = 0u; index < mouseButtonCount; ++index)
	{
		pressFlags   |= MapBit(newState, pressChecks, index);
		releaseFlags |= MapBit(newState, releaseChecks, index);
	}

	return { pressFlags, releaseFlags };
}

static constexpr std::array gamepadButtonChecks
{
	XINPUT_GAMEPAD_DPAD_UP,
	XINPUT_GAMEPAD_DPAD_DOWN,
	XINPUT_GAMEPAD_DPAD_LEFT,
	XINPUT_GAMEPAD_DPAD_RIGHT,
	XINPUT_GAMEPAD_START,
	XINPUT_GAMEPAD_BACK,
	XINPUT_GAMEPAD_LEFT_THUMB,
	XINPUT_GAMEPAD_RIGHT_THUMB,
	XINPUT_GAMEPAD_LEFT_SHOULDER,
	XINPUT_GAMEPAD_RIGHT_SHOULDER,
	XINPUT_GAMEPAD_A,
	XINPUT_GAMEPAD_B,
	XINPUT_GAMEPAD_X,
	XINPUT_GAMEPAD_Y
};

std::uint16_t ProcessGamepadRawButtons(std::uint16_t state) noexcept
{
	std::uint16_t buttonFlags = 0u;

	constexpr auto gamepadButtonCount = static_cast<size_t>(XBoxButton::Invalid);

	for (size_t index = 0u; index < gamepadButtonCount; ++index)
		buttonFlags |= MapBit(state, gamepadButtonChecks, index);

	return buttonFlags;
}
