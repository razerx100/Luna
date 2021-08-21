#ifndef __KEYBOARD_HPP__
#define __KEYBOARD_HPP__
#include <optional>
#include <queue>
#include <bitset>
#include <memory>

enum SKeyCodes {
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

SKeyCodes GetSKeyCodes(std::uint16_t nativeKeycode);

class Keyboard {
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
	Keyboard()
		: m_autoRepeatEnabled(false) {}

	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;

	// key events
	bool IsKeyPressed(SKeyCodes keycode) const noexcept;
	std::optional<Event> ReadKey() noexcept;
	bool IsKeyEmpty() const noexcept;
	void FlushKey() noexcept;

	// char events
	std::optional<char> ReadChar() noexcept;
	bool IsCharEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;

	// auto-repeat control
	void EnableAutoRepeat() noexcept;
	void DisableAutoRepeat() noexcept;
	bool IsAutoRepeatEnabled() const noexcept;

	static void Init();
	static Keyboard* GetRef() noexcept;

	void OnKeyPressed(std::uint16_t keycode) noexcept;
	void OnKeyReleased(std::uint16_t keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;

private:
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;

private:
	static constexpr unsigned int s_nKeys = 256u;
	static constexpr unsigned int s_bufferSize = 16u;
	bool m_autoRepeatEnabled;
	std::bitset<s_nKeys> m_keystates;
	std::queue<Event> m_keyBuffer;
	std::queue<char> m_charBuffer;

	static std::unique_ptr<Keyboard> s_instance;
};
#endif
