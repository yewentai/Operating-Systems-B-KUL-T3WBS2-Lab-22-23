#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define TEM_MAX 35 * 100
#define TEM_MIN -10 * 100
#define FREQ 1

void get_tem(void)
{
    float tem = (rand() % (TEM_MAX - TEM_MIN + 1) + TEM_MIN) / 100.0;
    printf("Temperature = %05.2f @ ", tem);
    fflush(stdout);
    system("date +'%d-%m-%y %H:%M:%S'");
}

int main()
{
    srand(time(NULL));
    // system("timedatectl set-timezone Europe/Brussels");
    while (1)
    {
        sleep(FREQ);
        get_tem();
    }
    return 0;
}
