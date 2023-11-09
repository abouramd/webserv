<?php
// Connect to the database
$conn = mysqli_connect("localhost", "root", "root", "mydb");

// Check connection
if (!$conn) {
  die("Connection failed: " . mysqli_connect_error());
}

// Verify username and password
$username = $_GET['username'];
$password = $_GET['password'];

$sql = "SELECT * FROM users WHERE username='$username' AND password='$password';";
$result = mysqli_query($conn, $sql);

if (mysqli_num_rows($result) == 1) {
  // Username and password are correct, set session variables
  $error = "Invalid username and password";
  header("Location: index.php?error=$error");
  exit;
} else {
  // Username and password are incorrect, display error message
  $_SESSION['username'] = $username;
  header("Location: dashboard.php"); 
  exit;
}

// Close connection
mysqli_close($conn);
?>

