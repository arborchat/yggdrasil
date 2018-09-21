#include "cutest-1.5/CuTest.h"

void test_read_line(CuTest *tc) {
}

CuSuite* ygg_get_suite() {
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_read_line);
    return suite;
}
