@echo off
copy "%~dp0\asmmap64.sys" "%WINDIR%\system32\drivers\asmmap64.sys"
sc create asmmap64 binPath= system32\drivers\asmmap64.sys type= kernel
sc start asmmap64

