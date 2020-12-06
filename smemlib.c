
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

// Define a name for your shared memory; you can give any name that start with a slash character; it will be like a filename.
#define SHM_NAME "/unique_1487x93"

// Define semaphore(s)

// Define your stuctures and variables.
int sh_mem;

int smem_init(int segmentsize) {
    shm_unlink(SHM_NAME);
    sh_mem = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0777);

    if (sh_mem == -1)
        perror("Failed\n");

    if (ftruncate(sh_mem, segmentsize) == -1)
        perror("ftruncate");

    return (0);
}

int smem_remove() {
    return (0);
}

int smem_open() {
    char *addr = mmap(0, 8, PROT_READ | PROT_WRITE,
                      MAP_SHARED, sh_mem, 0xf0000);

    return (1);
}

void *smem_alloc(int size) {
    return (NULL);
}

void smem_free(void *p) {
}

int smem_close() {
    shm_unlink(SHM_NAME);
    return (0);
}
