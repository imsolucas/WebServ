# include "Http.h"

using std::map;
using std::pair;
using std::string;

// Status Text
const pair<StatusCode, string> statusTextArray[] =
{
	pair<StatusCode, string>(OK, "OK"),
	pair<StatusCode, string>(CREATED, "Created"),
	pair<StatusCode, string>(NO_CONTENT, "No Content"),
	pair<StatusCode, string>(MOVED_PERMANENTLY, "Moved Permanently"),
	pair<StatusCode, string>(BAD_REQUEST, "Bad Request"),
	pair<StatusCode, string>(UNAUTHORIZED, "Unauthorized"),
	pair<StatusCode, string>(FORBIDDEN, "Forbidden"),
	pair<StatusCode, string>(NOT_FOUND, "Not Found"),
	pair<StatusCode, string>(METHOD_NOT_ALLOWED, "Method Not Allowed"),
	pair<StatusCode, string>(CONTENT_TOO_LARGE, "Content Too Large"),
	pair<StatusCode, string>(INTERNAL_SERVER_ERROR, "Internal Server Error"),
	pair<StatusCode, string>(BAD_GATEWAY, "Bad Gateway")
};

const map<StatusCode, string> Http::statusText(statusTextArray, statusTextArray + sizeof(statusTextArray)/sizeof(statusTextArray[0]));

// Content Type
const pair<string, string> mimeTypeArray[] =
{
	pair<string, string>(".html", "text/html"),
	pair<string, string>(".htm", "text/html"),
	pair<string, string>(".css", "text/css"),
	pair<string, string>(".css", "text/css"),
	pair<string, string>(".js", "application/javascript"),
	pair<string, string>(".json", "application/json"),
	pair<string, string>(".xml", "application/xml"),
	pair<string, string>(".jpg", "image/jpeg"),
	pair<string, string>(".jpeg", "image/jpeg"),
	pair<string, string>(".png", "image/png"),
	pair<string, string>(".gif", "image/gif"),
	pair<string, string>(".svg", "image/svg+xml"),
	pair<string, string>(".ico", "image/x-icon"),
	pair<string, string>(".txt", "text/plain"),
	pair<string, string>(".pdf", "application/pdf"),
	pair<string, string>(".zip", "application/zip"),
	pair<string, string>(".tar", "application/x-tar"),
	pair<string, string>(".mp4", "video/mp4"),
	pair<string, string>(".mp3", "audio/mpeg"),
	pair<string, string>(".wav", "audio/wav"),
	pair<string, string>(".ogg", "audio/ogg"),
	pair<string, string>(".bin", "application/octet-stream")
};

const map<string, string> Http::mimeType(mimeTypeArray, mimeTypeArray + sizeof(mimeTypeArray)/sizeof(mimeTypeArray[0]));
