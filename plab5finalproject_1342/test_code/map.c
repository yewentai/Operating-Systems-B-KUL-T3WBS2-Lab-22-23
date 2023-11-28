#include <stdio.h>
#include <stdlib.h>
int main()
{
    FILE *fp = fopen("room_sensor.map", "r");
    int m = 0;
    int n = 2;
    char mid;
    while (!feof(fp))
    {
        mid = fgetc(fp); // get the character
        if (mid == '\n') // if the character is map new line
            m++;         // increase the line count
    }
    rewind(fp); // set the file pointer to the beginning of the file

    int **map;
    map = (int **)malloc(sizeof(int *) * m);
    for (int i = 0; i < m; i++)
        map[i] = (int *)malloc(sizeof(int) * n);
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fscanf(fp, "%d", &map[i][j]);
        }
    }

    puts("The map is:");
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", map[i][j]);
        }
        printf("\n");
    }
    exit(EXIT_SUCCESS);
}