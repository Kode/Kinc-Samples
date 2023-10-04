@git -C %~dp0 submodule update --init Kinc
@git -C %~dp0 submodule update --init --recursive RuntimeShaderCompilation/krafix
@call %~dp0\Kinc\get_dlc.bat