#!/usr/bin/env python

import cgi
import os

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from fields
name = form.getvalue('name')
email = form.getvalue('email')

# Process the form data
# ... Perform necessary actions based on the user's input

# Generate the response
print("Content-type: text/html\n")
print("<html>")
print("<head>")
print("<title>CGI Script</title>")
print('<link rel="stylesheet" type="text/css" href="style.css">')
print("</head>")
print("<body>")
print("<h1>Hello, {}!</h1>".format(name))
print("<p>Your email address is: {}</p>".format(email))
print("<p>Your method is: {}</p>".format(os.environ['REQUEST_METHOD']))
print("</body>")
print("</html>")

