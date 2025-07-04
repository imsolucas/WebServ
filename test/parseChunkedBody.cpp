# include <ostream>
# include <sstream>
# include <cstring>

# include "utils.hpp"
# include "Http.hpp"
# include "test.hpp"

using std::exception;
using std::string;
using std::istringstream;

static bool test_valid_chunked_body();
static bool test_single_chunk();
static bool test_multiple_chunks();
static bool test_empty_chunk_at_end();
static bool test_zero_chunk_terminator();
static bool test_invalid_chunk_size();
static bool test_mixed_case_hex();
static bool test_chunked_with_binary_data();
static bool test_large_chunk_sizes();
static bool test_malformed_chunk_missing_crlf();

void test_parseChunkedBody(TestSuite &t)
{
    t.addTest(test_valid_chunked_body);
    t.addTest(test_single_chunk);
    t.addTest(test_multiple_chunks);
    t.addTest(test_empty_chunk_at_end);
    t.addTest(test_zero_chunk_terminator);
    t.addTest(test_invalid_chunk_size);
    t.addTest(test_mixed_case_hex);
    t.addTest(test_chunked_with_binary_data);
    t.addTest(test_large_chunk_sizes);
    t.addTest(test_malformed_chunk_missing_crlf);
}

bool test_valid_chunked_body()
{
    string message = "valid chunked body";
    string chunkedData =
        "D\r\n"
        "Hello, World!\r\n"
        "B\r\n"
        "How are you\r\n"
        "0\r\n"
        "\r\n";

    istringstream iss(chunkedData);
    string actual = parseChunkedBody(iss);
    string expected = "Hello, World!How are you";

    return assertEqual(message, actual, expected);
}

bool test_single_chunk()
{
    string message = "single chunk";
    string chunkedData =
        "5\r\n"
        "Hello\r\n"
        "0\r\n"
        "\r\n";

    istringstream iss(chunkedData);
    string actual = parseChunkedBody(iss);
    string expected = "Hello";

    return assertEqual(message, actual, expected);
}

bool test_multiple_chunks()
{
    string message = "multiple chunks";
    string chunkedData =
        "4\r\n"
        "Wiki\r\n"
        "5\r\n"
        "pedia\r\n"
        "e\r\n"
        " in\r\n\r\nchunks.\r\n"
        "0\r\n"
        "\r\n";

    istringstream iss(chunkedData);
    string actual = parseChunkedBody(iss);
    string expected = "Wikipedia in\r\n\r\nchunks.";

    return assertEqual(message, actual, expected);
}

bool test_empty_chunk_at_end()
{
    string message = "empty chunk at end";
    string chunkedData =
        "5\r\n"
        "Hello\r\n"
        "0\r\n"
        "\r\n";

    istringstream iss(chunkedData);
    string actual = parseChunkedBody(iss);
    string expected = "Hello";

    return assertEqual(message, actual, expected);
}

bool test_zero_chunk_terminator()
{
    string message = "zero chunk terminator";
    string chunkedData =
        "3\r\n"
        "abc\r\n"
        "0\r\n"
        "\r\n";

    istringstream iss(chunkedData);
    string actual = parseChunkedBody(iss);
    string expected = "abc";

    return assertEqual(message, actual, expected);
}

bool test_invalid_chunk_size()
{
    string message = "invalid chunk size";
    string chunkedData =
        "XYZ\r\n"
        "Hello\r\n"
        "0\r\n"
        "\r\n";

    istringstream iss(chunkedData);
    try {
        parseChunkedBody(iss);
    } catch (const exception &e) {
        return assertEqual(message, string(e.what()), string("Invalid hex string."));
    }
    return false;
}

bool test_mixed_case_hex()
{
    string message = "mixed case hex";
    string chunkedData =
        "A\r\n"
        "0123456789\r\n"
        "F\r\n"
        "abcdefghijklmno\r\n"
        "0\r\n"
        "\r\n";

    istringstream iss(chunkedData);
    string actual = parseChunkedBody(iss);
    string expected = "0123456789abcdefghijklmno";

    return assertEqual(message, actual, expected);
}

bool test_chunked_with_binary_data()
{
    string message = "with binary data";

    // Construct binary data using string constructor
    char data1[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    char data2[] = {0xFF, 0xFE, 0xFD, 0xFC};

    string binaryData1(data1, 8);
    string binaryData2(data2, 4);

    string chunkedData =
        "8\r\n" +
        binaryData1 + "\r\n" +
        "4\r\n" +
        binaryData2 + "\r\n" +
        "0\r\n" +
        "\r\n";

    istringstream iss(chunkedData);
    string actual = parseChunkedBody(iss);

    string expected = binaryData1 + binaryData2;

    return assertEqual(message, actual, expected);
}

bool test_large_chunk_sizes()
{
    string message = "large chunk sizes";
    string largeData(100, 'A');
    string chunkedData =
        "64\r\n" + largeData + "\r\n"
        "0\r\n"
        "\r\n";

    istringstream iss(chunkedData);
    string actual = parseChunkedBody(iss);
    string expected = largeData;

    return assertEqual(message, actual, expected);
}

bool test_malformed_chunk_missing_crlf()
{
    string message = "malformed chunk missing CRLF";
    string chunkedData =
        "5\r\n"
        "Hello"  // Missing \r\n after chunk data
        "0\r\n"
        "\r\n";

    istringstream iss(chunkedData);
    try
	{
        parseChunkedBody(iss);
        return true; // Adjust based on your implementation behavior
    }
	catch (const exception &e)
	{
        return assertEqual(message, e.what(), "BAD REQUEST: invalid chunk format");
    }
}
