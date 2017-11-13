
PING localhost -n 2 >NUL
sc stop asmmap64
sc delete asmmap64
del "%WINDIR%\system32\drivers\asmmap64.sys"



