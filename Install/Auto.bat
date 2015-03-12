@echo off
if not exist %~dp0bin mkdir %~dp0bin
copy C:\Windows\System32\format.com %~dp0bin\format.exe