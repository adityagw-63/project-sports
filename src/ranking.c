#include "ranking.h"
#include <stdio.h>
#include <stdlib.h>

/* Release old tables and partially built tables the same way. */
void ranking_free(Ranking *head)
{
    while (head) {
        Ranking *next = head->next;
        free(head);
        head = next;
    }
}

/* Start each calculation with one empty record per team. */
static Ranking *ranking_create(Team *teams)
{
    Ranking *head = NULL;

    while (teams) {
        Ranking *node = malloc(sizeof(*node));
        if (!node) {
            ranking_free(head);
            return NULL;
        }
        *node = (Ranking){0};
        node->team_id = teams->id;
        node->next = head;
        head = node;
        teams = teams->next;
    }
    return head;
}

/* Update one side of a completed match. */
static void ranking_apply(Ranking *head, int id, int scored, int conceded)
{
    while (head && head->team_id != id)
        head = head->next;
    if (!head)
        return;
    head->played++;
    head->score_for += scored;
    head->score_against += conceded;
    if (scored > conceded) {
        head->won++;
        head->points += 3;
    } else {
        head->lost++;
    }
}

/* Team ID keeps equal rankings in a predictable display order. */
static int ranking_before(Ranking *a, Ranking *b)
{
    long long difference_a = a->score_for - a->score_against;
    long long difference_b = b->score_for - b->score_against;

    if (a->points != b->points)
        return a->points > b->points;
    if (difference_a != difference_b)
        return difference_a > difference_b;
    return a->team_id < b->team_id;
}

/* Insertion sort changes list links instead of copying team statistics. */
static Ranking *ranking_sort(Ranking *head)
{
    Ranking *sorted = NULL;

    while (head) {
        Ranking *node = head;
        Ranking **position = &sorted;
        head = head->next;
        while (*position && ranking_before(*position, node))
            position = &(*position)->next;
        node->next = *position;
        *position = node;
    }
    return sorted;
}

/* Rebuilding avoids double counting when a stored score is edited. */
int ranking_update(Ranking **head, Team *teams, Match *matches)
{
    Ranking *updated = ranking_create(teams);

    ranking_free(*head);
    *head = NULL;
    if (teams && !updated) {
        puts("Not enough memory for rankings. Results are still stored.");
        return 0;
    }
    while (matches) {
        if (matches->has_result) {
            ranking_apply(updated, matches->team_a_id, matches->score_a, matches->score_b);
            ranking_apply(updated, matches->team_b_id, matches->score_b, matches->score_a);
        }
        matches = matches->next;
    }
    *head = ranking_sort(updated);
    return 1;
}

/* Keep headings short so the table is readable in a terminal. */
void ranking_view(Ranking *head, Team *teams)
{
    int position = 1;

    if (!head) {
        puts("No rankings available.");
        return;
    }
    printf("%-4s %-49s %4s %4s %4s %10s %10s %10s %6s\n",
           "Pos", "Team", "P", "W", "L", "For", "Against", "Diff", "Points");
    while (head) {
        printf("%-4d %-49s %4d %4d %4d %10lld %10lld %10lld %6lld\n",
               position++, team_name(teams, head->team_id), head->played,
               head->won, head->lost, head->score_for, head->score_against,
               head->score_for - head->score_against, head->points);
        head = head->next;
    }
}
