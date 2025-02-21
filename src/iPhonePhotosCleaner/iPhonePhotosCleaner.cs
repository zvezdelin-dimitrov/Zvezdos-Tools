public static class iPhonePhotosCleaner
{
    public static void Clean(string directoryPath, string[] photoExtensions, string[] livePhotoVideoExtensions)
    {
        var files = new DirectoryInfo(directoryPath).GetFiles();
        var photoFiles = files.Where(f => photoExtensions.Contains(f.Extension, StringComparer.OrdinalIgnoreCase));
        var photoNames = new HashSet<string>(photoFiles.Select(f => Path.GetFileNameWithoutExtension(f.Name)), StringComparer.OrdinalIgnoreCase);
        var livePhotoFiles = files.Where(f => livePhotoVideoExtensions.Contains(f.Extension, StringComparer.OrdinalIgnoreCase));        

        // Delete live photos
        DeleteFiles(livePhotoFiles.Where(f => photoNames.Contains(Path.GetFileNameWithoutExtension(f.Name))));

        // Delete original versions of edited photos
        DeleteFiles(photoFiles.Where(f => photoNames.Contains($"IMG_E{Path.GetFileNameWithoutExtension(f.Name).Replace("IMG_", string.Empty, StringComparison.OrdinalIgnoreCase)}")));

        // Delete edit history
        DeleteFiles(files.Where(f => f.Extension.Equals(".AAE", StringComparison.OrdinalIgnoreCase)));
    }

    private static void DeleteFiles(IEnumerable<FileInfo> files)
    {
        foreach (var file in files)
        {
            try
            {
                file.Delete();
            }
            catch
            {
            }
        }
    }
}
