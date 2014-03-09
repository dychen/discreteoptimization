/*
 *  Problem:
 *  Let C be the capacity and N be the number of objects.
 *  For all objects with (value, weight) (V_i, W_i) and inidicator X_i,
 *  We want to maximize:
 *    sum(V_i * X_i) over all i
 *  Subject to the constraint:
 *    sum(W_i * X_i) <= C
 *
 *  Current approach:
 *  Dynamic programming for all inputs.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 *  Find which objects were chosen in the optimal solution.
 *  Algorithm:
 *    Starting from V(N, C) and iterating backwards over n in N,
 *      If V(n, c) == V(n-1, c), the object n wasn't included in the optimal
 *      solution.
 *      Otherwise, it was included, and the previous capacity is at c - W_n.
 */
void dp_backtrace(int n, int c, int objs[], int vals[]) {
    int k = c;
    for (size_t i = n; i > 0; i--) {
        if (vals[i*(c+1) + k] != vals[(i-1)*(c+1) + k]) {
            objs[(i-1)*3 + 2] = 1;
            k -= objs[(i-1)*3 + 1];
        }
    }
}

/*
 *  Print the solution in the following format, e.g.:
 *  323 1              Solution of 323, 1 indicates solution is optimal, else 0
 *  0 1 0 0 1 1 0 0    1 indicates the item was selected, else 0
 */
void dp_print(int n, int max, int optimal, int objs[]) {
    printf("%d %d\n", max, optimal);
    for (size_t i = 0; i < n; i++) {
        if (objs[i*3 + 2] == 1)
            printf("1 ");
        else
            printf("0 ");
    }
    printf("\n");
}

/*
 *  Algorithm:
 *  Let V(i, c) be an optimal value for capacity c and objects 1..i
 *  Then, for object j, the optimal value for considering the j-th object
 *    when capacity is k, V(j, k), is:
 *    V(j, k) =
 *      V(j-1, k)                          , if k < W_j
 *      max(V(j-1, k), V(j-1, k-W_j) + V_j), otherwise
 */
void dp_knapsack(int n, int c, int objs[]) {
    int* vals;
    // Initialize 2D array of optimal values A. Order as (N+1) x (C+1) for
    // row-major contiguous access.
    // Initialize base case: The value for 0 objects of any capacity is 0.
    vals = calloc((n+1) * (c+1), sizeof(int));
    // Inductive step: For objects 1..N, for capacities 0..C, apply forumla.
    for (size_t i = 1; i <= n; i++) {
        for (size_t k = 0; k <= c; k++) {
            // Note: When accessing the objs array, all object indices will be
            //       offset by 1 (indices are 0..n-1 instead of 1..n).
            int v = objs[(i-1)*3];
            int w = objs[(i-1)*3 + 1];
            if (k < w)
                vals[i*(c+1) + k] = vals[(i-1)*(c+1) + k];
            else
                vals[i*(c+1) + k] = vals[(i-1)*(c+1) + k] >= vals[(i-1)*(c+1) + (k-w)] + v ?
                                vals[(i-1)*(c+1) + k] : vals[(i-1)*(c+1) + (k-w)] + v;
        }
    }
    dp_backtrace(n, c, objs, vals);
    dp_print(n, vals[n*(c+1) + c], 1, objs);
    free(vals);
}

int main(int argc, char* argv[]) {
    FILE* fp;
    char* filename;
    int n, capacity;
    int* objs;

    if (argc == 2) {
        filename = argv[1];

        // Load file into a N x 3 array.
        //   objs[i][0] = object value V_i
        //   objs[i][1] = object weight W_i
        //   objs[i][2] = object indicator X_i (selected or not)
        fp = fopen(filename, "rb");
        if (fp == NULL) {
            perror("Could not open file.");
            return -1;
        }
        fscanf(fp, "%d %d\n", &n, &capacity);
        // calloc because we want to initialize all indicators to 0
        objs = calloc(n * 3, sizeof(int));
        for (size_t i = 0; i < n; i++)
            fscanf(fp, "%d %d\n", &objs[i*3], &objs[i*3 + 1]);
        fclose(fp);

        // Solve the problem
        dp_knapsack(n, capacity, objs);
        free(objs);
    }
}
