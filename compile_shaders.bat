@echo off
::
:: $Notice: Xander Studios @ 2024
:: $Author: Amélie Heinrich
:: $Create Time: 2024-10-21 14:32:46
::

set rootDir=%cd%

if not exist romfs (
    mkdir romfs
)

pushd romfs
if not exist shaders (
    mkdir shaders
)

pushd shaders
uam %rootDir%/shaders/tri_vsh.glsl --stage=vert --out=tri_vsh.dksh
uam %rootDir%/shaders/tri_fsh.glsl --stage=frag --out=tri_fsh.dksh
popd

popd
