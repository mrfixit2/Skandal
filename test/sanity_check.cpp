/*
 * This is an example unit test that doesn't really do anything useful.
 * It is here as a reference for you when creating additional unit tests.
 * For additional reference information, see the "test.h" header.
 */
 
 #include "test.h"
 
 TEST(sanity_check) {
    CHECK(true);
    CHECK_EQUAL(2,1+1);
    
    int x[] = {1,2,3};
    int y[] = {1,2,3};
    CHECK_ARRAY_EQUAL(x,y,3);
    
    double a = 1.51;
    double b = 1.52;
    CHECK_CLOSE(a,b,0.1); 
}
