#include <stdio.h>

int main()
{
    int year, month, day, i;
    int daysBefore, firstDay, daysInMonth;
    int leap;

    char *months[] = {
        "January", "February", "March", "April",
        "May", "June", "July", "August",
        "September", "October", "November", "December"
    };

    int days[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    printf("Enter year: ");
    scanf("%d", &year);

    leap = (year % 400 == 0) ||
           (year % 4 == 0 && year % 100 != 0);

    if (leap)
        days[1] = 29;

    daysBefore = (year - 1) * 365
               + (year - 1) / 4
               - (year - 1) / 100
               + (year - 1) / 400;

    firstDay = (daysBefore + 1) % 7;

    for (month = 0; month < 12; month++)
    {
        printf("\n\n     %s %d\n", months[month], year);
        printf("Sun Mon Tue Wed Thu Fri Sat\n");

        daysInMonth = days[month];

        for (i = 0; i < firstDay; i++)
            printf("    ");

        for (day = 1; day <= daysInMonth; day++)
        {
            printf("%3d ", day);

            if ((firstDay + day) % 7 == 0)
                printf("\n");
        }

        firstDay = (firstDay + daysInMonth) % 7;
    }

    return 0;
}
