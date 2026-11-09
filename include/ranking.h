#ifndef RANKING_H
#define RANKING_H

#include "match.h"

typedef struct Ranking {
    int team_id;
    int played;
    int won;
    int lost;
    long long score_for;
    long long score_against;
    long long points;
    struct Ranking *next;
} Ranking;

int ranking_update(Ranking **head, Team *teams, Match *matches);
void ranking_view(Ranking *head, Team *teams);
void ranking_free(Ranking *head);

#endif
