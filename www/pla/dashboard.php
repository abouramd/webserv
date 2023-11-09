<?php
// Check if user is logged in
if (!isset($_SESSION['username'])) {
  header("Location: index.php");
  exit;
}

// Display username and logout button
echo "Welcome, " . $_SESSION['username'] . "! <a href='logout.php'>Logout</a>";
?>

