#include <stdio.h>
#include <stdlib.h>

/* Return a pointer to an array of two dynamically allocated arrays of ints.
   The first array contains the elements of the input array s that are
   at even indices.  The second array contains the elements of the input
   array s that are at odd indices.

   Do not allocate any more memory than necessary.
*/
int **split_array(const int *s, int length) {
  int length0, length1;

  if (length % 2 == 0) { // even; split evently
    length0 = length1 = length / 2;
  }
  else { // odd; result0 has extra
    length0 = (length / 2) + 1; // one extra
    length1 = (length / 2); // int rounds down
  }

  int *result_0 = malloc(length0 * sizeof(int));
  int *result_1 = malloc(length1 * sizeof(int)); 
  int **result = malloc(2 * sizeof(int*)); // array holding two pointers to int*

  int j = 0;
  int k = 0; // counters for result0 and result1
  for (int i = 0; i < length; i++) {
    if (i % 2 == 0) { //even
      result_0[j] = s[i];
      j++;
    }
    else {
      result_1[k] = s[i];
      k++;
    }
  }

  result[0] = result_0;
  result[1] = result_1;

  return result;
}

/* Return a pointer to an array of ints with size elements.
   - strs is an array of strings where each element is the string
     representation of an integer.
   - size is the size of the array
 */

int *build_array(char **strs, int size) {
  int *array_numbers = malloc(size * sizeof(int));
  for (int i = 1; i < size; i++) { // don't use argv[0]
    array_numbers[i-1] = strtol(strs[i], NULL, 10);
  }
  return array_numbers;  
}


int main(int argc, char **argv) {
    /* Replace the comments in the next two lines with the appropriate
       arguments.  Do not add any additional lines of code to the main
       function or make other changes.
     */
    int *full_array = build_array(argv, argc);
    int **result = split_array(full_array, argc);

    printf("Original array:\n");
    for (int i = 0; i < argc - 1; i++) {
        printf("%d ", full_array[i]);
    }
    printf("\n");

    printf("result[0]:\n");
    for (int i = 0; i < argc/2; i++) {
        printf("%d ", result[0][i]);
    }
    printf("\n");

    printf("result[1]:\n");
    for (int i = 0; i < (argc - 1)/2; i++) {
        printf("%d ", result[1][i]);
    }
    printf("\n");
    free(full_array);
    free(result[0]);
    free(result[1]);
    free(result);
    return 0;
}
