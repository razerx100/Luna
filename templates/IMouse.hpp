#ifndef __IMOUSE_HPP__
#define __IMOUSE_HPP__

#define PLUTO_DLL __declspec(dllimport)

struct PLUTO_DLL Vector2 {
	int x;
	int y;
};

class PLUTO_DLL IMouse {
public:
	class Event {
	public:
		enum Type {
			LPress,
			LRelease,
			RPress,
			RRelease,
			MPress,
			MRelease,
			X1Press,
			X1Release,
			X2Press,
			X2Release,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
			Invalid
		};

	private:
		Type m_type;
		Vector2 m_position;

	public:
		Event() noexcept
			: m_type(Type::Invalid),
			m_position{} {}

		Event(Type type) noexcept
			: m_type(type),
			m_position{} {}

		Event(Type type, const Vector2& position) noexcept
			: m_type(type),
			m_position(position) {}

		bool IsValid() const noexcept {
			return m_type != Type::Invalid;
		}

		Type GetType() const noexcept {
			return m_type;
		}

		Vector2 GetPos() const noexcept {
			return m_position;
		}

		int GetPosX() const noexcept {
			return m_position.x;
		}

		int GetPosY() const noexcept {
			return m_position.y;
		}
	};

public:
	virtual ~IMouse() = default;

	virtual Vector2 GetPos() const noexcept = 0;
	virtual int GetPosX() const noexcept = 0;
	virtual int GetPosY() const noexcept = 0;
	virtual float GetMouseTicks() const noexcept = 0;
	virtual Event Read() noexcept = 0;

	virtual bool IsInWindow() const noexcept = 0;
	virtual bool IsLeftPressed() const noexcept = 0;
	virtual bool IsMiddlePressed() const noexcept = 0;
	virtual bool IsRightPressed() const noexcept = 0;
	virtual bool IsX1Pressed() const noexcept = 0;
	virtual bool IsX2Pressed() const noexcept = 0;
	virtual bool IsBufferEmpty() const noexcept = 0;

	virtual void Flush() noexcept = 0;

	virtual void SetRawMouseState(std::uint16_t mouseState) noexcept = 0;
	virtual void OnMouseMove(int x, int y) noexcept = 0;
	virtual void OnMouseLeave() noexcept = 0;
	virtual void OnMouseEnter() noexcept = 0;
	virtual void OnWheelDelta(short delta) noexcept = 0;
};
#endif
