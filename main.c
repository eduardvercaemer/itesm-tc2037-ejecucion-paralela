#include <stdio.h>
#include <stdlib.h>

#include <windows.h>

// slow function to check prime numbers
static int isPrime(unsigned long n) {
  for (unsigned long i = 2; i < n; ++i) {
    if (n % i == 0) return 0;
  }

  return 1;
}

// add all prime numbers between the given limits
static unsigned long addPrimes(unsigned long start, unsigned long end) {
  unsigned long sum = 0;
  for (; start < end; ++start) {
    if (isPrime(start)) sum += start;
  }

  return sum;
}

typedef struct TData {
  unsigned long start, end;
  unsigned long sum;
} TDATA, *PTDATA;

static void calculateSimple(unsigned long limit) {
  const unsigned long sum = addPrimes(0, limit);
  printf("calculate simple (%d): %d\n", limit, sum);
}

DWORD WINAPI threadAddPrimes(PTDATA data) {
  const unsigned long sum = addPrimes(data->start, data->end);
  data->sum = sum;
  return 0;
}

static void calculateThreaded(unsigned long limit, unsigned tCount) {
  PTDATA pDataArray = calloc(tCount, sizeof(TDATA));
  PHANDLE hThreadArray = calloc(tCount, sizeof(HANDLE));

  if (!pDataArray || !hThreadArray) ExitProcess(2);
  
  unsigned long start = 0;
  unsigned long step = limit / tCount;
  for (unsigned i = 0; i < tCount; ++i) {
    pDataArray[i].start = start;
    pDataArray[i].end = start + step;
    start += step;

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

  unsigned long total = 0;
  for (unsigned i = 0; i < tCount; ++i) {
    total += pDataArray[i].sum;
  }

  printf("calculate threaded (%dt, %d): %d\n", tCount, limit, total);

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

