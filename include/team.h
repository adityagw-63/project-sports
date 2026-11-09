#ifndef TEAM_H
#define TEAM_H

#define TEAM_NAME_SIZE 50

typedef struct Team {
    int id;
    char name[TEAM_NAME_SIZE];
    struct Team *next;
} Team;

int team_add(Team **head);
Team *team_find(Team *head, int id);
const char *team_name(Team *head, int id);
int team_count(Team *head);
void team_view(Team *head);
void team_free(Team *head);

#endif
