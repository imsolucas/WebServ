<?php
// invalid_bin.php: Lacks a shebang line and may not be executable as CGI.
// Used to test how the server handles invalid CGI execution.
echo "Content-Type: text/plain\r\n\r\n";

echo "This should not run.\n";
?>
