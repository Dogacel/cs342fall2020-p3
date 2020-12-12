

#include <stdlib.h>
#include <unistd.h>

#include "smemlib.h"

int main(int argc, char** argv) {
    int ret;
    char* p;

    for (int i = 0; i < argv[1][0] - '0'; i++) {
        int pid = fork();
        if (pid != 0) {
            ret = smem_open();
            if (ret == -1)
                exit(1);

            p = smem_alloc(8);
            p = smem_alloc(16);

            char* old_p = smem_alloc(16);
            p = smem_alloc(16);
            smem_free(old_p);
            p = smem_alloc(16);

            p = smem_alloc(512);  // allocate space to hold 1024 characters

            smem_free(p);

            smem_close();
            return 0;
        }
    }

    return (0);
}
