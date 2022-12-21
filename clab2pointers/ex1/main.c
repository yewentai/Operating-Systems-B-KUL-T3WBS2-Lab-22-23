#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    short day, month;
    unsigned year;
} date_t;

date_t* date_struct(int day, int month, int year)
{
    date_t* dummy = malloc(sizeof(date_t));
    dummy->day = (short)day;
    dummy->month = (short)month;
    dummy->year = (unsigned)year;
    //(*dummy).day = (short)day;
    //(*dummy).month = (short)month;
    //(*dummy).year = (unsigned)year;
    return dummy;
}

int main(void)
{
    int day, month, year;
    date_t* d;
    printf("\nGive day, month, year(like 12 12 2000):\n");
    scanf("%d %d %d", &day, &month, &year);
    d = date_struct(day, month, year);
    printf("date struct values: %d-%d-%d\n", d->day, d->month, d->year);
    free(d); // free memory
    return 0;
}