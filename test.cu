#include <buggy.h>
#include <thread>

void test(buggy::allocator& buggy_alloc) {
  double* a = (double*)buggy_alloc.malloc(sizeof(double) * 128);
  double* b = (double*)buggy_alloc.malloc(sizeof(double) * 100);

  buggy_alloc.free(a);
  buggy_alloc.free(b);
}

int main() {
  buggy::allocator buggy_alloc;

  std::thread t1(test, std::ref(buggy_alloc));
  std::thread t2(test, std::ref(buggy_alloc));
  t1.join();
  t2.join();

  return 0;
}
