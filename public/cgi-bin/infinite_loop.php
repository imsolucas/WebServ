#!/usr/bin/env php
<?php
// infinite_loop.php: Simulates a hanging CGI script using an infinite loop.
// Used to test the server’s timeout or hanging connection behavior.
echo "Content-Type: text/plain\r\n\r\n";

while (true) {}
?>
