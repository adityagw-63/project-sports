#include "tournament.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* A parent fixture must occur strictly after each feeder match. */
static int tournament_is_later(Match *chosen, TournamentNode *child)
{
    int order;

    if (!child->match)
        return 1;
    if (!child->match->has_result || !child->match->is_scheduled)
        return 0;
    order = strcmp(chosen->date, child->match->date);
    return order > 0 || (order == 0 && chosen->time_slot > child->match->time_slot);
}

/* Locate the fixture's tree node to check its feeder dates. */
int tournament_schedule_valid(TournamentNode *root, Match *chosen)
{
    if (!root || !root->match)
        return 0;
    if (root->match->id == chosen->id)
        return tournament_is_later(chosen, root->left) &&
               tournament_is_later(chosen, root->right);
    return tournament_schedule_valid(root->left, chosen) ||
           tournament_schedule_valid(root->right, chosen);
}

/* Split the entrants into two groups; each internal node is one match. */
TournamentNode *tournament_build(Team **next_team, int count, Match **matches)
{
    TournamentNode *node = malloc(sizeof(*node));

    if (!node)
        return NULL;
    *node = (TournamentNode){0};
    if (count == 1) {
        node->team_id = (*next_team)->id;
        *next_team = (*next_team)->next;
        return node;
    }
    node->left = tournament_build(next_team, count / 2, matches);
    if (node->left)
        node->right = tournament_build(next_team, count - count / 2, matches);
    if (node->left && node->right)
        node->match = match_create(matches);
    if (!node->match) {
        tournament_free(node);
        return NULL;
    }
    return node;
}

/* A subtree supplies a team only after its match has a result. */
static int tournament_winner(TournamentNode *node)
{
    if (!node->match)
        return node->team_id;
    if (!node->match->has_result)
        return 0;
    return node->match->score_a > node->match->score_b
        ? node->match->team_a_id : node->match->team_b_id;
}

/* Work upward so every match receives the latest child winners. */
void tournament_refresh(TournamentNode *root)
{
    Match *match;
    int team_a, team_b;

    if (!root || !root->match)
        return;
    tournament_refresh(root->left);
    tournament_refresh(root->right);
    match = root->match;
    team_a = tournament_winner(root->left);
    team_b = tournament_winner(root->right);
    if (match->team_a_id != team_a || match->team_b_id != team_b) {
        if (match->is_scheduled || match->has_result)
            printf("Match %d cleared because its participants changed.\n", match->id);
        match->is_scheduled = 0;
        match->has_result = 0;
        match->date[0] = '\0';
        match->time_slot = 0;
        match->score_a = match->score_b = 0;
        match->team_a_id = team_a;
        match->team_b_id = team_b;
    }
}

/* Indentation shows which teams or matches feed into each match. */
void tournament_view(TournamentNode *root, Team *teams, int depth)
{
    int i;

    if (!root) {
        puts("Start scheduling to create the tournament bracket.");
        return;
    }
    for (i = 0; i < depth; i++)
        printf("    ");
    if (!root->match) {
        puts(team_name(teams, root->team_id));
        return;
    }
    printf("%sMatch %d: %s vs %s", depth == 0 ? "Final - " : "",
           root->match->id, team_name(teams, root->match->team_a_id),
           team_name(teams, root->match->team_b_id));
    if (root->match->has_result)
        printf(" | %d - %d | Winner: %s", root->match->score_a,
               root->match->score_b, team_name(teams, tournament_winner(root)));
    putchar('\n');
    tournament_view(root->left, teams, depth + 1);
    tournament_view(root->right, teams, depth + 1);
}

/* Match records are freed separately by match_free. */
void tournament_free(TournamentNode *root)
{
    if (!root)
        return;
    tournament_free(root->left);
    tournament_free(root->right);
    free(root);
}
