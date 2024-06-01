# FileTransporter
This is an extension for the Windows File Explorer.

# Functionality
- **Backup** - Creates a direct copy of the file(s) / folder(s) that are selected.
- **MoveTo** - Moves the file(s) / folder(s) to the folder that was selected
    - **..** - Moves the file(s) / folder(s) back a directory
    - **PinnedFolders** - User customizable directories for quick access
    - **Choose...** - Opens the Windows directory selector to choose any location quickly
    - **FoldersInCurrentDirectory** - Lists all the folders in the current directory that are valid to move to.
- **Pin / Unpin** - When selecting just one directory, an option to pin / unpin will appear. These are saved.

# Usage
The FileTransporter.dll needs to be registered with Windows.
1. Open an Administrator PowerShell.
2. Enter the command `Regsvr32.exe <PathToDll>` Example: `Regsvr32.exe "C:/FileTransporter/x64/Debug/FileTransporter.dll"`.<br>
A success message should be printed in a message box.

To unregister the FileTransporter.dll in case you want to remove it.
1. Open an Administrator PowerShell
2. Enter the command `Regsvr32.exe /u <PathToDll>` Example: `Regsvr32.exe /u "C:/FileTransporter/x64/Debug/FileTransporter.dll"`<br>
A success message should be printed in a message box.

# Building
1. Open the FileTransporter.sln
2. Build the Project. "Ctrl + B" for default Visual Studio
3. Output is located at `ProjectDir/{Architecture}/{Configuration}/FileTransporter.dll` Example: `C:/FileTransporter/x64/Debug/FileTransporter.dll`

#Image(s)
![alt text](https://github.com/xenontrioxide/FileTransporter/blob/master/images/SampleImage.png)
