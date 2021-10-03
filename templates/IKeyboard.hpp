#ifndef __IKEYBOARD_HPP__
#define __IKEYBOARD_HPP__
#include <cstdint>

#define PLUTO_DLL __declspec(dllimport)

enum PLUTO_DLL SKeyCodes {
	Default,
	BackSpace,
	Tab,
	Enter,
	Shift,
	Ctrl,
	Alt,
	CapsLock,
	Esc,
	SpaceBar,
	PageUp,
	PageDown,
	End,
	Home,
	LeftArrow,
	UpArrow,
	RightArrow,
	DownArrow,
	PrintScreen,
	Ins,
	Del,
	Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	Super,
	ZeroNumpad, OneNumpad, TwoNumpad, ThreeNumpad, FourNumpad, FiveNumpad, SixNumpad,
	SevenNumpad, EightNumpad, NineNumpad,
	Multiply, Divide, Add, Subtract, Decimal,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	NumLock, ScrollLock,
	ShiftLeft, ShiftRight,
	CtrlLeft, CtrlRight,
	AltLeft, AltRight,
	SemiColonUS,
	Plus,
	Comma,
	Hyphen,
	Period,
	SlashUS,
	TildeUS,
	BraceStartUS, BraceEndUS,
	BackSlashUS,
	QuoteUS
};

class PLUTO_DLL IKeyboard {
public:
	class Event {
	public:
		enum class Type {
			Press,
			Release,
			Invalid
		};

	private:
		Type m_type;
		SKeyCodes m_code;

	public:
		Event() noexcept
			: m_type(Type::Invalid), m_code(SKeyCodes::Default) {}
		Event(Type type, SKeyCodes code) noexcept
			: m_type(type), m_code(code) {}
		bool IsPress() const noexcept {
			return m_type == Type::Press;
		}
		bool IsRelease() const noexcept {
			return m_type == Type::Release;
		}
		bool IsValid() const noexcept {
			return m_type != Type::Invalid;
		}
		SKeyCodes GetCode() const noexcept {
			return m_code;
		}
	};

public:
	virtual ~IKeyboard() = default;

	// Must set a Native Keycode Getter in Window Module
	virtual void SetNativeKeyCodeGetter(
		SKeyCodes(*NativeKeyCodeGetter)(std::uint16_t)
	) noexcept = 0;

	// key events
	virtual bool IsKeyPressed(SKeyCodes keycode) const noexcept = 0;
	virtual Event ReadKey() noexcept = 0;
	virtual bool IsKeyEmpty() const noexcept = 0;
	virtual void FlushKey() noexcept = 0;

	// char events
	virtual char ReadChar() noexcept = 0;
	virtual bool IsCharEmpty() const noexcept = 0;
	virtual void FlushChar() noexcept = 0;
	virtual void Flush() noexcept = 0;

	// auto-repeat control
	virtual void EnableAutoRepeat() noexcept = 0;
	virtual void DisableAutoRepeat() noexcept = 0;
	virtual bool IsAutoRepeatEnabled() const noexcept = 0;

	virtual void OnKeyPressed(std::uint16_t keycode) noexcept = 0;
	virtual void OnKeyReleased(std::uint16_t keycode) noexcept = 0;
	virtual void OnChar(char character) noexcept = 0;
	virtual void ClearState() noexcept = 0;
};

PLUTO_DLL IKeyboard* _cdecl GetKeyboardInstance() noexcept;
PLUTO_DLL void _cdecl InitKeyboardInstance();
PLUTO_DLL void _cdecl CleanUpKeyboardInstance() noexcept;

#endif
