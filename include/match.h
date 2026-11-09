#ifndef MATCH_H
#define MATCH_H

#include "team.h"

typedef struct Match {
    int id;
    int team_a_id;
    int team_b_id;
    char date[11];
    int time_slot;
    int is_scheduled;
    int score_a;
    int score_b;
    int has_result;
    struct Match *next;
} Match;

Match *match_create(Match **head);
Match *match_find(Match *head, int id);
int match_view(Match *head, Team *teams, int ready_only);
int match_conflict(Match *head, Match *chosen, const char *date, int slot);
int match_schedule(Match *head, Match *chosen);
int match_enter_result(Match *match);
void match_free(Match *head);

#endif
