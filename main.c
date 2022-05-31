#include <stdio.h>
#include <stdlib.h>

#include <windows.h>

typedef struct TData {
  int val1, val2;
} TDATA, *PTDATA;

DWORD WINAPI myProc(PTDATA data) {
  printf("hello from thread %d (val: %d)\n", data->val1, data->val2);
  return 0;
}

#define T_COUNT 10

int main() {
  PTDATA pDataArray[T_COUNT];
  DWORD dwThreadIdArray[T_COUNT];
  HANDLE hThreadArray[T_COUNT];

  for (int i = 0; i < T_COUNT; ++i) {
    pDataArray[i] = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(TDATA));
    if (!pDataArray[i]) ExitProcess(2);

    pDataArray[i]->val1 = i;
    pDataArray[i]->val2 = i + 100;

    hThreadArray[i] = CreateThread(
        NULL,
        0,
        myProc,
        pDataArray[i],
        0,
        &dwThreadIdArray[i]);
    if (!hThreadArray[i]) ExitProcess(3);
  }

  WaitForMultipleObjects(T_COUNT, hThreadArray, TRUE, INFINITE);

  for (int i = 0; i < T_COUNT; ++i) {
    CloseHandle(hThreadArray[i]);
    if (pDataArray[i]) {
      HeapFree(GetProcessHeap(), 0, pDataArray[i]);
      pDataArray[i] = NULL;
    }
  }

  return EXIT_SUCCESS;
}

