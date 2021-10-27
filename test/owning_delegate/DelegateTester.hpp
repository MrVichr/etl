#ifndef DELEGATETESTER_HPP
#define DELEGATETESTER_HPP

#include "etl/delegate.h"

class DelegateTester
{
  etl::delegate<int(int,int), 1*8> lambda_ref;
  etl::delegate<int(int,int), 1*8> lambda_copy;
  etl::delegate<int(int,int), 4*8> lambda_big;
  etl::delegate<int(int,int)> freefunc;
  etl::delegate<int(int,int)> staticmethod;
  etl::delegate<int(int,int)> runtimemethod;
  etl::delegate<int(int,int)> runtimecmethod;
  etl::delegate<int(int,int)> fixedmethod;
  etl::delegate<int(int,int)> functor;

  etl::delegate<int(int), 5*sizeof(int)-sizeof(void *)> long_delegate;

  int insa, insb;
  static int cb_staticmethod(int a, int b) { return a+b; }
  int cb_method(int a, int b) { return insa+a+b; }
  int operator()(int a, int b) { return insa+a+b; }
public:
  DelegateTester();
  void setup();
  void clearstack();
  void test();
  void finale();
  void lambda_copy_p1();
  void lambda_ref_p1();
  void not_equal();
  void test_align();
  void test_owning();
  void test_set();
  void test_constexpr();
  void test_wrongtype();

  int gnuc;
};

#endif // DELEGATETESTER_HPP
