#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <windows.h>

// slow function to check prime numbers
static int isPrime(unsigned long long n) {
  if (n <= 1) return 0;
  if (n == 2) return 1;
  if (n % 2 == 0) return 0;
  unsigned long end = sqrt(n);

  for (unsigned long i = 3; i <= end; i += 2) {
    if (n % i == 0) return 0;
  }

  return 1;
}

// add all prime numbers between the given limits
static unsigned long long addPrimes(unsigned long long start, unsigned long long end) {
  unsigned long long sum = 0;
  for (; start < end; ++start) {
    if (isPrime(start)) sum += start;
  }

  return sum;
}

typedef struct TData {
  unsigned long long start, end;
  unsigned long long sum;
} TDATA, *PTDATA;

static void calculateSimple(unsigned long long limit) {
  const unsigned long long sum = addPrimes(0, limit);
  printf("calculate simple (%lld): %lld\n", limit, sum);
}

DWORD WINAPI threadAddPrimes(PTDATA data) {
  const unsigned long long sum = addPrimes(data->start, data->end);
  data->sum = sum;
  return 0;
}

static void calculateThreaded(unsigned long long limit, unsigned tCount) {
  PTDATA pDataArray = calloc(tCount, sizeof(TDATA));
  PHANDLE hThreadArray = calloc(tCount, sizeof(HANDLE));

  if (!pDataArray || !hThreadArray) ExitProcess(2);
  
  unsigned long long start = 0;
  unsigned long long step = limit / tCount;
  for (unsigned i = 0; i < tCount; ++i) {
    pDataArray[i].start = start;
    unsigned long long end = start + step;
    if (i + 1 == tCount) end = limit;
    pDataArray[i].end = end;
    start = end;

    hThreadArray[i] = CreateThread(
        NULL,
        0,
        threadAddPrimes,
        &pDataArray[i],
        0,
        NULL);

    if (!hThreadArray[i]) ExitProcess(3);
  }

  WaitForMultipleObjects(tCount, hThreadArray, TRUE, INFINITE);

  unsigned long long total = 0;
  for (unsigned i = 0; i < tCount; ++i) {
    total += pDataArray[i].sum;
  }

  printf("calculate threaded (%dt, %lld): %lld\n", tCount, limit, total);

  for (unsigned i = 0; i < tCount; ++i) {
    CloseHandle(hThreadArray[i]);
  }

  free(hThreadArray);
  free(pDataArray);
}

int main(int argc, char **argv) {
  unsigned long long limit;
  unsigned tCount;

  LARGE_INTEGER frequency;
  LARGE_INTEGER start;
  LARGE_INTEGER end;
  double interval;

  const char *mode = argv[1];
  if (strcmp(mode, "simple") == 0) {
    limit = strtoull(argv[2], NULL, 10);

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    calculateSimple(limit);
    QueryPerformanceCounter(&end);
    interval = (double) (end.QuadPart - start.QuadPart) / frequency.QuadPart;
    printf("time: %f\n", interval);
  } else if (strcmp(mode, "threaded") == 0) {
    limit = strtoull(argv[2], NULL, 10);
    tCount = strtoul(argv[3], NULL, 10);

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    calculateThreaded(limit, tCount);
    QueryPerformanceCounter(&end);
    interval = (double) (end.QuadPart - start.QuadPart) / frequency.QuadPart;
    printf("time: %f\n", interval);
  } else {
    ExitProcess(5);
  }

  return EXIT_SUCCESS;
}

