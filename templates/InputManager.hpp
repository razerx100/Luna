#ifndef INPUT_MANAGER_HPP_
#define INPUT_MANAGER_HPP_
#include <IKeyboard.hpp>
#include <IMouse.hpp>
#include <IGamepad.hpp>
#include <vector>

enum class DeviceType {
	Keyboard,
	Mouse,
	Gamepad,
	Invalid
};

class InputManager {
public:
	virtual ~InputManager() = default;

	virtual void AddDeviceSupport(DeviceType device, size_t count = 1u) noexcept = 0;
	virtual void DisconnectDevice(std::uint64_t handle) noexcept = 0;

	[[nodiscard]]
	virtual std::vector<IKeyboard*> GetKeyboardRefs() const noexcept = 0;
	[[nodiscard]]
	virtual std::vector<IMouse*> GetMouseRefs() const noexcept = 0;
	[[nodiscard]]
	virtual std::vector<IGamepad*> GetGamepadRefs() const noexcept = 0;

	[[nodiscard]]
	virtual size_t GetKeyboardCount() const noexcept = 0;
	[[nodiscard]]
	virtual size_t GetMouseCount() const noexcept = 0;
	[[nodiscard]]
	virtual size_t GetGamepadCount() const noexcept = 0;

	[[nodiscard]]
	virtual	IKeyboard* GetKeyboard() const noexcept = 0;
	[[nodiscard]]
	virtual	IMouse* GetMouse() const noexcept = 0;
	[[nodiscard]]
	virtual	IGamepad* GetGamepad() const noexcept = 0;

	[[nodiscard]]
	virtual	IKeyboard* GetKeyboardByIndex(size_t index) const noexcept = 0;
	[[nodiscard]]
	virtual	IMouse* GetMouseByIndex(size_t index) const noexcept = 0;
	[[nodiscard]]
	virtual	IGamepad* GetGamepadByIndex(size_t index) const noexcept = 0;

	[[nodiscard]]
	virtual	IKeyboard* GetKeyboardByHandle(std::uint64_t handle) noexcept = 0;
	[[nodiscard]]
	virtual	IMouse* GetMouseByHandle(std::uint64_t handle) noexcept = 0;

	virtual void DisconnectGamepadByIndex(size_t index) noexcept = 0;

	virtual void ClearInputStates() noexcept = 0;
};

#endif
