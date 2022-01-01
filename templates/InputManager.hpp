#ifndef __INPUT_MANAGER_HPP__
#define __INPUT_MANAGER_HPP__
#include <IKeyboard.hpp>
#include <IMouse.hpp>
#include <IGamepad.hpp>
#include <vector>

enum class __declspec(dllimport) DeviceType {
	Keyboard,
	Mouse,
	Gamepad,
	DeviceTypeCount
};

struct __declspec(dllimport) GamepadData {
	IGamepad* pGamepad = nullptr;
	std::uint8_t index = 0u;
};

class __declspec(dllimport) InputManager {
public:
	virtual ~InputManager() = default;

	virtual void AddDeviceSupport(
		DeviceType device, std::uint32_t count = 1u
	) noexcept = 0;
	virtual void DeviceDisconnected(
		std::uint64_t handle
	) noexcept = 0;

	[[nodiscard]]
	virtual std::vector<IKeyboard*> GetKeyboardRefs() const noexcept = 0;
	[[nodiscard]]
	virtual std::vector<IMouse*> GetMouseRefs() const noexcept = 0;
	[[nodiscard]]
	virtual std::vector<IGamepad*> GetGamepadRefs() const noexcept = 0;

	[[nodiscard]]
	virtual std::uint32_t GetKeyboardsCount() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetMousesCount() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint32_t GetGamepadsCount() const noexcept = 0;

	[[nodiscard]]
	virtual	IKeyboard* GetKeyboardByIndex(std::uint32_t index = 0u) const noexcept = 0;
	[[nodiscard]]
	virtual	IMouse* GetMouseByIndex(std::uint32_t index = 0u) const noexcept = 0;
	[[nodiscard]]
	virtual	IGamepad* GetGamepadByIndex(std::uint32_t index = 0u) const noexcept = 0;
	[[nodiscard]]
	virtual	IKeyboard* GetKeyboardByHandle(std::uint64_t handle) noexcept = 0;
	[[nodiscard]]
	virtual	IMouse* GetMouseByHandle(std::uint64_t handle) noexcept = 0;
	[[nodiscard]]
	virtual	GamepadData GetGamepadByHandle(std::uint64_t handle) noexcept = 0;

	virtual void ClearInputStates() noexcept = 0;
};

#endif
