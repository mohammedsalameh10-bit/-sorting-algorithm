/*
 * sorting.c
 * Experimental Comparison of Sorting Algorithms
 * NOW INCLUDES: Loading unsorted numbers from file
 *
 * Compile: gcc -O2 -o sorting sorting.c -lm
 * Usage:   ./sorting
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ─────────────────────────────────────────
   Utility helpers
───────────────────────────────────────── */
typedef long long ll;

static double now_sec(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static void swap(int *a, int *b)
{
    int t = *a; *a = *b; *b = t;
}

/* ─────────────────────────────────────────
   FILE LOADING
───────────────────────────────────────── */
int *load_from_file(const char *filename, int *out_n)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen input file");
        return NULL;
    }

    int capacity = 1024;
    int *arr = malloc(capacity * sizeof(int));
    int n = 0;

    while (1) {
        if (n >= capacity) {
            capacity *= 2;
            arr = realloc(arr, capacity * sizeof(int));
        }

        if (fscanf(fp, "%d", &arr[n]) != 1)
            break;

        n++;
    }

    fclose(fp);

    *out_n = n;
    return arr;
}

/* Global storage for file data */
static int *file_data = NULL;
static int file_n = 0;

/* ─────────────────────────────────────────
   Sorting Algorithms
───────────────────────────────────────── */
void bubble_sort(int *a, int n)
{
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++)
            if (a[j] > a[j+1]) { swap(&a[j], &a[j+1]); swapped = 1; }
        if (!swapped) break;
    }
}

void selection_sort(int *a, int n)
{
    for (int i = 0; i < n - 1; i++) {
        int min = i;
        for (int j = i+1; j < n; j++)
            if (a[j] < a[min]) min = j;
        swap(&a[i], &a[min]);
    }
}

void insertion_sort(int *a, int n)
{
    for (int i = 1; i < n; i++) {
        int key = a[i], j = i - 1;
        while (j >= 0 && a[j] > key) { a[j+1] = a[j]; j--; }
        a[j+1] = key;
    }
}

void shell_sort(int *a, int n)
{
    for (int gap = n/2; gap > 0; gap /= 2)
        for (int i = gap; i < n; i++) {
            int temp = a[i], j = i;
            while (j >= gap && a[j-gap] > temp) {
                a[j] = a[j-gap];
                j -= gap;
            }
            a[j] = temp;
        }
}

static void merge(int *a, int l, int m, int r)
{
    int n1 = m-l+1, n2 = r-m;
    int *L = malloc(n1*sizeof(int)), *R = malloc(n2*sizeof(int));
    memcpy(L, a+l, n1*sizeof(int));
    memcpy(R, a+m+1, n2*sizeof(int));

    int i=0, j=0, k=l;
    while (i<n1 && j<n2)
        a[k++] = (L[i]<=R[j]) ? L[i++] : R[j++];
    while (i<n1) a[k++] = L[i++];
    while (j<n2) a[k++] = R[j++];

    free(L); free(R);
}

static void msort(int *a, int l, int r)
{
    if (l < r) {
        int m=(l+r)/2;
        msort(a,l,m);
        msort(a,m+1,r);
        merge(a,l,m,r);
    }
}

void merge_sort(int *a, int n)
{
    if (n>1) msort(a,0,n-1);
}

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
    if (r - l < 10) {
        for (int i=l+1; i<=r; i++) {
            int key=a[i], j=i-1;
            while (j>=l && a[j]>key) {
                a[j+1]=a[j];
                j--;
            }
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

static void heapify(int *a, int n, int i)
{
    int largest=i, l=2*i+1, r=2*i+2;
    if (l<n && a[l]>a[largest]) largest=l;
    if (r<n && a[r]>a[largest]) largest=r;
    if (largest!=i) {
        swap(&a[i],&a[largest]);
        heapify(a,n,largest);
    }
}

void heap_sort(int *a, int n)
{
    for (int i=n/2-1; i>=0; i--) heapify(a,n,i);
    for (int i=n-1; i>0; i--) {
        swap(&a[0],&a[i]);
        heapify(a,i,0);
    }
}

void counting_sort(int *a, int n)
{
    if (n<=0) return;

    int mn=a[0], mx=a[0];
    for (int i=1;i<n;i++) {
        if(a[i]<mn) mn=a[i];
        if(a[i]>mx) mx=a[i];
    }

    int range = mx-mn+1;
    int *cnt = calloc(range, sizeof(int));

    for (int i=0;i<n;i++) cnt[a[i]-mn]++;

    int idx=0;
    for (int i=0;i<range;i++)
        while(cnt[i]--) a[idx++]=i+mn;

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

void gen_almost(int *a, int n)
{
    gen_sorted(a,n);
    int swaps = (int)(n*0.02)+1;
    for (int i=0;i<swaps;i++) {
        int p=rand()%n, q=rand()%n;
        swap(&a[p],&a[q]);
    }
}

void gen_half(int *a, int n)
{
    for (int i=0;i<n/2;i++) a[i]=i;
    for (int i=n/2;i<n;i++) a[i]=rand()%1000000;
}

void gen_flat(int *a, int n)
{
    for (int i=0;i<n;i++) a[i]=rand()%5;
}

void gen_from_file(int *a, int n)
{
    if (!file_data || file_n == 0) return;

    for (int i = 0; i < n; i++)
        a[i] = file_data[i % file_n];
}

/* ─────────────────────────────────────────
   Benchmark
───────────────────────────────────────── */
#define N_ALGOS   8
#define N_STRUCTS 7

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
    {"FromFile",     gen_from_file},
};

static double run_benchmark(SortFn fn, void(*gen)(int*,int), int n)
{
    int reps = (n<=50)?5000:(n<=100)?1000:(n<=1000)?100:(n<=10000)?10:3;

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

    file_data = load_from_file("input.txt", &file_n);
    if (!file_data)
        printf("Warning: input.txt not found. Skipping FromFile tests.\n");

    int sizes[] = {20,30,50,100,500,1000,5000,10000,50000,100000};
    int nsizes = sizeof(sizes)/sizeof(sizes[0]);

    FILE *fp = fopen("results.csv", "w");
    fprintf(fp,"Algorithm,DataStructure,Size,TimeSeconds\n");

    for (int ai=0; ai<N_ALGOS; ai++) {
        for (int gi=0; gi<N_STRUCTS; gi++) {

            if (gens[gi].gen == gen_from_file && file_data == NULL)
                continue;

            for (int si=0; si<nsizes; si++) {
                int n = sizes[si];

                if (algos[ai].skip_large && n > 10000)
                    continue;

                double t = run_benchmark(algos[ai].fn, gens[gi].gen, n);

                printf("%-15s %-12s %8d  %.8f\n",
                       algos[ai].name, gens[gi].name, n, t);

                fprintf(fp,"%s,%s,%d,%.9f\n",
                        algos[ai].name, gens[gi].name, n, t);
            }
        }
    }

    fclose(fp);
    free(file_data);

    printf("\nResults saved to results.csv\n");
    return 0;
}
