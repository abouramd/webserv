<!DOCTYPE html>
<html>
<head>
    <title>Set and Expire Cookies</title>
    <link rel="stylesheet" type="text/css" href="style.css">
    <script src="https://code.jquery.com/jquery-3.6.4.min.js"></script>
</head>
<body>
    <div class="container">
        <h1>Cookies</h1>
        <form method="POST" action="set_cookies.php">
            <div class="form-group">
                <label for="name">Name:</label>
                <input type="text" name="name" id="name" vlaue="webserv" required>
            </div>

            <div class="form-group">
                <label for="email">Email:</label>
                <input type="email" name="email" id="email" vlaue="webserv@1337.ma" required>
            </div>

            <div class="form-group">
                <label for="age">Age:</label>
                <input type="number" name="age" id="age" required>
            </div>

            <div class="form-group">
                <label for="time">Time to expire in seconds:</label>
                <input type="number" name="time" id="time" value="30" required>
            </div>

            <input type="submit" value="Set Cookies" class="btn">
        </form>

        <div class="cookies" id="cookies-container">
            <script>
                // Function to update the content inside the cookies div
function updateCookies() {
    // Fetch the updated content from the server (cookies.php)
    $.ajax({
        url: 'show_cookies.php',
        method: 'GET',
        success: function (cookiesContent) {
            // Update the HTML element with the new content
            $('#cookies-container').html(cookiesContent);
        }
    });
}

// Update the content every second
setInterval(updateCookies, 1000);

// Initial update when the page loads
updateCookies();
</script>
        </div>

        <form method="POST" action="expire_cookies.php">
            <input type="submit" value="Expire Cookies" class="btn">
        </form>
    </div>
</body>
</html>
