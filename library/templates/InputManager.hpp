#ifndef INPUT_MANAGER_HPP_
#define INPUT_MANAGER_HPP_
#include <Keyboard.hpp>
#include <Mouse.hpp>
#include <Gamepad.hpp>

class InputManager
{
public:
	virtual ~InputManager() = default;

	virtual void AddGamepadSupport(size_t count) noexcept = 0;

	[[nodiscard]]
	virtual size_t GetGamepadCount() const noexcept = 0;

	[[nodiscard]]
	virtual	const Keyboard& GetKeyboard() const noexcept = 0;
	[[nodiscard]]
	virtual Keyboard& GetKeyboard() noexcept = 0;

	[[nodiscard]]
	virtual	const Mouse& GetMouse() const noexcept = 0;
	[[nodiscard]]
	virtual Mouse& GetMouse() noexcept = 0;

	[[nodiscard]]
	virtual	const Gamepad& GetGamepad(size_t index = 0u) const noexcept = 0;
	[[nodiscard]]
	virtual	Gamepad& GetGamepad(size_t index = 0u) noexcept = 0;

	virtual void DisconnectGamepad(size_t index = 0u) noexcept = 0;

	virtual void ClearInputStates() noexcept = 0;
};
#endif
