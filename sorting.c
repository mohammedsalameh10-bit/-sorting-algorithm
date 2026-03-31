/*
 * sorting.c
 * Experimental Comparison of Sorting Algorithms
 * Algorithms: Bubble Sort, Selection Sort, Insertion Sort,
 *             Merge Sort, Quick Sort, Heap Sort, Counting Sort, Shell Sort
 *
 * Compile: gcc -O2 -o sorting sorting.c -lm
 * Usage:   ./sorting
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* ─────────────────────────────────────────
   Utility helpers
───────────────────────────────────────── */
typedef long long ll;

/* wall-clock in seconds (double precision) */
static double now_sec(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static int *clone_array(const int *src, int n)
{
    int *dst = malloc(n * sizeof(int));
    memcpy(dst, src, n * sizeof(int));
    return dst;
}

static void swap(int *a, int *b)
{
    int t = *a; *a = *b; *b = t;
}

/* ─────────────────────────────────────────
   1. Bubble Sort  O(n²)
───────────────────────────────────────── */
void bubble_sort(int *a, int n)
{
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++)
            if (a[j] > a[j+1]) { swap(&a[j], &a[j+1]); swapped = 1; }
        if (!swapped) break;   /* early exit for sorted input */
    }
}

/* ─────────────────────────────────────────
   2. Selection Sort  O(n²)
───────────────────────────────────────── */
void selection_sort(int *a, int n)
{
    for (int i = 0; i < n - 1; i++) {
        int min = i;
        for (int j = i+1; j < n; j++)
            if (a[j] < a[min]) min = j;
        swap(&a[i], &a[min]);
    }
}

/* ─────────────────────────────────────────
   3. Insertion Sort  O(n²) / O(n) best
───────────────────────────────────────── */
void insertion_sort(int *a, int n)
{
    for (int i = 1; i < n; i++) {
        int key = a[i], j = i - 1;
        while (j >= 0 && a[j] > key) { a[j+1] = a[j]; j--; }
        a[j+1] = key;
    }
}

/* ─────────────────────────────────────────
   4. Shell Sort  O(n log²n) avg
───────────────────────────────────────── */
void shell_sort(int *a, int n)
{
    for (int gap = n/2; gap > 0; gap /= 2)
        for (int i = gap; i < n; i++) {
            int temp = a[i], j = i;
            while (j >= gap && a[j-gap] > temp) { a[j] = a[j-gap]; j -= gap; }
            a[j] = temp;
        }
}

/* ─────────────────────────────────────────
   5. Merge Sort  O(n log n)
───────────────────────────────────────── */
static void merge(int *a, int l, int m, int r)
{
    int n1 = m-l+1, n2 = r-m;
    int *L = malloc(n1*sizeof(int)), *R = malloc(n2*sizeof(int));
    memcpy(L, a+l, n1*sizeof(int));
    memcpy(R, a+m+1, n2*sizeof(int));
    int i=0, j=0, k=l;
    while (i<n1 && j<n2) a[k++] = (L[i]<=R[j]) ? L[i++] : R[j++];
    while (i<n1) a[k++] = L[i++];
    while (j<n2) a[k++] = R[j++];
    free(L); free(R);
}
static void msort(int *a, int l, int r)
{
    if (l < r) { int m=(l+r)/2; msort(a,l,m); msort(a,m+1,r); merge(a,l,m,r); }
}
void merge_sort(int *a, int n) { if (n>1) msort(a,0,n-1); }

/* ─────────────────────────────────────────
   6. Quick Sort  O(n log n) avg, O(n²) worst
───────────────────────────────────────── */
static int median3(int *a, int l, int r)
{
    int m = l + (r-l)/2;
    if (a[l]>a[m]) swap(&a[l],&a[m]);
    if (a[l]>a[r]) swap(&a[l],&a[r]);
    if (a[m]>a[r]) swap(&a[m],&a[r]);
    swap(&a[m], &a[r-1]);
    return a[r-1];
}
static void qsort_helper(int *a, int l, int r)
{
    if (r - l < 10) { /* insertion sort for small sub-arrays */
        for (int i=l+1; i<=r; i++) {
            int key=a[i], j=i-1;
            while (j>=l && a[j]>key) { a[j+1]=a[j]; j--; }
            a[j+1]=key;
        }
        return;
    }
    int pivot = median3(a,l,r);
    int i=l, j=r-1;
    while (1) {
        while (a[++i]<pivot);
        while (a[--j]>pivot);
        if (i>=j) break;
        swap(&a[i],&a[j]);
    }
    swap(&a[i],&a[r-1]);
    qsort_helper(a,l,i-1);
    qsort_helper(a,i+1,r);
}
void quick_sort(int *a, int n)
{
    if (n > 1) qsort_helper(a,0,n-1);
}

/* ─────────────────────────────────────────
   7. Heap Sort  O(n log n)
───────────────────────────────────────── */
static void heapify(int *a, int n, int i)
{
    int largest=i, l=2*i+1, r=2*i+2;
    if (l<n && a[l]>a[largest]) largest=l;
    if (r<n && a[r]>a[largest]) largest=r;
    if (largest!=i) { swap(&a[i],&a[largest]); heapify(a,n,largest); }
}
void heap_sort(int *a, int n)
{
    for (int i=n/2-1; i>=0; i--) heapify(a,n,i);
    for (int i=n-1; i>0; i--) { swap(&a[0],&a[i]); heapify(a,i,0); }
}

/* ─────────────────────────────────────────
   8. Counting Sort  O(n+k)  [integers ≥0]
───────────────────────────────────────── */
void counting_sort(int *a, int n)
{
    if (n<=0) return;
    int mn=a[0], mx=a[0];
    for (int i=1;i<n;i++) { if(a[i]<mn) mn=a[i]; if(a[i]>mx) mx=a[i]; }
    int range = mx-mn+1;
    int *cnt = calloc(range, sizeof(int));
    for (int i=0;i<n;i++) cnt[a[i]-mn]++;
    int idx=0;
    for (int i=0;i<range;i++) while(cnt[i]--) a[idx++]=i+mn;
    free(cnt);
}

/* ─────────────────────────────────────────
   Data generators
───────────────────────────────────────── */
void gen_random(int *a, int n)
{
    for (int i=0;i<n;i++) a[i]=rand()%1000000;
}
void gen_sorted(int *a, int n)
{
    for (int i=0;i<n;i++) a[i]=i;
}
void gen_reverse(int *a, int n)
{
    for (int i=0;i<n;i++) a[i]=n-i;
}
void gen_almost(int *a, int n)  /* 2% elements out of place */
{
    gen_sorted(a,n);
    int swaps = (int)(n*0.02)+1;
    for (int i=0;i<swaps;i++) {
        int p=rand()%n, q=rand()%n;
        swap(&a[p],&a[q]);
    }
}
void gen_half(int *a, int n)    /* first half sorted, second half random */
{
    for (int i=0;i<n/2;i++) a[i]=i;
    for (int i=n/2;i<n;i++) a[i]=rand()%1000000;
}
void gen_flat(int *a, int n)    /* only 5 distinct values */
{
    for (int i=0;i<n;i++) a[i]=rand()%5;
}

/* ─────────────────────────────────────────
   Benchmark runner
───────────────────────────────────────── */
#define N_ALGOS   8
#define N_STRUCTS 6

typedef void (*SortFn)(int*,int);

typedef struct { const char *name; SortFn fn; int skip_large; } Algo;
typedef struct { const char *name; void (*gen)(int*,int); } DataGen;

static Algo algos[N_ALGOS] = {
    {"BubbleSort",    bubble_sort,    1},
    {"SelectionSort", selection_sort, 1},
    {"InsertionSort", insertion_sort, 1},
    {"ShellSort",     shell_sort,     0},
    {"MergeSort",     merge_sort,     0},
    {"QuickSort",     quick_sort,     0},
    {"HeapSort",      heap_sort,      0},
    {"CountingSort",  counting_sort,  0},
};

static DataGen gens[N_STRUCTS] = {
    {"Random",       gen_random},
    {"Sorted",       gen_sorted},
    {"Reverse",      gen_reverse},
    {"AlmostSorted", gen_almost},
    {"HalfSorted",   gen_half},
    {"Flat",         gen_flat},
};

/* For small n we repeat and average */
static double run_benchmark(SortFn fn, void(*gen)(int*,int), int n)
{
    int reps = 1;
    if      (n<=50)   reps=5000;
    else if (n<=100)  reps=1000;
    else if (n<=1000) reps=100;
    else if (n<=10000)reps=10;
    else              reps=3;

    double total=0.0;
    for (int r=0;r<reps;r++) {
        int *a = malloc(n*sizeof(int));
        gen(a,n);
        double t0=now_sec();
        fn(a,n);
        total += now_sec()-t0;
        free(a);
    }
    return total/reps;
}

int main(void)
{
    srand(42);

    /* sizes: small -> large */
    int sizes[]  = {20,30,50,100,500,1000,5000,10000,50000,100000,500000,1000000};
    int nsizes   = sizeof(sizes)/sizeof(sizes[0]);

    FILE *fp = fopen("results.csv", "w");
    if (!fp) { perror("fopen"); return 1; }

    /* CSV header */
    fprintf(fp,"Algorithm,DataStructure,Size,TimeSeconds\n");

    printf("%-18s %-14s %10s  %14s\n","Algorithm","Structure","Size","Time(s)");
    printf("%-18s %-14s %10s  %14s\n","----------","----------","----","-------");

    for (int ai=0; ai<N_ALGOS; ai++) {
        for (int gi=0; gi<N_STRUCTS; gi++) {
            for (int si=0; si<nsizes; si++) {
                int n = sizes[si];

                /* skip O(n²) algos for large inputs to avoid hour-long runs */
                if (algos[ai].skip_large && n > 10000) continue;

                double t = run_benchmark(algos[ai].fn, gens[gi].gen, n);

                printf("%-18s %-14s %10d  %.8f\n",
                       algos[ai].name, gens[gi].name, n, t);
                fprintf(fp,"%s,%s,%d,%.9f\n",
                        algos[ai].name, gens[gi].name, n, t);
                fflush(fp);
            }
        }
    }

    fclose(fp);
    printf("\nResults saved to results.csv\n");
    return 0;
}
