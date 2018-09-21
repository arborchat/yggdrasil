// for lseek
#include <sys/types.h>
#include <unistd.h>

// for mkstemp
#include <stdlib.h>

// for NULL
#include <stddef.h>

// for remove
#include <stdio.h>

#include "cutest-1.5/CuTest.h"
#include "protocol.h"

#define YGG_WELCOME_TEXT "{\"Type\":0,\"Root\":\"abf07434-230c-4a69-5602-c3bdb6870954\",\"Recent\":[\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"],\"Major\":0,\"Minor\":1}"
const char *welcome_message_text = YGG_WELCOME_TEXT "\n";
const char *welcome_message_text_multi = YGG_WELCOME_TEXT "\n" YGG_WELCOME_TEXT "\n";

void test_read_line(CuTest *tc) {
        char filename[] = "ygg_test_data-XXXXXX";
	int fd = mkstemp(filename);
	if (remove(filename) == -1) {
    		CuFail(tc, "Unable to remove tempfile from file system");
	}
	if (fd == -1) {
    		CuFail(tc, "Unable to open test data file");
	}
	if (write(fd, welcome_message_text, strlen(welcome_message_text)) != strlen(welcome_message_text)) {
    		CuFail(tc, "Failed to write test data into temporary file");
	}
	lseek(fd, 0, SEEK_SET);
	int num_read = -1;
	char *message = read_line(fd, &num_read);
	CuAssertStrEquals(tc, YGG_WELCOME_TEXT, message);
	CuAssertIntEquals(tc, strlen(YGG_WELCOME_TEXT), num_read);
	close(fd);
}

void test_read_line_multi(CuTest *tc) {
        char filename[] = "ygg_test_data-XXXXXX";
	int fd = mkstemp(filename);
	if (remove(filename) == -1) {
    		CuFail(tc, "Unable to remove tempfile from file system");
	}
	if (fd == -1) {
    		CuFail(tc, "Unable to open test data file");
	}
	if (write(fd, welcome_message_text_multi, strlen(welcome_message_text_multi)) != strlen(welcome_message_text_multi)) {
    		CuFail(tc, "Failed to write test data into temporary file");
	}
	lseek(fd, 0, SEEK_SET);
	int num_read = -1;
	char *message = read_line(fd, &num_read);
	CuAssertStrEquals(tc, YGG_WELCOME_TEXT, message);
	CuAssertIntEquals(tc, strlen(YGG_WELCOME_TEXT), num_read);
	free(message);
	message = read_line(fd, &num_read);
	CuAssertStrEquals(tc, YGG_WELCOME_TEXT, message);
	CuAssertIntEquals(tc, strlen(YGG_WELCOME_TEXT), num_read);
	close(fd);
}

void test_read_line_EOF(CuTest *tc) {
        char filename[] = "ygg_test_data-XXXXXX";
	int fd = mkstemp(filename);
	if (remove(filename) == -1) {
    		CuFail(tc, "Unable to remove tempfile from file system");
	}
	if (fd == -1) {
    		CuFail(tc, "Unable to open test data file");
	}
	if (write(fd, YGG_WELCOME_TEXT, strlen(YGG_WELCOME_TEXT)) != strlen(YGG_WELCOME_TEXT)) {
    		CuFail(tc, "Failed to write test data into temporary file");
	}
	lseek(fd, 0, SEEK_SET);
	int num_read = -1;
	char *message = read_line(fd, &num_read);
	CuAssertStrEquals(tc, "", message);
	CuAssertIntEquals(tc, 0, num_read);
	close(fd);
}

CuSuite* ygg_get_protocol_suite() {
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_read_line);
    SUITE_ADD_TEST(suite, test_read_line_EOF);
    SUITE_ADD_TEST(suite, test_read_line_multi);
    return suite;
}