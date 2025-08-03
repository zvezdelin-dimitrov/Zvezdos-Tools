# Zvezdos Tools

A suite of utilities available as both a command-line tool and a Windows Explorer shell extension.

## Projects

### 1. ConsoleTools
A .NET 9 command-line utility for running various tools. Currently, it supports cleaning iPhone photo directories by removing unnecessary files (live photo videos, original versions of edited photos, and edit history files).

**Usage:**
```
ConsoleTools.exe <directoryPath> <Tool>
```
- `<directoryPath>`: Path to the folder containing iPhone photos.
- `<Tool>`: Tool to run. Currently supported: `CleanIPhonePhotos`.

**Example:**
```
ConsoleTools.exe "C:\Users\User\Pictures\iPhone" CleanIPhonePhotos
```

### 2. iPhonePhotosCleaner
A .NET 9 class library containing the logic for cleaning iPhone photo directories. Used by `ConsoleTools` and can be referenced in other .NET projects.

### 3. ConsoleToolsShellExtensions
A native Windows Explorer shell extension (C++) that adds context menu commands (e.g., "Clean iPhone Photos") to folders. When invoked, it launches the command-line tool (`zvezdostools.exe`) with the appropriate arguments for the selected folder.
