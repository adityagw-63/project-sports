#ifndef UTILS_H
#define UTILS_H

int utils_read_line(const char *prompt, char *text, int size);
int utils_read_int(const char *prompt, int min, int max, int *value);
int utils_valid_date(const char *date);

#endif
