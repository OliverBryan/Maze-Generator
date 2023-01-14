mkdir bin
cd bin

cmake ..\

cmake --build . --config Debug
cmake --build . --config Release

move .\_deps\sfml-build\lib\Debug\*.dll .\Debug
move .\_deps\sfml-build\lib\Release\*.dll .\Release
