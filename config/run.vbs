Set ws = CreateObject("Wscript.shell") 
Set sf = CreateObject("Scripting.FileSystemObject")
ws.run "cmd.exe /C " + sf.GetFile(Wscript.ScriptFullName).ParentFolder.Path + "\run.bat",vbhide