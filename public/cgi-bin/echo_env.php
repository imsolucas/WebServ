#!/usr/bin/env php
<?php
// echo_env.php: Prints all CGI environment variables.
// Useful to verify what headers and server info are passed by the webserver.
echo "Content-Type: text/plain\r\n\r\n";

foreach ($_SERVER as $key => $value) 
{
    echo "$key=$value\n";
}
?>
