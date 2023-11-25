<?php

// YouTube video URL
if ($_SERVER['REQUEST_METHOD'] == 'POST')
{
    $videoUrl = $_POST['youtubeUrl'];
    $videoName = $_POST['youtubeName'];

    // Command to download video using youtube-dl
    $command = "wget $videoUrl";

    // Execute the command
    exec($command, $output, $return);

    // Check if the command executed successfully
    if ($return === 0) {
        echo "Video downloaded successfully!";
    } else {
        echo "Error downloading video.";
    }
}
else
{
    $html = '<!DOCTYPE html>
    <html>
    <head>
    <title>YouTube URL Form</title>
    </head>
    <body>
        <form method="post" action="youtube.php">
            <label for="youtube-url">YouTube URL:</label>
            <input type="text" id="youtube-url" name="youtubeUrl" required>
            <label for="youtube-name">name:</label>
            <input type="text" id="youtube-name" name="youtubeName" required>
            <button type="submit">Submit</button>
        </form>
    </body>
    </html>';

    echo $html;
}
?>