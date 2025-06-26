#!/usr/bin/env php
<?php
// crash.php: Simulates a server-side crash by throwing an exception.
// Used to test how the server handles internal errors (should return 500 or 502).
throw new Exception("Intentional server-side crash for testing purposes");
?>
