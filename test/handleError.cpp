# include "utils.hpp"
# include "ClientManager.hpp"
# include "test.hpp"

static bool test_400();
static bool test_401();
static bool test_403();
static bool test_404();
static bool test_405();
static bool test_413();
static bool test_500();
static bool test_502();

void test_handleError(TestSuite &t)
{
	t.addTest(test_400);
	t.addTest(test_401);
	t.addTest(test_403);
	t.addTest(test_404);
	t.addTest(test_405);
	t.addTest(test_413);
	t.addTest(test_500);
	t.addTest(test_502);
}

bool test_400()
{
	HttpResponse actual = ClientManager::handleError(BAD_REQUEST);

	HttpResponse expected;
	expected.protocol = "HTTP/1.1";
	expected.statusCode = BAD_REQUEST;
	expected.statusText = Http::statusText.find(BAD_REQUEST)->second;
	expected.headers[Http::CONTENT_TYPE] = "text/html";
	expected.body =
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head><title>400 Bad Request</title></head>\n"
		"<body>\n"
		"  <h1>400 Bad Request</h1>\n"
		"  <p>Your request could not be understood by the server.</p>\n"
		"</body>\n"
		"</html>\n";
	expected.headers[Http::CONTENT_LENGTH] = utils::toString(expected.body.size());

	return assertEqual("handle Bad Request error", actual, expected);
}

bool test_401()
{
	HttpResponse actual = ClientManager::handleError(UNAUTHORIZED);

	HttpResponse expected;
	expected.protocol = "HTTP/1.1";
	expected.statusCode = UNAUTHORIZED;
	expected.statusText = Http::statusText.find(UNAUTHORIZED)->second;
	expected.headers[Http::CONTENT_TYPE] = "text/html";
	expected.body =
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head><title>401 Unauthorized</title></head>\n"
		"<body>\n"
		"  <h1>401 Unauthorized</h1>\n"
		"  <p>Authentication is required to access this resource.</p>\n"
		"</body>\n"
		"</html>\n";
	expected.headers[Http::CONTENT_LENGTH] = utils::toString(expected.body.size());

	return assertEqual("handle Unauthorized error", actual, expected);
}

bool test_403()
{
	HttpResponse actual = ClientManager::handleError(FORBIDDEN);

	HttpResponse expected;
	expected.protocol = "HTTP/1.1";
	expected.statusCode = FORBIDDEN;
	expected.statusText = Http::statusText.find(FORBIDDEN)->second;
	expected.headers[Http::CONTENT_TYPE] = "text/html";
	expected.body =
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head><title>403 Forbidden</title></head>\n"
		"<body>\n"
		"  <h1>403 Forbidden</h1>\n"
		"  <p>You do not have permission to access this resource.</p>\n"
		"</body>\n"
		"</html>\n";
	expected.headers[Http::CONTENT_LENGTH] = utils::toString(expected.body.size());

	return assertEqual("handle Forbidden error", actual, expected);
}

bool test_404()
{
	HttpResponse actual = ClientManager::handleError(NOT_FOUND);

	HttpResponse expected;
	expected.protocol = "HTTP/1.1";
	expected.statusCode = NOT_FOUND;
	expected.statusText = Http::statusText.find(NOT_FOUND)->second;
	expected.headers[Http::CONTENT_TYPE] = "text/html";
	expected.body =
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head><title>404 Not Found</title></head>\n"
		"<body>\n"
		"  <h1>404 Not Found</h1>\n"
		"  <p>The requested resource could not be found.</p>\n"
		"</body>\n"
		"</html>\n";
	expected.headers[Http::CONTENT_LENGTH] = utils::toString(expected.body.size());

	return assertEqual("handle Not Found error", actual, expected);
}

bool test_405()
{
	HttpResponse actual = ClientManager::handleError(METHOD_NOT_ALLOWED);

	HttpResponse expected;
	expected.protocol = "HTTP/1.1";
	expected.statusCode = METHOD_NOT_ALLOWED;
	expected.statusText = Http::statusText.find(METHOD_NOT_ALLOWED)->second;
	expected.headers[Http::CONTENT_TYPE] = "text/html";
	expected.body =
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head><title>405 Method Not Allowed</title></head>\n"
		"<body>\n"
		"  <h1>405 Method Not Allowed</h1>\n"
		"  <p>The HTTP method used in the request is not allowed for the requested resource.</p>\n"
		"</body>\n"
		"</html>\n";
	expected.headers[Http::CONTENT_LENGTH] = utils::toString(expected.body.size());

	return assertEqual("handle Method Not error Allowed", actual, expected);
}

bool test_413()
{
	HttpResponse actual = ClientManager::handleError(CONTENT_TOO_LARGE);

	HttpResponse expected;
	expected.protocol = "HTTP/1.1";
	expected.statusCode = CONTENT_TOO_LARGE;
	expected.statusText = Http::statusText.find(CONTENT_TOO_LARGE)->second;
	expected.headers[Http::CONTENT_TYPE] = "text/html";
	expected.body =
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head><title>413 Content Too Large</title></head>\n"
		"<body>\n"
		"  <h1>413 Content Too Large</h1>\n"
		"  <p>The request body is too large and exceeds the server's limit.</p>\n"
		"</body>\n"
		"</html>\n";
	expected.headers[Http::CONTENT_LENGTH] = utils::toString(expected.body.size());

	return assertEqual("handle Content Too error Large", actual, expected);
}

bool test_500()
{
	HttpResponse actual = ClientManager::handleError(INTERNAL_SERVER_ERROR);

	HttpResponse expected;
	expected.protocol = "HTTP/1.1";
	expected.statusCode = INTERNAL_SERVER_ERROR;
	expected.statusText = Http::statusText.find(INTERNAL_SERVER_ERROR)->second;
	expected.headers[Http::CONTENT_TYPE] = "text/html";
	expected.body =
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head><title>500 Internal Server Error</title></head>\n"
		"<body>\n"
		"  <h1>500 Internal Server Error</h1>\n"
		"  <p>The server encountered an internal error and was unable to complete your request.</p>\n"
		"</body>\n"
		"</html>\n";
	expected.headers[Http::CONTENT_LENGTH] = utils::toString(expected.body.size());

	return assertEqual("handle Internal Server error Error", actual, expected);
}

bool test_502()
{
	HttpResponse actual = ClientManager::handleError(BAD_GATEWAY);

	HttpResponse expected;
	expected.protocol = "HTTP/1.1";
	expected.statusCode = BAD_GATEWAY;
	expected.statusText = Http::statusText.find(BAD_GATEWAY)->second;
	expected.headers[Http::CONTENT_TYPE] = "text/html";
	expected.body =
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head><title>502 Bad Gateway</title></head>\n"
		"<body>\n"
		"  <h1>502 Bad Gateway</h1>\n"
		"  <p>The server received an invalid response from the upstream server.</p>\n"
		"</body>\n"
		"</html>\n";
	expected.headers[Http::CONTENT_LENGTH] = utils::toString(expected.body.size());

	return assertEqual("handle Bad Gateway error", actual, expected);
}
