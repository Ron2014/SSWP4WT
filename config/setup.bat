reg.exe Add "HKEY_CURRENT_USER\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" /v "%~dp0WallPaper.exe" /t REG_SZ /d "RUNASADMIN"
reg.exe Add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" /v "%~dp0WallPaper.exe" /t REG_SZ /d "RUNASADMIN"
schtasks /create /tn Living_Wallpaper_For_Terminal /tr %~dp0run.vbs /sc minute /mo 1
pause