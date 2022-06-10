#include <iostream>
#include <math.h>
#include <omp.h>

#include <windows.h>

using std::cout;
using std::min;

using T = signed long long;

int isPrime (const T n) {
  if (n <= 1) return 0;
  if (n == 2) return 1;
  if (n % 2 == 0) return 0;
  for (T i = 3, end = sqrt(n); i <= end; i += 2) {
    if (n % i == 0) return 0;
  }
  return 1;
}

unsigned long long sumPrimes(const T start, const T end) {
  T sum = 0;

  #pragma omp parallel for reduction(+:sum) num_threads(16)
  for (T i = start; i < end; ++i) {
    if (isPrime(i)) sum += i;
  }

  return sum;
}

int main () {
  const T limit = 5000000;
  LARGE_INTEGER frequency;
  LARGE_INTEGER start;
  LARGE_INTEGER end;

  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&start);
  const T sum = sumPrimes(0, limit);
  QueryPerformanceCounter(&end);
  double interval = (double) (end.QuadPart - start.QuadPart) / frequency.QuadPart;

  cout << "sum (" << limit << "): " << sum << "\n"
       << "time: " << interval << "s\n";

  return 0;
}

