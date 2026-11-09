#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "team.h"
#include "utils.h"

/* Remove surrounding spaces without changing spaces inside a name. */
static void team_trim(char *name)
{
    int start = 0;
    int length;

    while (name[start] == ' ' || name[start] == '\t')
        start++;
    memmove(name, name + start, strlen(name + start) + 1);
    length = (int)strlen(name);
    while (length > 0 && (name[length - 1] == ' ' || name[length - 1] == '\t'))
        name[--length] = '\0';
}

/* Keep validation separate from allocation and list changes. */
static int team_read_name(char *name)
{
    if (!utils_read_line("Team name: ", name, TEAM_NAME_SIZE))
        return 0;
    team_trim(name);
    if (name[0] == '\0') {
        puts("Team name cannot be empty.");
        return 0;
    }
    return 1;
}

/* Initialize a node fully before linking it into the list. */
static Team *team_create(const char *name, int id)
{
    Team *node = malloc(sizeof(*node));

    if (!node) {
        puts("Not enough memory to add the team.");
        return NULL;
    }
    node->id = id;
    strcpy(node->name, name);
    node->next = NULL;
    return node;
}

/* Append teams so their entry order also determines bracket order. */
int team_add(Team **head)
{
    char name[TEAM_NAME_SIZE];
    Team **tail = head;
    Team *node;
    int id = 1;

    if (!team_read_name(name))
        return 0;
    while (*tail) {
        if (strcmp((*tail)->name, name) == 0) {
            puts("A team with that name already exists.");
            return 0;
        }
        id = (*tail)->id + 1;
        tail = &(*tail)->next;
    }
    node = team_create(name, id);
    if (!node)
        return 0;
    *tail = node;
    printf("Team added with ID %d.\n", id);
    return 1;
}

/* Find a team without copying its record. */
Team *team_find(Team *head, int id)
{
    while (head && head->id != id)
        head = head->next;
    return head;
}

/* Use one label for participants that are not known yet. */
const char *team_name(Team *head, int id)
{
    Team *team = team_find(head, id);
    return team ? team->name : "Pending";
}

/* Count the teams before building the bracket. */
int team_count(Team *head)
{
    int count = 0;
    while (head) {
        count++;
        head = head->next;
    }
    return count;
}

/* Display IDs used by the rest of the program. */
void team_view(Team *head)
{
    if (!head)
        puts("No teams added yet.");
    while (head) {
        printf("%d. %s\n", head->id, head->name);
        head = head->next;
    }
}

/* Save the next link before releasing each node. */
void team_free(Team *head)
{
    while (head) {
        Team *next = head->next;
        free(head);
        head = next;
    }
}
