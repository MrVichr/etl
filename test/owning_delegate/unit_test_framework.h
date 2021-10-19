#ifndef ETL_UNIT_TEST_FRAMEWORK_INCLUDED
#define ETL_UNIT_TEST_FRAMEWORK_INCLUDED

#define SUITE(x) namespace
#define TEST_FIXTURE(clas, func) void clas::func()
#define TEST(func) void SetupFixture::func()
#define CHECK(x) assert(x)
#define CHECK_THROW(call, exc) \
  { bool bleflemle=false; try { call; } catch (exc &barnabas) { bleflemle=true; }; assert(bleflemle); }
#define CHECK_NO_THROW(call) \
  { bool bleflemle=false; try { call; bleflemle=true; } catch (...) { }; assert(bleflemle); }
#define CHECK_EQUAL(why1, why2) assert(why1==why2)

#endif
