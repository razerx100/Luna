#ifndef __I_GAMEPAD_HPP__
#define __I_GAMEPAD_HPP__
#include <cstdint>

#define PLUTO_DLL __declspec(dllimport)

enum class PLUTO_DLL XBoxButton {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	START,
	BACK,
	LEFT_THUMB,
	RIGHT_THUMB,
	LEFT_SHOULDER,
	RIGHT_SHOULDER,
	A,
	B,
	X,
	Y
};

struct PLUTO_DLL Vector2D {
	float x;
	float y;
};

class PLUTO_DLL IGamepad {
public:
	class Event {
	public:
		enum class Type {
			LeftThumbStick,
			RightThumbStick,
			LeftTrigger,
			RightTrigger,
			Invalid
		};

	private:
		Type m_type;
		Vector2D m_direction;

	public:
		Event() noexcept
			: m_type(Type::Invalid),
			m_direction{} {}

		Event(Type type, float magnitude) noexcept
			: m_type(type),
			m_direction{magnitude, 0.0f} {}

		Event(Type type, const Vector2D& position) noexcept
			: m_type(type),
			m_direction(position) {}

		bool IsValid() const noexcept {
			return m_type != Type::Invalid;
		}

		Type GetType() const noexcept {
			return m_type;
		}

		Vector2D GetPos() const noexcept {
			return m_direction;
		}

		float GetMagnitude() const noexcept {
			return m_direction.x;
		}
	};

public:
	virtual ~IGamepad() = default;

	virtual void ClearState() noexcept = 0;

	virtual Event Read() noexcept = 0;
	virtual bool IsBufferEmpty() const noexcept = 0;

	virtual bool IsButtonPressed(XBoxButton button) const noexcept = 0;
	virtual bool AreButtonsPressed(int count, ...) const noexcept = 0;

	virtual void OnLeftThumbStickMove(float data) noexcept = 0;
	virtual void OnRightThumbStickMove(float data) noexcept = 0;
	virtual void OnLeftTriggerMove(float data) noexcept = 0;
	virtual void OnRightTriggerMove(float data) noexcept = 0;

	virtual void SetRawButtonState(std::uint16_t buttonFlags) noexcept = 0;
	virtual void SetLeftThumbStickDeadZone(std::uint32_t deadzone) noexcept = 0;
	virtual void SetRightThumbStickDeadZone(std::uint32_t deadzone) noexcept = 0;
	virtual void SetTriggerThreshold(std::uint32_t threshold) noexcept = 0;

	virtual std::uint32_t GetLeftThumbStickDeadZone() const noexcept = 0;
	virtual std::uint32_t GetRightThumbStickDeadZone() const noexcept = 0;
	virtual std::uint32_t GetTriggerThreshold() const noexcept = 0;
};
#endif
