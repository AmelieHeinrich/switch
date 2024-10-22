@echo off
::
:: $Notice: Xander Studios @ 2024
:: $Author: Amélie Heinrich
:: $Create Time: 2024-10-21 14:35:25
::

echo Copying assets.
call copy_assets.bat
echo Done.

echo Compiling shaders.
call compile_shaders.bat
echo Done.

echo Compiling program.
call make
echo Done.
