#ifndef INPUT_MANAGER_HPP_
#define INPUT_MANAGER_HPP_
#include <IKeyboard.hpp>
#include <IMouse.hpp>
#include <IGamepad.hpp>

class InputManager {
public:
	virtual ~InputManager() = default;

	virtual void AddGamepadSupport(size_t count) noexcept = 0;

	[[nodiscard]]
	virtual size_t GetGamepadCount() const noexcept = 0;

	[[nodiscard]]
	virtual	IKeyboard& GetKeyboard() const noexcept = 0;
	[[nodiscard]]
	virtual	IMouse& GetMouse() const noexcept = 0;
	[[nodiscard]]
	virtual	IGamepad& GetGamepad(size_t index = 0u) const noexcept = 0;
	[[nodiscard]]
	virtual	IKeyboard* GetKeyboardRef() const noexcept = 0;
	[[nodiscard]]
	virtual	IMouse* GetMouseRef() const noexcept = 0;
	[[nodiscard]]
	virtual	IGamepad* GetGamepadRef(size_t index = 0u) const noexcept = 0;

	virtual void DisconnectGamepad(size_t index = 0u) noexcept = 0;

	virtual void ClearInputStates() noexcept = 0;
};
#endif
