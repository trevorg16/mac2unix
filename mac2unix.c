#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


#define READ_SIZE (4096)

#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)

int main(int argc, char** argv)
{
    int rc = 0;

    for (int i = 1; i < argc; i++)
    {
        FILE* f = fopen(argv[i], "r+");

        if (f)
        {
            int error = 0;

            if (fseeko (f, 0, SEEK_END) != 0)
            {
                fprintf(stderr, "Failed to seek to end of file %s\n", argv[i]);
                fclose(f);
                continue;
            }

            off_t size = ftello(f);

            char m[READ_SIZE];

            off_t totalSizeProcessed = 0;

            while (totalSizeProcessed < size)
            {
                if(fseeko(f, totalSizeProcessed, SEEK_SET) != 0)
                {
                    error = 1;
                    fprintf(stderr, "Failed to seek to position %ld for file %s\n", totalSizeProcessed, argv[i]);
                    break;
                }

                size_t readSize = fread(m, sizeof(char), MIN(READ_SIZE, size - totalSizeProcessed), f);

                if (readSize == 0 && ferror(f))
                {
                    error = 1;
                    fprintf(stderr, "Error when doing fread on file %s", argv[i]);
                    break;
                }

                // Translate \r to \n
                for (size_t pos = 0; pos < readSize; pos++)
                {
                    if (m[pos] == '\r')
                    {
                        m[pos] = '\n';
                    }
                }

                if(fseeko(f, totalSizeProcessed, SEEK_SET) != 0)
                {
                    error = 1;
                    fprintf(stderr, "Failed to seek to position %ld for file %s\n", totalSizeProcessed, argv[i]);
                    break;
                }

                size_t totalSizeWritten = 0;

                while (totalSizeWritten < readSize)
                {
                    size_t sizeWritten = fwrite(m, sizeof(char), readSize - totalSizeWritten, f);

                    if (sizeWritten == 0)
                    {
                        error = 1;
                        fprintf(stderr, "Failed to write to file %s\n", argv[i]);
                        break;
                    }

                    totalSizeWritten += sizeWritten;
                }

                totalSizeProcessed += totalSizeWritten;
            }

            if (!error)
            {
                fprintf(stderr, "Success converting %s\n", argv[i]);
            }

            fclose(f);
        }
        else
        {
            fprintf(stderr, "Failed to open file %s\n", argv[i]);
        }
    }

    return rc;
}
