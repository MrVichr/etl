#include "DelegateTester.hpp"
#include <string.h>

#include <array>


int cb_freefunc(int a, int b)
{
  return a+b;
}

struct TesterMini
{
  int insb;
  int operator()(int a, int b) { return insb+a+b; }
  int testfunc(int a, int b) { return insb+a+b; }
  int testcfunc(int a, int b) const { return insb+a+b; }
} testerMini;

DelegateTester::DelegateTester()
{
  clearstack();
  lambda_copy_p1();
  lambda_ref_p1();
  not_equal();
  test_align();
  test_owning();
  test_set();
  test_constexpr();

  setup();
  clearstack();
  test();
  clearstack();
  finale();

  gnuc=__GNUC__;
  //c14=ETL_CONSTEXPR14;
}

void DelegateTester::clearstack()
{
  char data[2048];
  memset(data, 0xbb, sizeof(data));
}

void DelegateTester::lambda_copy_p1()
{
  int loca=1, locb=2;
  int res;

  lambda_copy=[loca, locb](int a, int b)mutable {loca+=a; locb+=b; return loca+locb;};
  res=lambda_copy(3, 4);
  assert(res==1+3+2+4);
  res=lambda_copy(5, 6);
  assert(res==1+3+5+2+4+6);
  assert(loca==1);
  assert(locb==2);
}

void DelegateTester::lambda_ref_p1()
{
  int loca=1, locb=2, res;

  insa=loca;
  insb=locb;
  auto lamb=[&insa=this->insa, &insb=this->insb](int a, int b) {insa+=a; insb+=b; return insa+insb;};
  assert(!lambda_ref);
  lambda_ref=lamb;
  assert(lambda_ref);
  res=lambda_ref(3, 4);
  assert(res==1+3+2+4);
  res=lambda_ref(5, 6);
  assert(res==1+3+5+2+4+6);
  assert(insa==1+3+5);
  assert(insb==2+4+6);
}

void DelegateTester::not_equal()
{
  auto lamb1a=[a=5, b=6] (int p) mutable { a=p; };
  /*auto lamb1b=lamb1a;
  assert(lamb1a==lamb1b);
  lamb1b(7);
  assert(lamb1a==lamb1b);*/
  etl::delegate<void(int)> d1(lamb1a);
  etl::delegate<void(int)> d2(d1);
  assert(d1==d2);
  /* lambdas don't support ==, so why should we
  d1(10);
  assert(d1==d2); fails */
  d2=[a=5, b=6] (int p) mutable { a=p; };
  assert(d1!=d2);

  auto lamb2=[a=1] (int p) mutable { a=p; };
  d1=[a=5, b=6] (int p) mutable { a=p; };
  d2=[a=7, b=8] (int p) mutable { a=p; };
  d1=lamb2;
  d1(20);
  d2=lamb2;
  d2(20);//30);
  assert(d1==d2);
}

void DelegateTester::test_align()
{
  char x1='A';
  uint32_t x4=40;
  double x8=2.718;
  struct A16
  {
    alignas(16) double data=9.806;
  } x16;
  //alignas(32) double x32=42.0;

  etl::delegate<void(void)> t1=[x1] () mutable { x1++; };
  t1();
  assert(alignof(t1)>=alignof(x1));

  etl::delegate<void(void)> t4=[x4] () mutable { x4++; };
  t4();
  assert(alignof(t4)>=alignof(x4));

  etl::delegate<void(void)> t8=[x8] () mutable { x8+=1; };
  t8();
  assert(alignof(t8)>=alignof(x8));

  auto l16=[x16] () mutable { x16.data+=1; };
  //etl::delegate<void(void), 8, 8> t16=l16; //-> static_assert(..., "Insufficient alignment of delegate")
  etl::delegate<void(void), 8, 16> t16=l16;
  t16();
  size_t sl16=sizeof(l16);
  size_t sx16=sizeof(x16);
  size_t al16=alignof(l16);
  size_t at16=alignof(t16);
  size_t ax16=alignof(x16);
  assert(alignof(t16)>=alignof(x16));

  /*etl::delegate<void(void)> t32=[alignas(32) x32] () mutable { x32+=1; };
  t32();
  size_t at32=alignof(t32);
  size_t ax32=alignof(x32);
  assert(alignof(t32)>=alignof(x32));*/
}

void DelegateTester::test_owning()
{
  static int total_ctr_calls=0;
  static int total_dtr_calls=0;
  class TestClass
  {
  public:
    int data;
    bool ctr_called;
    bool assign_called;
    bool dtr_called;
    bool func_called;

    TestClass(): data(5), ctr_called(false), assign_called(false), dtr_called(false), func_called(false)
    {
      ctr_called=true;
      total_ctr_calls++;
    }
    TestClass(const TestClass &rhs): ctr_called(false), assign_called(false), dtr_called(false), func_called(false)
    {
      data=rhs.data+10;
      assign_called=true;
      total_ctr_calls++;
    }
    TestClass &operator=(const TestClass &rhs)
    {
      data=rhs.data+1;
      assign_called=true;
      return *this;
    }
    ~TestClass() { dtr_called=true; total_dtr_calls++; }
    TestClass *func()
    {
      func_called=true;
      return this;
    }
  };

  TestClass *peek=nullptr;
  TestClass *peek2=nullptr;
  {
    TestClass testClass;
    etl::delegate<TestClass *(void)> d1([tc=testClass] () mutable { return tc.func(); });
    peek=d1();
    peek2=&testClass;
    assert(peek->data==25);
  }
  assert(total_ctr_calls==total_dtr_calls);
  (void)peek;
  (void)peek2;
  peek=nullptr;
}

void DelegateTester::test_set()
{
  int res;
  etl::delegate<int(int,int)> d;

  d.set<cb_freefunc>();
  res=d(8,9);
  assert(res==8+9);

  int loca=12;
  d.set([&loca](int a, int b) { return loca+a+b; });
  res=d(2,3);
  assert(res==12+2+3);

  d.set<TesterMini, &TesterMini::testfunc>(testerMini);
  testerMini.insb=13;
  res=d(3,4);
  assert(res==13+3+4);

  d.set<TesterMini, &TesterMini::testcfunc>(testerMini);
  testerMini.insb=14;
  res=d(4,5);
  assert(res==14+4+5);

  d.set<TesterMini, testerMini, &TesterMini::testfunc>();
  testerMini.insb=15;
  res=d(5,6);
  assert(res==15+5+6);

  d.set<TesterMini, testerMini, &TesterMini::testcfunc>();
  testerMini.insb=15;
  res=d(5,6);
  assert(res==15+5+6);
}

void DelegateTester::test_constexpr()
{
#if ETL_DELEGATE_SUPPORTS_CONSTEXPR
  int res;

  constexpr auto dff = etl::delegate<int(int,int)>::create<cb_freefunc>();
  res=dff(8,9);
  assert(res==8+9);

  /* lambdas need some more work
  int loca=12;
  constexpr auto dl = etl::delegate<int(int,int)>::create([&loca](int a, int b) { return loca+a+b; });
  res=dl(2,3);
  assert(res==12+2+3);/* */

  constexpr auto dimr = etl::delegate<int(int,int)>::create<TesterMini, &TesterMini::testfunc>(testerMini);
  testerMini.insb=18;
  res=dimr(3,4);
  assert(res==18+3+4);

  constexpr auto dcimr = etl::delegate<int(int,int)>::create<TesterMini, &TesterMini::testcfunc>(testerMini);
  testerMini.insb=19;
  res=dcimr(4,5);
  assert(res==19+4+5);

  constexpr auto dimc = etl::delegate<int(int,int)>::create<TesterMini, testerMini, &TesterMini::testfunc>();
  testerMini.insb=20;
  res=dimc(5,6);
  assert(res==20+5+6);

  constexpr auto dcimc = etl::delegate<int(int,int)>::create<TesterMini, testerMini, &TesterMini::testcfunc>();
  testerMini.insb=21;
  res=dcimc(5,6);
  assert(res==21+5+6);
#endif
}

void DelegateTester::setup()
{
  int loca=1, locb=2, locc=3, locd=4, loce=5;
  int res;
  auto lamb=[&insa=this->insa, &insb=this->insb](int a, int b) {insa+=a; insb+=b; return insa+insb;};

  lambda_ref=lamb;
  etl::delegate<int(int,int), 1*8> d2=lamb;
  assert(d2==lambda_ref);
  etl::delegate<int(int,int), 1*8> d3(lambda_copy);
  assert(d3==lambda_copy);
  assert(d3!=lambda_ref);
  etl::delegate<int(int,int), 1*8> d4=etl::delegate<int(int,int), 1*8>::create(lamb);
  assert(d4==lambda_ref);
  assert(d4!=lambda_copy);
  //etl::delegate<int(int,int), 0*8> d5(lambda_copy); //should fail
  //assert(d5==lambda_copy);
  etl::delegate<int(int,int), 2*8> d7(lambda_copy);
  assert(d7==lambda_copy);
  assert(lambda_copy==d7);
  assert(d7!=lambda_ref);
  assert(lambda_ref!=d7);
  //etl::delegate<int(int,int), 0*8> d9;
  //d9=lambda_copy; //should fail
  //assert(d9==lambda_copy);
  etl::delegate<int(int,int), 2*8> d11;
  d11=lambda_copy;
  assert(d11==lambda_copy);
  assert(lambda_copy==d11);
  assert(d11!=lambda_ref);
  assert(lambda_ref!=d11);
  //etl::delegate<int(int,int), 1*8, 4> d6(lambda_copy); //should fail
  //assert(d6==lambda_copy);
  etl::delegate<int(int,int), 1*8, 16> d8(lambda_copy);
  assert(d8==lambda_copy);
  assert(lambda_copy==d8);
  assert(d8!=lambda_ref);
  assert(lambda_ref!=d8);
  //etl::delegate<int(int,int), 1*8, 4> d10;
  //d10=lambda_copy; //should fail
  //assert(d10==lambda_copy);
  etl::delegate<int(int,int), 1*8, 16> d12;
  d12=lambda_copy;
  assert(d12==lambda_copy);
  assert(lambda_copy==d12);
  assert(d12!=lambda_ref);
  assert(lambda_ref!=d12);

  etl::delegate<int(int,int), 1*8> ctr_test(lamb);
  res=ctr_test(0, 0);
  assert(res==21);

  /*res=-10;
  lambda_big=[&loca, &locb, &locc, &locd, &loce, &res](int a, int b) { return loca+locb+locc+locd+loce+res+a+b; };
  res=lambda_big(1,2);
  assert(res==9+12+3+4+5-10+3);*/

  loca=9; locb=12;
  lambda_big=[&loca, &locb, &locc, &locd, &loce](int a, int b) { return loca+locb+locc+locd+loce+a+b; };
  res=lambda_big(1,2);
  assert(res==9+12+3+4+5+3);

  //not implemented by OP freefunc=cb_freefunc;

  freefunc=etl::delegate<int(int,int)>::create<cb_freefunc>();
  res=freefunc(6,7);
  assert(res==6+7);

  staticmethod=etl::delegate<int(int,int)>::create<&DelegateTester::cb_staticmethod>();
  res=staticmethod(9,91);
  assert(res==100);

  runtimemethod=etl::delegate<int(int,int)>::create<DelegateTester, &DelegateTester::cb_method>(*this);
  insa=10;
  res=runtimemethod(3,4);
  assert(res==17);

  runtimecmethod=etl::delegate<int(int,int)>::create<TesterMini, &TesterMini::testcfunc>(testerMini);
  testerMini.insb=12;
  res=runtimecmethod(3,4);
  assert(res==19);

  testerMini.insb=15;
  fixedmethod=etl::delegate<int(int,int)>::create<TesterMini, testerMini, &TesterMini::testfunc>();
  res=fixedmethod(4,5);
  assert(res==15+4+5);

  //ETL_COMPILER_GCC && __GNUC__
  testerMini.insb=20;
  functor=etl::delegate<int(int,int)>::create<TesterMini, testerMini>();
  res=functor(5, 6);
  assert(res==20+5+6);

  testerMini.insb=19;
  etl::delegate<int(int,int), 1*8> d100=etl::delegate<int(int,int), 1*8>::create<TesterMini, testerMini, &TesterMini::testcfunc>();
  res=d100(5, 6);
  assert(res==19+5+6);

  etl::delegate<void(int,int), 1*8> d101;
  assert(!d101.call_if(3,4));
  d101=[&locd, &loce](int a, int b) { locd=555; loce=a+b; };
  assert(d101.call_if(3,4));
  assert(locd==555);
  assert(loce==7);


  loca=0; locb=0; locc=0; locd=0; loce=0;
  long_delegate=[loca, locb, locc, locd, loce](int x) mutable {
    loca=locb;
    locb=locc;
    locc=locd;
    locd=loce;
    loce=x;
    return loca+locb+locc+locd+loce;
  };
}

void DelegateTester::test()
{
  int res;

  insa=1;
  insb=2;
  res=lambda_ref(3, 4);
  assert(res==1+3+2+4);
  res=lambda_ref(5, 6);
  assert(res==1+3+5+2+4+6);
  assert(insa==1+3+5);
  assert(insb==2+4+6);

  //not implemented by OP freefunc=cb_freefunc;

  res=freefunc(6,7);
  assert(res==6+7);

  res=staticmethod(9,91);
  assert(res==100);

  insa=10;
  res=runtimemethod(3,4);
  assert(res==17);

  testerMini.insb=13;
  res=runtimecmethod(3,4);
  assert(res==20);

  testerMini.insb=25;
  res=fixedmethod(4,5);
  assert(res==25+4+5);

  testerMini.insb=30;
  res=functor(5, 6);
  assert(res==30+5+6);

  testerMini.insb=31;
  auto ores=functor.call_if(6, 7);
  assert(ores.has_value());
  assert(ores.value()==44);

  testerMini.insb=32;
  res=functor.call_or([](int a, int b) {return 222+a+b;}, 7, 8);
  assert(res==47);
  res=functor.call_or<cb_freefunc>(9, 10);
  assert(res==32+19);

  functor=etl::delegate<int(int,int)>();
  testerMini.insb=33;
  ores=functor.call_if(6, 7);
  assert(!ores.has_value());

  testerMini.insb=34;
  res=functor.call_or([](int a, int b) {return 222+a+b;}, 7, 8);
  assert(res==222+7+8);
  res=functor.call_or<cb_freefunc>(9, 10);
  assert(res==19);

  long_delegate(10);
  long_delegate(11);
  long_delegate(12);
  long_delegate(13);
}

void DelegateTester::finale()
{
  int res=long_delegate(14);
  assert(res==10+11+12+13+14);
}
