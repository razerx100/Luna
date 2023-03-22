#ifndef I_MOUSE_HPP_
#define I_MOUSE_HPP_
#include <utility>
#include <optional>

struct CursorCoord {
	std::uint16_t x;
	std::uint16_t y;
};

enum class MouseButtons {
	Left,
	Right,
	Middle,
	X1,
	X2,
	Invalid
};

class IMouse {
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
		Event() noexcept : m_type(Type::Invalid), m_button(MouseButtons::Invalid) {}

		Event(Type type) noexcept : m_type(type), m_button(MouseButtons::Invalid) {}

		Event(Type type, MouseButtons button) noexcept : m_type(type), m_button(button) {}

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
	virtual float GetMouseTicks() const noexcept = 0;
	[[nodiscard]]
	virtual std::optional<Event> ReadEvents() noexcept = 0;
	[[nodiscard]]
	virtual CursorCoord GetCurrentCursorCoord() const noexcept = 0;

	[[nodiscard]]
	virtual bool IsInWindow() const noexcept = 0;
	[[nodiscard]]
	virtual bool IsButtonPressed(MouseButtons button) const noexcept = 0;
	[[nodiscard]]
	virtual bool AreButtonsPressed(size_t count, ...) const noexcept = 0;

	virtual void Flush() noexcept = 0;

	virtual void SetPressState(std::uint8_t mouseState) noexcept = 0;
	virtual void SetReleaseState(std::uint8_t mouseState) noexcept = 0;
	virtual void SetCurrentCursorCoord(std::uint16_t xCoord, std::uint16_t yCoord) noexcept = 0;
	virtual void OnMouseLeave() noexcept = 0;
	virtual void OnMouseEnter() noexcept = 0;
	virtual void OnWheelDelta(std::int16_t delta) noexcept = 0;
	virtual void ClearState() noexcept = 0;
};
#endif
