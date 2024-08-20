#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int n = 0, x = 0, y = 0, m = 0;
    char dimensions[256];

    printf("\nEnter The Dimensions For Matrix A, Seperated By A Space (2).\n");

    fgets(dimensions, 256, stdin);

    n = atoi(strtok(dimensions, " "));
    x = atoi(strtok(NULL, " "));

    printf("\nEnter The Dimensions For Matrix B, Seperated By A Space (2).\n");

    fgets(dimensions, 256, stdin);

    y = atoi(strtok(dimensions, " "));
    m = atoi(strtok(NULL, " "));

    if (x != y)
    {
        printf("\nThose Matrices Cannot Be Multiplied.\n");
        exit(1);
    }

    int i, j, k, fpid, p[2], a_pipes[n][m][2], b_pipes[n][m][2], result[3] = { 0 };
    
    // Create Pipe p for sending N x M child process results to parent
    if (pipe(p) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    // Create N x M a-pipes and N x M b-pipes
    // Used for sending elements from Matricies A and B between processes
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            if (pipe(a_pipes[i][j]) == -1)
            {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
            
            if (pipe(b_pipes[i][j]) == -1)
            {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }
    }
    
    i = 0;
    j = -1;
    fpid = 1;
    
    // Create 12 Child Processes (i,j = 0,0 -> n-1,m-1)
    while (fpid > 0)
    {
        j++;
        
        if (j == m)
        {
            i++;
            
            if ((i == n) && (j == m))
            {
                break;
            }
            
            j = 0;
        }
        fpid = fork();
    }
    
    // Child Processes (N x M)
    if (fpid == 0)
    {
        for (k = 0; k < x; k++)
        {
            int a = 0;
            int b = 0;
            result[0] = i;
            result[1] = j;
            
            // Read from a pipe
            read(a_pipes[i][j][0], &a, sizeof(a));
            
            // Read from b pipe
            read(b_pipes[i][j][0], &b, sizeof(b));
            
            result[2] += (a * b);
            
            // Check if edge process
            if (i < n-1)
            {
                // Write b to pipe
                write(b_pipes[i+1][j][1], &b, sizeof(b));
            }
            
            // Check if edge process
            if (j < m-1)
            {
                // Write a to pipe
                write(a_pipes[i][j+1][1], &a, sizeof(a));
            }
        }
        // write sum to pipe p
        write(p[1], &result, sizeof(result));
        
        exit(0);
    }
    
    // Parent Process
    else
    {
        int matrixA[n][x];
        int matrixB[y][m];
        int matrixC[n][m];
        
        char elementsA[256];
        char elementsB[256];
        char* num;

        printf("\nEnter The Elements For Matrix A, Seperated By Spaces (%d).\n", n*x);
        
        fgets(elementsA, 256, stdin);
        
        num = strtok(elementsA, " ");
        
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < x; j++)
            {
                matrixA[i][j] = atoi(num);
                num = strtok(NULL, " ");
            }
        }

        printf("\nEnter The Elements For Matrix B, Seperated By Spaces (%d).\n", y*m);

        fgets(elementsB, 256, stdin);

        num = strtok(elementsB, " ");
        
        for (i = 0; i < y; i++)
        {
            for (j = 0; j < m; j++)
            {
                matrixB[i][j] = atoi(num);
                num = strtok(NULL, " ");
            }
        }

        printf("\nMatrix A (%dx%d)\n", n, x);

        for (i = 0; i < n; i++)
        {
            for (j = 0; j < x; j++)
            {
                printf("%d ", matrixA[i][j]);
            }
            printf("\n");
        }

        printf("\nMatrix B (%dx%d)\n", y, m);

        for (i = 0; i < y; i++)
        {
            for (j = 0; j < m; j++)
            {
                printf("%d ", matrixB[i][j]);
            }
            printf("\n");
        }

        // Send elements from Matrices A and B to boundary processes
        for (k = 0; k < x; k++)
        {
            for (i = 0; i < n; i++)
            {
                // Write to a pipe
                write(a_pipes[i][0][1], &matrixA[i][k], sizeof(matrixA[i][k]));
            }

            for (j = 0; j < m; j++)
            {
                // Write to b pipe
                write(b_pipes[0][j][1], &matrixB[k][j], sizeof(matrixB[k][j]));
            }
        }
        
        // Collect 12 results from children
        for (k = 0; k < (n*m); k++)
        {
            // Read from pipe p
            read(p[0], &result, sizeof(result));
            matrixC[result[0]][result[1]] = result[2];
        }

        printf("\nResult (%dx%d)\n", n, m);
        
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < m; j++)
            {
                printf("%d ", matrixC[i][j]);
            }
            printf("\n");
        }
    }
}
