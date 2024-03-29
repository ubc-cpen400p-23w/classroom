#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#define ARRSIZE 6

using namespace std;

int partition(int arr[], int start, int end) {

  int pivot = arr[start];

  int count = 0;
  for (int i = end; i > start; i--) {
    if (arr[i] <= pivot)
      count++;
  }

  int pivotIndex = start + count;
  swap(arr[pivotIndex], arr[start]);

  int i = start, j = end;

  while (i < pivotIndex && j > pivotIndex) {

    while (arr[i] <= pivot) {
      i++;
    }

    while (arr[j] > pivot) {
      j--;
    }

    if (i < pivotIndex && j > pivotIndex) {
      swap(arr[i++], arr[j++]);
    }
  }

  return pivotIndex;
}
 
void quickSort(int arr[], int start, int end) {

  if (start >= end)
      return;

  int p = partition(arr, start, end);
  quickSort(arr, start, p - 1);
  quickSort(arr, p + 1, end);
}

int main() {

  int *n = (int *)malloc(sizeof(int));

  if (n == NULL) {
    return 0;
  }

  *n = ARRSIZE;

  int *arr = (int *)calloc(*n, sizeof(int));

  memcpy(arr, (int []){16, 3, -4, 1, 1, -8}, ARRSIZE * sizeof(int));

  quickSort(arr, 0, *n - 1);

  for (int i = 0; i < *n; i++) {
      cout << arr[i] << " ";
  }

  cout << "\n";

  free(arr);
  return 0;
}

