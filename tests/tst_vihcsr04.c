#include "unity.h"
#include "unity_fixture.h"
#include "vihcsr04.h"
#include "stdio.h"

TEST_GROUP(TST_VIHCSR04);

TEST_GROUP_RUNNER(TST_VIHCSR04) {
  RUN_TEST_CASE(TST_VIHCSR04, VIHCSR04_Init);
}

TEST_SETUP(TST_VIHCSR04) {
}

TEST_TEAR_DOWN(TST_VIHCSR04) {
}

TEST(TST_VIHCSR04, VIHCSR04_Init)
{
  printf("Test: VIHCSR04_Init");
  TEST_ASSERT_EQUAL(0, VIHCSR04_Init());
}