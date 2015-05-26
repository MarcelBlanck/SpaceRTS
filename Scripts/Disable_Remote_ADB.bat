@Echo Off

adb.exe devices
adb.exe tcpip 5555
adb.exe connect 192.168.178.38:5555

pause
