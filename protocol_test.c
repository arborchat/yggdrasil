// for lseek
#include <sys/types.h>
#include <unistd.h>

// for mkstemp
#include <stdlib.h>

// for NULL
#include <stddef.h>

// for remove
#include <stdio.h>

// for strlen
#include <string.h>

#include "cutest-1.5/CuTest.h"
#include "protocol.h"

#define YGG_WELCOME_TEXT "{\"Type\":0,\"Root\":\"abf07434-230c-4a69-5602-c3bdb6870954\",\"Recent\":[\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"],\"Major\":0,\"Minor\":1}"
const char *welcome_message_text = YGG_WELCOME_TEXT "\n";
const char *welcome_message_text_multi = YGG_WELCOME_TEXT "\n" YGG_WELCOME_TEXT "\n";

FILE *prepare_tempfile(CuTest *tc, const char * file_data, size_t data_size) {
        char filename[] = "ygg_test_data-XXXXXX";
	int fd = mkstemp(filename);
	if (remove(filename) == -1) {
    		CuFail(tc, "Unable to remove tempfile from file system");
    		return NULL;
	}
	if (fd == -1) {
    		CuFail(tc, "Unable to open test data file");
    		return NULL;
	}
	if (write(fd, file_data, data_size) != (signed) data_size) {
    		CuFail(tc, "Failed to write test data into temporary file");
    		return NULL;
	}
	lseek(fd, 0, SEEK_SET);
	return fdopen(fd, "r");
}

void test_read_line(CuTest *tc) {
        FILE *tempfile = prepare_tempfile(tc, welcome_message_text, strlen(welcome_message_text));
	size_t num_read = -1;
	char *message = read_line(tempfile, &num_read);
	CuAssertStrEquals(tc, YGG_WELCOME_TEXT, message);
	CuAssertIntEquals(tc, strlen(YGG_WELCOME_TEXT), num_read);
	fclose(tempfile);
}

void test_read_line_multi(CuTest *tc) {
        FILE *tempfile = prepare_tempfile(tc, welcome_message_text_multi, strlen(welcome_message_text_multi));
	size_t num_read = -1;
	char *message = read_line(tempfile, &num_read);
	CuAssertStrEquals(tc, YGG_WELCOME_TEXT, message);
	CuAssertIntEquals(tc, strlen(YGG_WELCOME_TEXT), num_read);
	free(message);
	message = read_line(tempfile, &num_read);
	CuAssertStrEquals(tc, YGG_WELCOME_TEXT, message);
	CuAssertIntEquals(tc, strlen(YGG_WELCOME_TEXT), num_read);
	fclose(tempfile);
}

void test_read_line_EOF(CuTest *tc) {
        FILE *tempfile = prepare_tempfile(tc, YGG_WELCOME_TEXT, strlen(YGG_WELCOME_TEXT));
	size_t num_read = -1;
	char *message = read_line(tempfile, &num_read);
	CuAssertStrEquals(tc, "", message);
	CuAssertIntEquals(tc, 0, num_read);
	fclose(tempfile);
}

void test_read_line_long(CuTest *tc) {
        const size_t buflen = 1536;
        char long_string[buflen];
        memset(long_string, 0, buflen);
        unsigned int iterations = (buflen / strlen(YGG_WELCOME_TEXT));
        for (unsigned int i = 0; i < iterations; i++) {
            strncat(long_string, YGG_WELCOME_TEXT, buflen-2);
        }
        long_string[strlen(long_string)] = '\n';
        FILE *tempfile = prepare_tempfile(tc, long_string, strlen(long_string));
        long_string[strlen(long_string)-1] = '\0';
	size_t num_read = -1;
	char *message = read_line(tempfile, &num_read);
	CuAssertStrEquals(tc, long_string, message);
	CuAssertIntEquals(tc, strlen(long_string), num_read);
	fclose(tempfile);
}

void test_read_line_too_long(CuTest *tc) {
        const size_t buflen = 70000;
        char long_string[buflen];
        memset(long_string, 'a', buflen);
        long_string[buflen-2] = '\n';
        long_string[buflen-1] = '\0';
        FILE *tempfile = prepare_tempfile(tc, long_string, strlen(long_string));
	size_t num_read = -1;
	char *message = read_line(tempfile, &num_read);
	CuAssertStrEquals(tc, "", message);
	CuAssertIntEquals(tc, 0, num_read);
	fclose(tempfile);
}

#define TEST_ROOT "abf07434-230c-4a69-5602-c3bdb6870954"
#define TEST_MAJOR 0
#define TEST_MINOR 1
#define TEST_TIMESTAMP 2147483647
#define TEST_MSG_ID "aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa"
#define TEST_NULL_MSG_ID ""
#define TEST_CONTENT "content"
#define TEST_USERNAME "Yggdrasil"
#define NO_RECENT_VALID_WELCOME "{\"Type\":%d,\"Major\":%d,\"Minor\":%d,\"Root\":\"%s\",\"Recent\":[]}" 
#define ONE_RECENT_VALID_WELCOME "{\"Type\":%d,\"Major\":%d,\"Minor\":%d,\"Root\":\"%s\",\"Recent\":[\"%s\"]}" 
#define VALID_NEW_MESSAGE "{\"Type\":%d,\"UUID\":\"%s\",\"Parent\":\"%s\",\"Content\":\"%s\",\"Username\":\"%s\",\"Timestamp\":%d}"
#define TEST_MSG_MAX_SIZE 1024

void test_parse_welcome_nullrecent(CuTest *tc) {
    char testmsg[TEST_MSG_MAX_SIZE];
    memset(testmsg, 0, TEST_MSG_MAX_SIZE);
    sprintf(testmsg, ONE_RECENT_VALID_WELCOME, ARBOR_WELCOME, TEST_MAJOR, TEST_MINOR, TEST_ROOT, TEST_NULL_MSG_ID);
    arbor_msg_t msg;
    memset(&msg, 0, sizeof(arbor_msg_t));
    CuAssertTrue(tc, parse_arbor_message(testmsg, &msg));
    CuAssertIntEquals(tc, ARBOR_WELCOME, msg.type);
    CuAssertStrEquals(tc, TEST_ROOT, msg.root);
    CuAssertIntEquals(tc, TEST_MAJOR, msg.major);
    CuAssertIntEquals(tc, TEST_MINOR, msg.minor);
    CuAssertTrue(tc, msg.recent_len == 1);
    CuAssertPtrNotNull(tc, msg.recent);
    for (unsigned int i = 0; i < msg.recent_len; i++) {
        CuAssertStrEquals(tc, TEST_NULL_MSG_ID, msg.recent[i]);
    }
}

void test_parse_welcome(CuTest *tc) {
    char testmsg[TEST_MSG_MAX_SIZE];
    memset(testmsg, 0, TEST_MSG_MAX_SIZE);
    sprintf(testmsg, ONE_RECENT_VALID_WELCOME, ARBOR_WELCOME, TEST_MAJOR, TEST_MINOR, TEST_ROOT, TEST_MSG_ID);
    arbor_msg_t msg;
    memset(&msg, 0, sizeof(arbor_msg_t));
    CuAssertTrue(tc, parse_arbor_message(testmsg, &msg));
    CuAssertIntEquals(tc, ARBOR_WELCOME, msg.type);
    CuAssertStrEquals(tc, TEST_ROOT, msg.root);
    CuAssertIntEquals(tc, TEST_MAJOR, msg.major);
    CuAssertIntEquals(tc, TEST_MINOR, msg.minor);
    CuAssertTrue(tc, msg.recent_len == 1);
    CuAssertPtrNotNull(tc, msg.recent);
    for (unsigned int i = 0; i < msg.recent_len; i++) {
        CuAssertStrEquals(tc, TEST_MSG_ID, msg.recent[i]);
    }
}

void test_parse_welcome_norecent(CuTest *tc) {
    char testmsg[TEST_MSG_MAX_SIZE];
    memset(testmsg, 0, TEST_MSG_MAX_SIZE);
    sprintf(testmsg, NO_RECENT_VALID_WELCOME, ARBOR_WELCOME, TEST_MAJOR, TEST_MINOR, TEST_ROOT);
    arbor_msg_t msg;
    memset(&msg, 0, sizeof(arbor_msg_t));
    CuAssertTrue(tc, parse_arbor_message(testmsg, &msg));
    CuAssertIntEquals(tc, ARBOR_WELCOME, msg.type);
    CuAssertStrEquals(tc, TEST_ROOT, msg.root);
    CuAssertIntEquals(tc, TEST_MAJOR, msg.major);
    CuAssertIntEquals(tc, TEST_MINOR, msg.minor);
    CuAssertTrue(tc, msg.recent_len == 0);
    CuAssertPtrNotNull(tc, msg.recent);
}

void test_parse_invalid_json(CuTest *tc) {
    const char *testmsg = NO_RECENT_VALID_WELCOME;
    arbor_msg_t msg;
    memset(&msg, 0, sizeof(arbor_msg_t));
    CuAssertTrue(tc, !parse_arbor_message(testmsg, &msg));
    CuAssertIntEquals(tc, 0, msg.type);
    CuAssertStrEquals(tc, NULL, msg.root);
    CuAssertIntEquals(tc, 0, msg.major);
    CuAssertIntEquals(tc, 0, msg.minor);
    CuAssertTrue(tc, msg.recent_len == 0);
    CuAssertTrue(tc, msg.recent == NULL);
}

void test_parse_new(CuTest *tc) {
    char testmsg[TEST_MSG_MAX_SIZE];
    memset(testmsg, 0, TEST_MSG_MAX_SIZE);
    sprintf(testmsg, VALID_NEW_MESSAGE, ARBOR_NEW, TEST_MSG_ID, TEST_ROOT, TEST_CONTENT, TEST_USERNAME, TEST_TIMESTAMP);
    arbor_msg_t msg;
    memset(&msg, 0, sizeof(arbor_msg_t));
    CuAssertTrue(tc, parse_arbor_message(testmsg, &msg));
    CuAssertIntEquals(tc, ARBOR_NEW, msg.type);
    CuAssertStrEquals(tc, TEST_ROOT, msg.parent);
    CuAssertStrEquals(tc, TEST_MSG_ID, msg.uuid);
    CuAssertStrEquals(tc, TEST_USERNAME, msg.username);
    CuAssertStrEquals(tc, TEST_CONTENT, msg.content);
    CuAssertIntEquals(tc, TEST_TIMESTAMP, msg.timestamp);
}


void test_write_new(CuTest *tc) {
    arbor_msg_t msg;
    memset(&msg, 0, sizeof(arbor_msg_t));
    msg.type=ARBOR_NEW;
    msg.timestamp=TEST_TIMESTAMP;
    msg.uuid=TEST_MSG_ID;
    msg.parent=TEST_ROOT;
    msg.username=TEST_USERNAME;
    msg.content=TEST_CONTENT;
    size_t written = 0;
    char *encoded = write_message(&msg, &written);
    CuAssertPtrNotNull(tc, strstr(encoded, "\"Type\":2"));
    CuAssertPtrNotNull(tc, strstr(encoded, "\"Timestamp\":2147483647"));
    CuAssertPtrNotNull(tc, strstr(encoded, "\"UUID\":" TEST_MSG_ID));
    CuAssertPtrNotNull(tc, strstr(encoded, "\"Parent\":" TEST_ROOT));
    CuAssertPtrNotNull(tc, strstr(encoded, "\"Username\":" TEST_USERNAME));
    CuAssertPtrNotNull(tc, strstr(encoded, "\"Type\":" TEST_CONTENT));
}

CuSuite* ygg_get_protocol_suite() {
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_read_line);
    SUITE_ADD_TEST(suite, test_read_line_EOF);
    SUITE_ADD_TEST(suite, test_read_line_multi);
    SUITE_ADD_TEST(suite, test_read_line_long);
    SUITE_ADD_TEST(suite, test_parse_welcome);
    SUITE_ADD_TEST(suite, test_parse_welcome_norecent);
    SUITE_ADD_TEST(suite, test_parse_welcome_nullrecent);
    SUITE_ADD_TEST(suite, test_parse_invalid_json);
    SUITE_ADD_TEST(suite, test_parse_new);
    SUITE_ADD_TEST(suite, test_write_new);
    return suite;
}
