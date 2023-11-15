<?php
// Print the values of the environment variables
echo "Content-Type: text/plain\n\n";
echo "AUTH_TYPE: " . $_SERVER['AUTH_TYPE'] . "\n";
echo "REDIRECT_STATUS: " . $_SERVER['REDIRECT_STATUS'] . "\n";
echo "CONTENT_LENGTH: " . $_SERVER['CONTENT_LENGTH'] . "\n";
echo "CONTENT_TYPE: " . $_SERVER['CONTENT_TYPE'] . "\n";
echo "GATEWAY_INTERFACE: " . $_SERVER['GATEWAY_INTERFACE'] . "\n";
echo "HTTP_ACCEPT: " . $_SERVER['HTTP_ACCEPT'] . "\n";
echo "HTTP_ACCEPT_CHARSET: " . $_SERVER['HTTP_ACCEPT_CHARSET'] . "\n";
echo "HTTP_ACCEPT_ENCODING: " . $_SERVER['HTTP_ACCEPT_ENCODING'] . "\n";
echo "HTTP_ACCEPT_LANGUAGE: " . $_SERVER['HTTP_ACCEPT_LANGUAGE'] . "\n";
echo "HTTP_FORWARDED: " . $_SERVER['HTTP_FORWARDED'] . "\n";
echo "HTTP_HOST: " . $_SERVER['HTTP_HOST'] . "\n";
echo "HTTP_PROXY_AUTHORIZATION: " . $_SERVER['HTTP_PROXY_AUTHORIZATION'] . "\n";
echo "HTTP_USER_AGENT: " . $_SERVER['HTTP_USER_AGENT'] . "\n";
echo "PATH_INFO: " . $_SERVER['PATH_INFO'] . "\n";
echo "PATH_TRANSLATED: " . $_SERVER['PATH_TRANSLATED'] . "\n";
echo "QUERY_STRING: " . $_SERVER['QUERY_STRING'] . "\n";
echo "REMOTE_HOST: " . $_SERVER['REMOTE_HOST'] . "\n";
echo "REMOTE_USER: " . $_SERVER['REMOTE_USER'] . "\n";
echo "REQUEST_METHOD: " . $_SERVER['REQUEST_METHOD'] . "\n";
echo "SCRIPT_NAME: " . $_SERVER['SCRIPT_NAME'] . "\n";
echo "SERVER_NAME: " . $_SERVER['SERVER_NAME'] . "\n";
echo "SERVER_PROTOCOL: " . $_SERVER['SERVER_PROTOCOL'] . "\n";
echo "SERVER_SOFTWARE: " . $_SERVER['SERVER_SOFTWARE'] . "\n";
echo "HTTP_COOKIE: " . $_SERVER['HTTP_COOKIE'] . "\n";
?>

