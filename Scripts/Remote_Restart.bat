adb.exe shell am force-stop vr.mighty.pirates.AerialExplorerVR
ping 1.1.1.1 -n 1 -w 3000 > nul
adb.exe shell am start -n vr.mighty.pirates.AerialExplorerVR/com.epicgames.ue4.GameActivity
ping 1.1.1.1 -n 1 -w 3000 > nul
