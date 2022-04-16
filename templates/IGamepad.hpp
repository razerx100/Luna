#ifndef I_GAMEPAD_HPP_
#define I_GAMEPAD_HPP_
#include <cstdint>

enum class XBoxButton {
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
	Y,
	Invalid
};

struct ASData {
	float magnitude;
	float xDirection;
	float yDirection;
};

class IGamepad {
public:
	class Event {
	public:
		enum class Type {
			LeftThumbStick,
			RightThumbStick,
			LeftTrigger,
			RightTrigger,
			Press,
			Release,
			Invalid
		};

	private:
		Type m_type;
		XBoxButton m_button;
		ASData m_data;

	public:
		Event() noexcept
			: m_type(Type::Invalid),
			m_button(XBoxButton::Invalid),
			m_data{} {}

		Event(Type type, float magnitude) noexcept
			: m_type(type),
			m_button(XBoxButton::Invalid),
			m_data{magnitude, 0.0f, 0.0f} {}

		Event(Type type, ASData data) noexcept
			: m_type(type),
			m_button(XBoxButton::Invalid),
			m_data(data) {}

		Event(Type type, XBoxButton button) noexcept
			: m_type(type),
			m_button(button),
			m_data{} {}

		[[nodiscard]]
		bool IsValid() const noexcept {
			return m_type != Type::Invalid;
		}

		[[nodiscard]]
		Type GetType() const noexcept {
			return m_type;
		}

		[[nodiscard]]
		ASData GetASData() const noexcept {
			return m_data;
		}

		[[nodiscard]]
		float GetMagnitude() const noexcept {
			return m_data.magnitude;
		}

		[[nodiscard]]
		XBoxButton GetButton() const noexcept {
			return m_button;
		}
	};

public:
	virtual ~IGamepad() = default;

	virtual void ClearState() noexcept = 0;

	[[nodiscard]]
	virtual Event Read() noexcept = 0;

	[[nodiscard]]
	virtual bool IsButtonPressed(XBoxButton button) const noexcept = 0;
	[[nodiscard]]
	virtual bool AreButtonsPressed(size_t count, ...) const noexcept = 0;

	virtual void OnLeftThumbStickMove(ASData data) noexcept = 0;
	virtual void OnRightThumbStickMove(ASData data) noexcept = 0;
	virtual void OnLeftTriggerMove(float data) noexcept = 0;
	virtual void OnRightTriggerMove(float data) noexcept = 0;

	virtual void SetRawButtonState(std::uint16_t buttonFlags) noexcept = 0;
	virtual void SetLeftThumbStickDeadZone(std::uint32_t deadzone) noexcept = 0;
	virtual void SetRightThumbStickDeadZone(std::uint32_t deadzone) noexcept = 0;
	virtual void SetTriggerThreshold(std::uint32_t threshold) noexcept = 0;

	[[nodiscard]]
	virtual std::uint32_t GetLeftThumbStickDeadZone() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetRightThumbStickDeadZone() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetTriggerThreshold() const noexcept = 0;
};
#endif
