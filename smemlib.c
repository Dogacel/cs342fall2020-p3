
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Define a name for your shared memory; you can give any name that start with a slash character; it will be like a filename.
#define SHM_NAME "/unique_1487x93"
#define SEM_NAME "/sem_unique_23823x23"

// #define FIRST_FIT
// #define BEST_FIT
#define WORST_FIT

#define ll unsigned long long int

// Define semaphore(s)
sem_t *sema;

// Define your stuctures and variables.
ll sh_mem, segmentsize;
ll *addr;

int smem_init(int segmentsize) {
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_NAME);

    sh_mem = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0777);
    sema = sem_open(SEM_NAME, O_CREAT, 0666, 1);

    if (sh_mem == -1)
        perror("Failed\n");

    if (ftruncate(sh_mem, segmentsize) == -1)
        perror("ftruncate");

    addr = mmap(NULL, segmentsize + 1, PROT_READ | PROT_WRITE,
                MAP_SHARED, sh_mem, 0);

    if (addr == MAP_FAILED)
        perror("mmap");

    addr[0] = segmentsize / 8;  // Segment Size
    addr[1] = segmentsize / 8;  // Next
    addr[2] = 0;

    return (0);
}

int smem_remove() {
    sem_close(sema);
    sem_unlink(SEM_NAME);

    shm_unlink(SHM_NAME);
    return (0);
}

int smem_open() {
    sema = sem_open(SEM_NAME, O_RDWR, 0666, 1);

    sh_mem = shm_open(SHM_NAME, O_RDWR, 0777);
    addr = mmap(0, sizeof(int), PROT_READ | PROT_WRITE,
                MAP_SHARED, sh_mem, 0);

    segmentsize = addr[0];

    addr = mmap(0, segmentsize, PROT_READ | PROT_WRITE,
                MAP_SHARED, sh_mem, 0);

    printf("%p - %p\n", addr + 1, addr + segmentsize + 1);
    return (1);
}

void *smem_alloc(int size) {
    sem_wait(sema);

    size += 7;
    size /= 8;

    if (size > segmentsize) {
        sem_post(sema);
        perror("SIZE OVERFLOW!\n");
    }

    ll *current = addr + 1;

    ll gap = current[0] - (ll)(current - addr - 1) - current[1] - 2ULL;

    // printf("GAP: %llu\n", gap);
    // printf("%llu %llu - ", current[0], current[1]);
    while (gap < size + 2ULL && current[0] != segmentsize) {
        current = (addr + 1) + current[0];
        gap = current[0] - (ll)(current - addr - 1) - current[1] - 2ULL;
    }
#if defined(BEST_FIT) || defined(WORST_FIT)
    ll best_gap = gap;
    ll *best_current = current;
    while (current[0] != segmentsize) {
        current = (addr + 1) + current[0];
        gap = current[0] - (ll)(current - addr - 1) - current[1] - 2ULL;

        if (gap > size + 2ULL) {
#ifdef BEST_FIT
            if (gap < best_gap)
#elif defined(WORST_FIT)
            if (gap > best_gap)
#endif
            {
                gap = best_gap;
                best_current = current;
            }
        }
    }
    gap = best_gap;
    current = best_current;
#endif

    if (gap < size + 2ULL) {
        sem_post(sema);
        perror("ALREADY FULL!\n");
        return (NULL);
    }

    ll *new = current + current[1] + 2;

    new[0] = current[0];
    new[1] = size;

    // Offset data
    current[0] = (int)(current - addr - 1) + current[1] + 2;

    printf("Alloc %p - %p : %d\n", new, new + 2 + new[1], size);
    printf("Return %p\n", new + 2);

    sem_post(sema);

    return new + 2;
}

void smem_free(void *p) {
    sem_wait(sema);

    ll *current = addr + 1;
    ll *last = addr + 1;
    while (current + 2 != p && current + 2 < addr + segmentsize + 1) {
        last = current;
        current = (addr + 1) + current[0];
        // printf("%p\n", current);
    }

    if (current + 2 == p) {
        last[0] = current[0];

        printf("FREED %p\n", p);
    }

    sem_post(sema);
}

int smem_close() {
    int res = munmap(addr, segmentsize);
    if (res == -1)
        perror("munmap");

    sem_close(sema);

    return (0);
}

/*
- shared memory
------------
Segmentsize - 8byte
------------
-1 -> All empty
.
.
.
.
------------
200 ->
.
.
.
.
------------
0x2FF
.
.
.
-------------

*/
