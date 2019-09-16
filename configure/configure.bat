@echo off

echo Setting OpenCV environment system variable.
setx OpenCV %~dp0 /m
