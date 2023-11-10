<?php
// Set the cookie
$cookie_name = "user";
$cookie_value = "John Doe";
$cookie_expiry = time() + (86400 * 30); // Cookie expires in 30 days
$cookie_path = "/"; // Cookie is available in the entire domain
$cookie_domain = ""; // Set your domain here, e.g. ".example.com" for all subdomains
$cookie_secure = false; // Set to true if using HTTPS
$cookie_httponly = true; // Cookie is only accessible through HTTP protocol

setcookie($cookie_name, $cookie_value, $cookie_expiry, $cookie_path, $cookie_domain, $cookie_secure, $cookie_httponly);

// Display a message after the cookie is set
echo "Cookie '$cookie_name' is set!";
?>

