
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

// Define a name for your shared memory; you can give any name that start with a slash character; it will be like a filename.
#define SHM_NAME "/unique_1487x93"

#define ll unsigned long long int

// Define semaphore(s)

// Define your stuctures and variables.
ll sh_mem, segmentsize;
ll *addr;

int smem_init(int segmentsize) {
    shm_unlink(SHM_NAME);
    sh_mem = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0777);

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
    shm_unlink(SHM_NAME);
    return (0);
}

int smem_open() {
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
    size += 7;
    size /= 8;

    if (size > segmentsize) {
        perror("SIZE OVERFLOW!\n");
    }

    ll *current = addr + 1;

    ll gap = current[0] - (ll)(current - addr - 1) - current[1] - 2ULL;

    // printf("GAP: %llu\n", gap);
    // printf("%llu %llu - ", current[0], current[1]);
    while (gap < size + 2ULL && current[0] != segmentsize) {
        current = (addr + 1) + current[0];
        gap = current[0] - (ll)(current - addr - 1) - current[1] - 2ULL;
        // printf("GAP: %llu\n", gap);
        // printf("%llu %llu - ", current[0], current[1]);
    }

    if (gap < size + 2ULL) {
        perror("ALREADY FULL!\n");
        return (NULL);
    }

    ll *new = current + current[1] + 2;

    new[0] = current[0];
    new[1] = size;

    // Offset data
    current[0] = (int)(current - addr - 1) + current[1] + 2;

    printf("Alloc %p - %p : %lld\n", new, new + 2 + new[1], size);
    printf("Return %p\n", new + 2);

    return new + 2;
}

void smem_free(void *p) {
    ll *current = addr + 1;
    ll *last = addr + 1;
    while (current + 2 != p) {
        last = current;
        current = (addr + 1) + current[0];
    }

    last[0] = current[0];

    printf("FREED %p\n", p);
}

int smem_close() {
    int res = munmap(addr, segmentsize);
    if (res == -1)
        perror("munmap");

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
