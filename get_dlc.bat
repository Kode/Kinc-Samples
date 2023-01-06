@git -C %~dp0 submodule update --init
@git -C %~dp0\RuntimeShaderCompilation submodule update --init --recursive
@call %~dp0\Kinc\get_dlc.bat