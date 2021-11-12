if not exist Solution\ (
    mkdir .\Solution
)
cd .\Solution
cmake .. -G "Visual Studio 17 2022" -A x64 -DIMGUIFLAG:STRING=DISABLE -DINPUT_MANAGER:STRING=Pluto -DGRAPHICS_ENGINE:STRING=Terra -DPLATFORM:STRING=WINDOWS