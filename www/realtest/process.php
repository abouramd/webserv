<?php
if (isset($_GET['name'])) {
    $name = $_GET['name'];
    echo "<h1>Hello, $name!</h1>";
} else {
    echo "Please fill out the form.";
}
?>