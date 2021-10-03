#ifndef __IMOUSE_HPP__
#define __IMOUSE_HPP__

#define PLUTO_DLL __declspec(dllimport)

struct PLUTO_DLL Vector2 {
	int x;
	int y;
};

class PLUTO_DLL IMouse {
public:
	struct MouseData {
		bool leftPressed = false;
		bool rightPressed = false;
		bool middlePressed = false;
		int x;
		int y;
	};

	class Event {
	public:
		enum class Type {
			LPress,
			LRelease,
			MPress,
			MRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
			Invalid
		};

	private:
		Type m_type;
		MouseData m_data;

	public:
		Event() noexcept
			: m_type(Type::Invalid),
			m_data{} {}

		Event(Type type, const MouseData& data) noexcept
			: m_type(type),
			m_data(data) {}

		bool IsValid() const noexcept {
			return m_type != Type::Invalid;
		}

		Type GetType() const noexcept {
			return m_type;
		}

		Vector2 GetPos() const noexcept {
			return { m_data.x, m_data.y };
		}

		int GetPosX() const noexcept {
			return m_data.x;
		}

		int GetPosY() const noexcept {
			return m_data.y;
		}

		bool IsLeftPressed() const noexcept {
			return m_data.leftPressed;
		}

		bool IsMiddlePressed() const noexcept {
			return m_data.middlePressed;
		}

		bool IsRightPressed() const noexcept {
			return m_data.rightPressed;
		}
	};

public:
	virtual ~IMouse() = default;

	virtual Vector2 GetPos() const noexcept = 0;
	virtual int GetPosX() const noexcept = 0;
	virtual int GetPosY() const noexcept = 0;
	virtual Event Read() noexcept = 0;
	virtual Vector2 ReadRawDelta() noexcept = 0;

	virtual bool IsInWindow() const noexcept = 0;
	virtual bool IsLeftPressed() const noexcept = 0;
	virtual bool IsMiddlePressed() const noexcept = 0;
	virtual bool IsRightPressed() const noexcept = 0;
	virtual bool IsBufferEmpty() const noexcept = 0;
	virtual bool IsRawEnabled() const noexcept = 0;

	virtual void Flush() noexcept = 0;
	virtual void EnableRaw() noexcept = 0;
	virtual void DisableRaw() noexcept = 0;

	virtual void OnMouseMove(int x, int y) noexcept = 0;
	virtual void OnMouseLeave() noexcept = 0;
	virtual void OnMouseEnter() noexcept = 0;
	virtual void OnLeftPress() noexcept = 0;
	virtual void OnMiddlePress() noexcept = 0;
	virtual void OnRightPress() noexcept = 0;
	virtual void OnLeftRelease() noexcept = 0;
	virtual void OnMiddleRelease() noexcept = 0;
	virtual void OnRightRelease() noexcept = 0;
	virtual void OnWheelUp() noexcept = 0;
	virtual void OnWheelDown() noexcept = 0;
	virtual void OnMouseRawDelta(int dx, int dy) noexcept = 0;
	virtual void OnWheelDelta(int delta) noexcept = 0;
};

PLUTO_DLL IMouse* _cdecl GetMouseInstance() noexcept;
PLUTO_DLL void _cdecl InitMouseInstance();
PLUTO_DLL void _cdecl CleanUpMouseInstance() noexcept;

#endif
