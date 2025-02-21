if (args.Length >= 2)
{
    var directoryPath = args[0];
    Enum.TryParse(args[1], out Tool selectedTool);

    switch (selectedTool)
    {
        case Tool.CleanIPhonePhotos:
            iPhonePhotosCleaner.Clean(directoryPath, [".HEIC"], [".MP4"]);
            return;
    }
}
