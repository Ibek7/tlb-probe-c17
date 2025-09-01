#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

#ifdef __linux__
#include <sched.h>
#endif

#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/thread_policy.h>
#endif

/*
 * tlb.c
 * Simple program to measure average time per page access.
 * Usage: ./tlb <num_pages> <num_trials>
 * Prints: <num_pages>\t<avg_time_ns_per_access> to stdout
 * Prints sum to stderr to avoid compiler optimizing the loop away.
 */

static inline uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <num_pages> <num_trials>\n", argv[0]);
        return 1;
    }

    int num_pages = atoi(argv[1]);
    long num_trials = atol(argv[2]);
    if (num_pages <= 0 || num_trials <= 0) {
        fprintf(stderr, "num_pages and num_trials must be positive\n");
        return 1;
    }

    int page_size = sysconf(_SC_PAGESIZE);
    int jump = page_size / (int)sizeof(int);
    long array_size = (long)num_pages * (long)jump;

    // allocate a page-aligned buffer
    int *a = NULL;
    if (posix_memalign((void**)&a, (size_t)page_size, (size_t)array_size * sizeof(int)) != 0) {
        perror("posix_memalign");
        return 1;
    }

    // Try to pin to CPU 0 to reduce noise
#ifdef __linux__
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    if (sched_setaffinity(0, sizeof(set), &set) != 0) {
        perror("sched_setaffinity");
    }
#elif defined(__APPLE__)
    thread_affinity_policy_data_t policy = { 1 };
    thread_policy_set(mach_thread_self(), THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, 1);
#endif

    // initialize to avoid demand-zeroing costs during measurement
    for (long i = 0; i < array_size; i++) {
        a[i] = (int)(i & 0x7fffffff);
    }

    // warm-up pass
    volatile int tmp = 0;
    for (long i = 0; i < array_size; i += jump) {
        tmp += a[i];
    }

    uint64_t start = now_ns();

    unsigned long long checksum = 0;
    for (long t = 0; t < num_trials; t++) {
        for (long i = 0; i < array_size; i += jump) {
            a[i] += 1;
            checksum += (unsigned int)a[i];
        }
    }

    uint64_t end = now_ns();
    uint64_t elapsed_ns = (end - start);

    unsigned long long accesses = (unsigned long long)num_trials * (unsigned long long)num_pages;
    double avg_ns = (double)elapsed_ns / (double)accesses;

    printf("%d\t%.3f\n", num_pages, avg_ns);
    fprintf(stderr, "checksum=%llu\n", checksum);

    free(a);
    return 0;
}
