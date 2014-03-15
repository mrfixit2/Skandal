/*
 * This file includes the magic necessary in order to get your unit tests
 * that you create with UnitTest++ to automatically run. There should
 * never be a reason to modify this file. For an example unit test,
 * see "sanity_check.cpp". For a reference on creating tests, see "test.h".
 *
 */
#include "test.h"

int main(int argc, char* argv[]) {
    
    /* FIXME: executing unit tests twice for human-readable output */
    UnitTest::RunAllTests();
    
    /* creating tests xml for nice jenkins output */
    std::ofstream f("tests.xml");
    UnitTest::XmlTestReporter reporter(f);
    UnitTest::TestRunner runner(reporter);
    return runner.RunTestsIf(UnitTest::Test::GetTestList(), NULL, UnitTest::True(), 0);
}
