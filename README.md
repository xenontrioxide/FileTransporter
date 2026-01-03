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
- This project uses [CMake](https://cmake.org/) for the build system.
- This project uses [vcpkg](https://vcpkg.io/) to manage dependencies.
### [CMake GUI](https://cmake.org/download/)
1. Select this project root as the source directory.
2. Choose and select the build directory.
3. Generate & Open Project.
### CMake Commandline
1. At the root of the project `cmake -S . -B <BuildDir>`
2. Run `cmake --build <BuildDir>`

# Image(s)
![alt text](https://github.com/xenontrioxide/FileTransporter/blob/master/images/explorer_ldWLs31FaP.png)
