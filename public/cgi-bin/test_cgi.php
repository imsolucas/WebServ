#!/usr/bin/env php
<?php
// test_cgi.php: Handles GET and POST requests via CGI.
// - For GET: prints method and query string.
// - For POST: reads and prints the raw POST body.

// Print the CGI header manually
echo "Content-Type: text/plain\r\n\r\n";

$method = $_SERVER["REQUEST_METHOD"];

if ($method === "GET") 
{
    echo "Received GET\n";
    echo "Query: " . ($_SERVER["QUERY_STRING"] ?? "") . "\n";
} 
elseif ($method === "POST") 
{
	$stdin = fopen('php://stdin', 'r');
    $body = stream_get_contents($stdin);
    echo "Received POST\n";
    echo "Data: $body\n";
}
?>
