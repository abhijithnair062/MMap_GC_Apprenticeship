#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
int main(int argc, char *argv[]) {
    const char* filePath = argv[1];
    /*
     * Open the file in Read/Write mode, since we want to update the contents of file in memory
     */
    int fd = open(filePath,O_RDWR);
    assert(fd >= 0);

    /*
     * Use fstat syscall to get the length of the contents of the file
     */
    struct stat statbuf;
    int status = fstat(fd,&statbuf);
    assert(status >= 0);

    /*
     * Using MAP_SHARED : Share this mapping.  Updates to the mapping are visible to
              other processes mapping the same region, and (in the case
              of file-backed mappings) are carried through to the
              underlying file
     */
    char* ptr = mmap(NULL,statbuf.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    assert(ptr != MAP_FAILED);
    close(fd);

    for(size_t i = 0; i < statbuf.st_size / 2; i++){
        int endIndex = statbuf.st_size - i - 1;
        char temp = ptr[i];
        ptr[i] = ptr[endIndex];
        ptr[endIndex] = temp;
    }


    ssize_t outPut = write(1,ptr,statbuf.st_size);
    assert(outPut == statbuf.st_size);

    status = munmap(ptr,statbuf.st_size); //To remove the mapping
    assert(status >= 0);

    return 0;
}
