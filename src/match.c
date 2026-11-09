#include "match.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

/* Read both scores before replacing an existing result. */
int match_enter_result(Match *match)
{
    int score_a, score_b;

    if (!utils_read_int("First team's score (0-1000000): ", 0, 1000000, &score_a) ||
        !utils_read_int("Second team's score (0-1000000): ", 0, 1000000, &score_b))
        return 0;
    if (score_a == score_b) {
        puts("A knockout match needs a winner. Tied scores were not saved.");
        return 0;
    }
    match->score_a = score_a;
    match->score_b = score_b;
    match->has_result = 1;
    return 1;
}

/* Ignore the selected match itself when checking a reschedule. */
int match_conflict(Match *head, Match *chosen, const char *date, int slot)
{
    while (head) {
        if (head->id != chosen->id && head->is_scheduled &&
            strcmp(head->date, date) == 0 && head->time_slot == slot) {
            if (head->team_a_id == chosen->team_a_id ||
                head->team_a_id == chosen->team_b_id ||
                head->team_b_id == chosen->team_a_id ||
                head->team_b_id == chosen->team_b_id)
                return 1;
        }
        head = head->next;
    }
    return 0;
}

/* The caller passes a copy so rejected changes do not alter the fixture. */
int match_schedule(Match *head, Match *chosen)
{
    char date[80];
    int slot;

    while (utils_read_line("Date (YYYY-MM-DD): ", date, sizeof(date))) {
        if (utils_valid_date(date))
            break;
        puts("Enter a valid calendar date in YYYY-MM-DD format.");
    }
    if (feof(stdin) || ferror(stdin))
        return 0;
    if (!utils_read_int("Time slot (1-1000000): ", 1, 1000000, &slot))
        return 0;
    if (match_conflict(head, chosen, date, slot)) {
        puts("Conflict: a team already plays on that date and slot.");
        return 0;
    }
    strcpy(chosen->date, date);
    chosen->time_slot = slot;
    chosen->is_scheduled = 1;
    return 1;
}

/* Keep fixtures in the same order as their assigned IDs. */
Match *match_create(Match **head)
{
    Match *node = malloc(sizeof(*node));
    int id = 1;

    if (!node)
        return NULL;
    while (*head) {
        id = (*head)->id + 1;
        head = &(*head)->next;
    }
    *node = (Match){0};
    node->id = id;
    *head = node;
    return node;
}

/* Locate a fixture by its displayed match ID. */
Match *match_find(Match *head, int id)
{
    while (head && head->id != id)
        head = head->next;
    return head;
}

/* Show either ready fixtures or the scheduled match list. */
int match_view(Match *head, Team *teams, int ready_only)
{
    int shown = 0;

    while (head) {
        int ready = head->team_a_id && head->team_b_id && !head->has_result;
        if ((ready_only && ready) || (!ready_only && head->is_scheduled)) {
            printf("Match %d: %s vs %s", head->id,
                   team_name(teams, head->team_a_id), team_name(teams, head->team_b_id));
            if (head->is_scheduled)
                printf(" | %s, slot %d", head->date, head->time_slot);
            else
                printf(" | Not scheduled");
            if (head->has_result)
                printf(" | Score: %d - %d", head->score_a, head->score_b);
            putchar('\n');
            shown++;
        }
        head = head->next;
    }
    if (!shown)
        puts(ready_only ? "No matches are ready for scheduling." : "No scheduled matches.");
    return shown;
}

/* The match list owns these records, including those used by the tree. */
void match_free(Match *head)
{
    while (head) {
        Match *next = head->next;
        free(head);
        head = next;
    }
}
