#include <stdio.h>
#include "utils.h"
#include "team.h"
#include "tournament.h"
#include "ranking.h"

/* Refresh later rounds after either a new result or a correction. */
static int main_result(Match *matches, Team *teams, TournamentNode *root)
{
    int id;
    Match *chosen;

    if (!match_view(matches, teams, 0))
        return 0;
    if (!utils_read_int("Match ID (0 to return): ", 0, 1000000, &id) || id == 0)
        return 0;
    chosen = match_find(matches, id);
    if (!chosen || !chosen->is_scheduled) {
        puts("Choose a scheduled match.");
        return 0;
    }
    printf("%s vs %s\n", team_name(teams, chosen->team_a_id),
           team_name(teams, chosen->team_b_id));
    if (!match_enter_result(chosen))
        return 0;
    tournament_refresh(root);
    puts("Result saved.");
    return 1;
}

/* Commit a proposed schedule only after all checks pass. */
static void main_schedule(Match *matches, Team *teams, TournamentNode *root)
{
    int id;
    Match *chosen;
    Match proposed;

    if (!match_view(matches, teams, 1))
        return;
    if (!utils_read_int("Match ID (0 to return): ", 0, 1000000, &id) || id == 0)
        return;
    chosen = match_find(matches, id);
    if (!chosen || !chosen->team_a_id || !chosen->team_b_id || chosen->has_result) {
        puts("Choose a ready match without a result.");
        return;
    }
    proposed = *chosen;
    if (!match_schedule(matches, &proposed))
        return;
    if (!tournament_schedule_valid(root, &proposed)) {
        puts("Schedule must be later than both feeder matches.");
        return;
    }
    *chosen = proposed;
    puts("Match scheduled.");
}

/* Leave the roster editable if bracket allocation fails. */
static int main_create_tournament(Team *teams, TournamentNode **root, Match **matches)
{
    Team *next_team = teams;
    int count = team_count(teams);

    if (*root)
        return 1;
    if (count < 2) {
        puts("Add at least two teams first.");
        return 0;
    }
    *root = tournament_build(&next_team, count, matches);
    if (!*root) {
        match_free(*matches);
        *matches = NULL;
        puts("Not enough memory to build the bracket.");
        return 0;
    }
    tournament_refresh(*root);
    puts("Bracket created. The team list is now fixed.");
    return 1;
}

/* Keep menu text separate from the actions. */
static void main_menu(void)
{
    puts("\nSports League Management System");
    puts("1. Add team\n2. View teams\n3. Schedule match\n4. View schedule");
    puts("5. Enter or update match result\n6. View rankings");
    puts("7. View tournament tree\n0. Exit");
}

/* End the loop on either Exit or end of input. */
int main(void)
{
    int choice;
    Team *teams = NULL;
    Match *matches = NULL;
    TournamentNode *root = NULL;
    Ranking *rankings = NULL;

    for (;;) {
        main_menu();
        if (!utils_read_int("Choice: ", 0, 7, &choice) || choice == 0)
            break;
        switch (choice) {
        case 1:
            if (root)
                puts("Teams cannot be added after the bracket is created.");
            else if (team_add(&teams))
                ranking_update(&rankings, teams, matches);
            break;
        case 2: team_view(teams); break;
        case 3:
            if (main_create_tournament(teams, &root, &matches))
                main_schedule(matches, teams, root);
            break;
        case 4: match_view(matches, teams, 0); break;
        case 5:
            if (main_result(matches, teams, root))
                ranking_update(&rankings, teams, matches);
            break;
        case 6:
            if (!rankings && teams)
                ranking_update(&rankings, teams, matches);
            ranking_view(rankings, teams);
            break;
        case 7: tournament_view(root, teams, 0); break;
        }
    }
    team_free(teams);
    tournament_free(root);
    match_free(matches);
    ranking_free(rankings);
    return 0;
}
