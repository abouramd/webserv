<?php
// Connect to the database
$conn = mysqli_connect("localhost", "root", "", "my_db");

// Check connection
if (!$conn) {
  die("Connection failed: " . mysqli_connect_error());
}

// Verify username and password
$username = $_POST['username'];
$password = $_POST['password'];

$sql = "SELECT * FROM users WHERE username='$username' AND password='$password'";
$result = mysqli_query($conn, $sql);

if (mysqli_num_rows($result) == 1) {
  // Username and password are correct, set session variables
  $_SESSION['username'] = $username;
  header("Location: dashboard.php");
  exit;
} else {
  // Username and password are incorrect, display error message
  $error = "Invalid username and password";
  header("Location: index.php?error=$error");
  exit;
}

// Close connection
mysqli_close($conn);
?>

