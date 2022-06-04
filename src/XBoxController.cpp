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
	ThumbStickData data = {};

	data.xDirection = x / magnitude;
	data.yDirection = y / magnitude;

	data.magnitude = ProcessDeadZone(magnitude, 32767u, deadZone);

	return data;
}

void CheckXBoxControllerStates(InputManager* inputManager) noexcept {
	XINPUT_STATE state = {};;
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	auto gamepadCount = static_cast<DWORD>(inputManager->GetGamepadCount());

	for (DWORD gamepadIndex = 0u; gamepadIndex < gamepadCount; ++gamepadIndex) {
		if (XInputGetState(gamepadIndex, &state) == ERROR_SUCCESS) {
			IGamepad* pGamepad = inputManager->GetGamepadByIndex(gamepadIndex);

			const XINPUT_GAMEPAD& xData = state.Gamepad;

			pGamepad->SetRawButtonState(ProcessGamepadRawButtons(xData.wButtons));

			std::uint32_t leftStickDeadZone =
				pGamepad->GetLeftThumbStickDeadZone();
			if (float magnitude = GetMagnitude(xData.sThumbLX, xData.sThumbLY);
				magnitude > leftStickDeadZone)
				pGamepad->OnLeftThumbStickMove(
					ProcessThumbStickData(
						magnitude, xData.sThumbLX, xData.sThumbLY,
						leftStickDeadZone
					)
				);

			std::uint32_t rightStickDeadZone =
				pGamepad->GetRightThumbStickDeadZone();
			if (float magnitude = GetMagnitude(xData.sThumbRX, xData.sThumbRY);
				magnitude > rightStickDeadZone)
				pGamepad->OnRightThumbStickMove(
					ProcessThumbStickData(
						magnitude, xData.sThumbRX, xData.sThumbRY,
						rightStickDeadZone
					)
				);

			std::uint32_t threshold = pGamepad->GetTriggerThreshold();
			if (xData.bLeftTrigger > threshold)
				pGamepad->OnLeftTriggerMove(
					ProcessDeadZone(
						static_cast<float>(xData.bLeftTrigger),
						255u,
						threshold
					)
				);

			if (xData.bRightTrigger > threshold)
				pGamepad->OnRightTriggerMove(
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
	XINPUT_STATE state = {};
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	auto gamepadCount = static_cast<DWORD>(inputManager->GetGamepadCount());

	for (DWORD gamepadIndex = 0u; gamepadIndex < gamepadCount; ++gamepadIndex)
		if (XInputGetState(gamepadIndex, &state) == ERROR_DEVICE_NOT_CONNECTED)
			inputManager->DisconnectGamepadByIndex(gamepadIndex);
}
