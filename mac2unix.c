#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    int rc = 0;

    for (int i = 1; i < argc; i++)
    {
        FILE* f = fopen(argv[i], "r+");

        if (f)
        {
            int error = 0;

            if (fseek (f, 0, SEEK_END) != 0)
            {
                fprintf(stderr, "Failed to seek to end of file %s\n", argv[i]);
                fclose(f);
                continue;
            }

            long size = ftell(f);

            rewind(f);

            char* m = (char*) malloc(size * sizeof(char));

            if (!m)
            {
                error = 1;
                fprintf(stderr, "Could not allocate %ld bytes of memory for file %s\n", size, argv[i]);
            }

            size_t totalSizeRead = 0;
            while (totalSizeRead < size && !error)
            {
                size_t sizeRead = fread(m + totalSizeRead, sizeof(char), size, f);

                if(sizeRead == 0 && ferror(f) != 0)
                {
                    error = 1;
                    fprintf(stderr, "Error when doing fread on file %s", argv[i]);
                }
                totalSizeRead += sizeRead;
            }

            if (!error)
            {
                // Translate \r to \n
                for (long pos = 0; pos < size; pos++)
                {
                    if (m[pos] == '\r')
                    {
                        m[pos] = '\n';
                    }
                }

                // Write the file back
                rewind(f);

                size_t totalSizeWritten = 0;

                while(totalSizeWritten < size && !error)
                {
                    size_t sizeWritten = fwrite(m + totalSizeWritten, sizeof(char), size - totalSizeWritten, f);

                    if (sizeWritten == 0)
                    {
                        error = 1;
                        fprintf(stderr, "Failed to write to file %s\n", argv[i]);
                    }

                    totalSizeWritten += sizeWritten;
                }
            }

            if (!error)
            {
                printf("Success converting %s\n", argv[i]);
            }

            fclose(f);
            free(m);
        }
        else
        {
            fprintf(stderr, "Failed to open file %s\n", argv[i]);
        }
    }

    return rc;
}
