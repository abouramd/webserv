<?php
// Check if the form is submitted
if ($_SERVER['REQUEST_METHOD'] === 'GET') {
    // Get the form data
    $username = $_GET['username'];
    $password = $_GET['password'];

    // Create a new MySQLi object
    $mysqli = new mysqli('localhost', 'root', 'root', 'mydb');

    // Check if the connection was successful
    if ($mysqli->connect_errno) {
        die('Connection failed: ' . $mysqli->connect_error);
    }

    // Prepare the SQL statement
    $stmt = $mysqli->prepare("INSERT INTO users (username, password) VALUES ('$username . ', '$password')");

    // Bind the parameters
    $stmt->bind_param('ss', $username, $password);

    // Execute the statement
    if ($stmt->execute()) {
        echo 'User inserted successfully!';
    } else {
        echo 'Error inserting user: ' . $stmt->error;
    }

    // Close the statement and the connection
    $stmt->close();
    $mysqli->close();
}
// echo $REQUEST_METHOD 'username' 'password';
?>
