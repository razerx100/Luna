#ifndef XBOX_CONTROLLER_HPP_
#define XBOX_CONTROLLER_HPP_
#include <CleanWin.hpp>
#include <cstdint>
#include <InputManager.hpp>

void CheckXBoxControllerStates(InputManager* inputManager) noexcept;
void DisconnectXBoxController(InputManager* inputManager) noexcept;

[[nodiscard]]
std::uint16_t ProcessGamepadRawButtons(std::uint16_t state) noexcept;
#endif
