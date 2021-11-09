# Luna
Window module using Win32. Currently dependent on Pluto for Input managing and GaiaX or Terra for monitor's co-ordinate. Both of these can be replaced with similar Input and Graphics modules implementing the required interface.

# Dependencies
1. [Pluto](https://github.com/razerx100/Pluto).
2. [GaiaX](https://github.com/razerx100/GaiaX) or [Terra](https://github.com/razerx100/Terra).

# Third party library Used
ImGui

## Instructions
Run the appropriate Setup script to configure the project with or without ImGui. Use the ones with VK suffix for using Vulkan renderer or Dx12 suffix for Directx12 renderer. The setup scripts use the ***Visual Studio 17 2022*** generator for project generation. But ***Visual Studio 16 2019*** generator should work as well.

## Requirements
cmake 3.21+.\
C++20 Standard supported Compiler.
