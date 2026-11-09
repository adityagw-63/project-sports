#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include "match.h"

typedef struct TournamentNode {
    int team_id;
    Match *match;
    struct TournamentNode *left;
    struct TournamentNode *right;
} TournamentNode;

TournamentNode *tournament_build(Team **next_team, int count, Match **matches);
void tournament_refresh(TournamentNode *root);
int tournament_schedule_valid(TournamentNode *root, Match *chosen);
void tournament_view(TournamentNode *root, Team *teams, int depth);
void tournament_free(TournamentNode *root);

#endif
