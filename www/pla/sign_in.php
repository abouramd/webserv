<!DOCTYPE html>
<html>
<head>
    <title>Sign In</title>
    <link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
    <h1>Sign In</h1>
    <form method="GET" action="">
        <label for="username">Username:</label>
        <input type="text" name="username" id="username" required><br>

        <label for="password">Password:</label>
        <input type="password" name="password" id="password" required><br>

        <input type="submit" value="Sign In">
    </form>
<?php
echo '<p>User inserted successfully!</p>';
// Check if the form is submitted
if ($_SERVER['REQUEST_METHOD'] === 'GET') {
    echo '<p>REQUEST_METHOD is GET!</p>';
    // Get the form data
    $username = $_GET['username'];
    $password = $_GET['password'];

    // Create a new MySQLi object
    $mysqli = new mysqli('localhost', 'root', 'root', 'mydb');

    // Check if the connection was successful
    if ($mysqli->connect_errno) {
        echo '<p>Connection failed</p>'; 
        die('Connection failed: ' . $mysqli->connect_error);
    }

    // Prepare the SQL statement
    $stmt = $mysqli->prepare("INSERT INTO users (username, password) VALUES ('$username', '$password')");

    // Bind the parameters
    $stmt->bind_param('ss', $username, $password);

    // Execute the statement
    if ($stmt->execute()) {
        echo '<p>User inserted successfully!</p>';
    } else {
        echo '<p>Error inserting user: ' . $stmt->error . '</p>';
    }
    // Close the statement and the connection
    $stmt->close();
    $mysqli->close();
}
?>
</body>
</html>
