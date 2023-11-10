<?php
function listFilesRecursively($directory) {
    $entries = scandir($directory);

    // Remove '.' and '..' from the list of entries
    $entries = array_diff($entries, array('.', '..'));

    echo '<ul>';

    foreach ($entries as $entry) {
        $fullPath = $directory . '/' . $entry;

        if (is_dir($fullPath)) {
            // If it's a directory, display it as a link and list its contents
            echo '<li><strong>Directory: ' . $entry . '/</strong>';
            listFilesRecursively($fullPath); // Recursively list contents of the subdirectory
            echo '</li>';
        } elseif (is_file($fullPath)) {
            // If it's a file, display it as a link
            echo '<li><a href="' . $fullPath . '">' . $entry . '</a></li>';
        }
    }

    echo '</ul>';
}

$rootDirectory = '.';
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>List Files and Directories in Root Directory</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <h1>List Files and Directories in Root Directory</h1>
    <?php listFilesRecursively($rootDirectory); ?>
</body>
</html>

