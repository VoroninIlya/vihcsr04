#include "unity_fixture.h"
#include "vihcsr04.h"

static void runAllTests(void)
{
  RUN_TEST_GROUP(TST_VIHCSR04);
}

int main(int argc, const char* argv[])
{
  return UnityMain(argc, argv, runAllTests);
}