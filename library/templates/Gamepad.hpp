#ifndef GAMEPAD_HPP_
#define GAMEPAD_HPP_
#include <cstdint>
#include <optional>

enum class XBoxButton
{
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

struct ThumbStickData
{
	float magnitude;
	float xDirection;
	float yDirection;
};

class Gamepad
{
public:
	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
			Invalid
		};

	private:
		Type       m_type;
		XBoxButton m_button;

	public:
		Event()
			: m_type{ Type::Invalid }, m_button{ XBoxButton::Invalid }
		{}

		Event(Type type, XBoxButton button)
			: m_type{ type }, m_button{ button }
		{}

		[[nodiscard]]
		bool IsValid() const noexcept
		{
			return m_type != Type::Invalid;
		}

		[[nodiscard]]
		Type GetType() const noexcept
		{
			return m_type;
		}

		[[nodiscard]]
		XBoxButton GetButton() const noexcept
		{
			return m_button;
		}
	};

public:
	virtual ~Gamepad() = default;

	virtual void ClearState() noexcept = 0;
	virtual void ClearBuffers() noexcept = 0;
	virtual void Flush() noexcept = 0;

	[[nodiscard]]
	virtual std::optional<Event> ReadEvent() noexcept = 0;
	[[nodiscard]]
	virtual std::optional<float> ReadLeftTriggerData() noexcept = 0;
	[[nodiscard]]
	virtual std::optional<float> ReadRightTriggerData() noexcept = 0;
	[[nodiscard]]
	virtual std::optional<ThumbStickData> ReadLeftThumbStickData() noexcept = 0;
	[[nodiscard]]
	virtual std::optional<ThumbStickData> ReadRightThumbStickData() noexcept = 0;

	[[nodiscard]]
	virtual bool IsButtonPressed(XBoxButton button) const noexcept = 0;
	[[nodiscard]]
	virtual bool AreButtonsPressed(size_t count, ...) const noexcept = 0;

	virtual void OnLeftThumbStickMove(ThumbStickData data) noexcept = 0;
	virtual void OnRightThumbStickMove(ThumbStickData data) noexcept = 0;
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
