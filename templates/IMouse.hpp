#ifndef __IMOUSE_HPP__
#define __IMOUSE_HPP__
#include <utility>

typedef std::pair<std::int32_t, std::int32_t> PosDelta;

enum class __declspec(dllimport) MouseButtons {
	Left,
	Right,
	Middle,
	X1,
	X2,
	Invalid
};

class __declspec(dllimport) IMouse {
public:
	class Event {
	public:
		enum class Type {
			WheelUp,
			WheelDown,
			Enter,
			Leave,
			Press,
			Release,
			Invalid
		};

	private:
		Type m_type;
		MouseButtons m_button;

	public:
		Event() noexcept
			: m_type(Type::Invalid),
			m_button(MouseButtons::Invalid) {}

		Event(Type type) noexcept
			: m_type(type),
			m_button(MouseButtons::Invalid) {}

		Event(Type type, MouseButtons button) noexcept
			: m_type(type),
			m_button(button) {}

		[[nodiscard]]
		bool IsValid() const noexcept {
			return m_type != Type::Invalid;
		}

		[[nodiscard]]
		Type GetType() const noexcept {
			return m_type;
		}

		[[nodiscard]]
		MouseButtons GetButton() const noexcept {
			return m_button;
		}
	};

public:
	virtual ~IMouse() = default;

	[[nodiscard]]
	virtual PosDelta GetPosDelta() const noexcept = 0;
	[[nodiscard]]
	virtual int GetPosDX() const noexcept = 0;
	[[nodiscard]]
	virtual int GetPosDY() const noexcept = 0;
	[[nodiscard]]
	virtual float GetMouseTicks() const noexcept = 0;
	[[nodiscard]]
	virtual Event Read() noexcept = 0;

	[[nodiscard]]
	virtual bool IsInWindow() const noexcept = 0;
	[[nodiscard]]
	virtual bool IsButtonPressed(MouseButtons button) const noexcept = 0;
	[[nodiscard]]
	virtual bool AreButtonsPressed(int count, ...) const noexcept = 0;

	virtual void Flush() noexcept = 0;

	virtual void SetPressState(std::uint16_t mouseState) noexcept = 0;
	virtual void SetReleaseState(std::uint16_t mouseState) noexcept = 0;
	virtual void OnMouseMove(int dx, int dy) noexcept = 0;
	virtual void OnMouseLeave() noexcept = 0;
	virtual void OnMouseEnter() noexcept = 0;
	virtual void OnWheelDelta(short delta) noexcept = 0;
};
#endif
