#include <gmock-gtest-all.cc>

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::FLAGS_gtest_catch_exceptions = true;
    int ret = RUN_ALL_TESTS();
	
    if (argc > 1)
        system("pause"); // stop program and show output when run from IDE by F5

    return ret;
}
