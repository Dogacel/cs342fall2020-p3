
#include <stdio.h>
#include <stdlib.h>
#include "smemlib.h"

// Define a name for your shared memory; you can give any name that start with a slash character; it will be like a filename.

// Define semaphore(s)

// Define your stuctures and variables. 


int smem_init()
{
    printf ("smem init called"); // remove all printfs when you are submitting to us.  
    return (0); 
}

int smem_remove()
{

    return (0); 
}


void *smem_alloc (int size)
{
    return (NULL);
}


void smem_free (void *p)
{

 
}


