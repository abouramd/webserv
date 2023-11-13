#!/usr/bin/perl
use CGI;

my $cgi = CGI->new();
my $path_info = $cgi->path_info;

#print $cgi->header('text/html');
print "<p>Script Path: $ENV{SCRIPT_NAME}</p>";
print "<p>Path Info: $path_info</p>";
