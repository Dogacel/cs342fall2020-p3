

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "smemlib.h"

int main() {
    smem_init(1024);

    printf("Memory segment is created.\n");

    return (0);
}
