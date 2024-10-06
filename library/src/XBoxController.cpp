#include <XBoxController.hpp>
#include <Xinput.h>
#include <cmath>

[[nodiscard]]
float GetMagnitude(std::int16_t x, std::int16_t y) noexcept {
	return std::sqrtf(
		static_cast<float>(std::pow(x, 2.f) + std::pow(y, 2.f))
	);
}

[[nodiscard]]
float ProcessDeadZone(
	float magnitude, std::uint32_t maxValue, std::uint32_t deadZone
) noexcept {
	magnitude = std::min(magnitude, static_cast<float>(maxValue));
	magnitude -= deadZone;

	return magnitude / (maxValue - deadZone);
}

[[nodiscard]]
ThumbStickData ProcessThumbStickData(
	float magnitude, std::int16_t x, std::int16_t y, std::uint32_t deadZone
) noexcept {
	ThumbStickData data{
		.magnitude = ProcessDeadZone(magnitude, 32767u, deadZone),
		.xDirection = x / magnitude,
		.yDirection = y / magnitude
	};

	return data;
}

void CheckXBoxControllerStates(InputManager* inputManager) noexcept {
	XINPUT_STATE state{};
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	auto gamepadCount = static_cast<DWORD>(inputManager->GetGamepadCount());

	for (DWORD gamepadIndex = 0u; gamepadIndex < gamepadCount; ++gamepadIndex) {
		if (XInputGetState(gamepadIndex, &state) == ERROR_SUCCESS) {
			IGamepad& gamepad = inputManager->GetGamepad(gamepadIndex);

			const XINPUT_GAMEPAD& xData = state.Gamepad;

			gamepad.SetRawButtonState(ProcessGamepadRawButtons(xData.wButtons));

			std::uint32_t leftStickDeadZone = gamepad.GetLeftThumbStickDeadZone();
			if (float magnitude = GetMagnitude(xData.sThumbLX, xData.sThumbLY);
				magnitude > leftStickDeadZone)
				gamepad.OnLeftThumbStickMove(
					ProcessThumbStickData(
						magnitude, xData.sThumbLX, xData.sThumbLY,
						leftStickDeadZone
					)
				);

			std::uint32_t rightStickDeadZone = gamepad.GetRightThumbStickDeadZone();
			if (float magnitude = GetMagnitude(xData.sThumbRX, xData.sThumbRY);
				magnitude > rightStickDeadZone)
				gamepad.OnRightThumbStickMove(
					ProcessThumbStickData(
						magnitude, xData.sThumbRX, xData.sThumbRY,
						rightStickDeadZone
					)
				);

			std::uint32_t threshold = gamepad.GetTriggerThreshold();
			if (xData.bLeftTrigger > threshold)
				gamepad.OnLeftTriggerMove(
					ProcessDeadZone(
						static_cast<float>(xData.bLeftTrigger),
						255u,
						threshold
					)
				);

			if (xData.bRightTrigger > threshold)
				gamepad.OnRightTriggerMove(
					ProcessDeadZone(
						static_cast<float>(xData.bRightTrigger),
						255u,
						threshold
					)
				);
		}
	}
}

void DisconnectXBoxController(InputManager* inputManager) noexcept {
	XINPUT_STATE state{};
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	auto gamepadCount = static_cast<DWORD>(inputManager->GetGamepadCount());

	for (DWORD gamepadIndex = 0u; gamepadIndex < gamepadCount; ++gamepadIndex)
		if (XInputGetState(gamepadIndex, &state) == ERROR_DEVICE_NOT_CONNECTED)
			inputManager->DisconnectGamepad(gamepadIndex);
}
