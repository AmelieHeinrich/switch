@echo off
::
:: $Notice: Xander Studios @ 2024
:: $Author: Amélie Heinrich
:: $Create Time: 2024-10-22 22:11:54
::

if not exist romfs (
    mkdir romfs
)

cp -r assets romfs/assets
