#ifndef __MOUSE_HPP__
#define __MOUSE_HPP__
#include <optional>
#include <queue>
#include <memory>

class Mouse {
public:
	struct RawDelta {
		int x;
		int y;
	};

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

		std::pair<int, int> GetPos() const noexcept {
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
	Mouse();

	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;

	std::pair<int, int> GetPos() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	std::optional<Event> Read() noexcept;
	std::optional<RawDelta> ReadRawDelta() noexcept;

	bool IsInWindow() const noexcept;
	bool IsLeftPressed() const noexcept;
	bool IsMiddlePressed() const noexcept;
	bool IsRightPressed() const noexcept;
	bool IsBufferEmpty() const noexcept;
	bool IsRawEnabled() const noexcept;

	void Flush() noexcept;
	void EnableRaw() noexcept;
	void DisableRaw() noexcept;

	static void Init();
	static Mouse* GetRef() noexcept;

	void OnMouseMove(int x, int y) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnLeftPress() noexcept;
	void OnMiddlePress() noexcept;
	void OnRightPress() noexcept;
	void OnLeftRelease() noexcept;
	void OnMiddleRelease() noexcept;
	void OnRightRelease() noexcept;
	void OnWheelUp() noexcept;
	void OnWheelDown() noexcept;
	void OnMouseRawDelta(int dx, int dy) noexcept;
	void OnWheelDelta(int delta) noexcept;

private:
	void TrimBuffer() noexcept;
	void TrimRawDeltaBuffer() noexcept;

private:
	static constexpr unsigned int s_bufferSize = 16u;
	bool m_inWindow;
	int m_wheelDeltaCarry;
	bool m_rawEnabled;
	MouseData m_mouseData;

	std::queue<Event> m_buffer;
	std::queue<RawDelta> m_rawDeltaBuffer;
	static std::unique_ptr<Mouse> s_instance;
};
#endif
