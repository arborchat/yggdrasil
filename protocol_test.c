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

CuSuite* ygg_get_protocol_suite() {
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_read_line);
    SUITE_ADD_TEST(suite, test_read_line_EOF);
    SUITE_ADD_TEST(suite, test_read_line_multi);
    return suite;
}
