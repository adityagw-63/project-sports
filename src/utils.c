#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

/* Discard an oversized line so it cannot become the next answer. */
int utils_read_line(const char *prompt, char *text, int size)
{
    int ch;
    char *end;

    for (;;) {
        printf("%s", prompt);
        fflush(stdout);
        if (!fgets(text, size, stdin))
            return 0;
        end = strchr(text, '\n');
        if (end) {
            *end = '\0';
            if (end > text && end[-1] == '\r')
                end[-1] = '\0';
            return 1;
        }
        ch = getchar();
        if (ch == '\n' || ch == EOF)
            return 1;
        while ((ch = getchar()) != '\n' && ch != EOF) { }
        puts("Input is too long. Please try again.");
    }
}

/* Limit significant digits before strtol so conversion cannot overflow. */
static int utils_parse_int(const char *text, int min, int max, int *value)
{
    const char *start;
    const char *significant;
    long number;

    while (*text == ' ' || *text == '\t')
        text++;
    start = text;
    while (*text == '0')
        text++;
    significant = text;
    while (*text >= '0' && *text <= '9')
        text++;
    if (text == start || text - significant > 9)
        return 0;
    while (*text == ' ' || *text == '\t')
        text++;
    if (*text != '\0')
        return 0;
    number = strtol(start, NULL, 10);
    if (number < min || number > max)
        return 0;
    *value = (int)number;
    return 1;
}

/* Retry invalid numbers, but let the caller handle end of input. */
int utils_read_int(const char *prompt, int min, int max, int *value)
{
    char text[80];

    while (utils_read_line(prompt, text, sizeof(text))) {
        if (utils_parse_int(text, min, max, value))
            return 1;
        printf("Enter a whole number from %d to %d.\n", min, max);
    }
    return 0;
}

/* Validate the calendar date before allowing string comparisons. */
int utils_valid_date(const char *date)
{
    int year, month, day, i;
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (strlen(date) != 10 || date[4] != '-' || date[7] != '-')
        return 0;
    for (i = 0; i < 10; i++) {
        if (i != 4 && i != 7 && (date[i] < '0' || date[i] > '9'))
            return 0;
    }
    if (sscanf(date, "%d-%d-%d", &year, &month, &day) != 3)
        return 0;
    if (year < 1 || month < 1 || month > 12)
        return 0;
    if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))
        days[1] = 29;
    return day >= 1 && day <= days[month - 1];
}
