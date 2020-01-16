#include <buggy.h>
#include <thread>

void test() {
  buggy::initialize();

  double* a = (double*)buggy::malloc(sizeof(double) * 128);
  double* b = (double*)buggy::malloc(sizeof(double) * 100);

  buggy::free(a);

  //buggy::finalize();
}

int main() {
  std::thread t1(test);
  std::thread t2(test);
  t1.join();
  t2.join();

  return 0;
}
