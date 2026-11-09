# Sports League Management System

A menu-driven C99 college DSA project. Teams, matches, and rankings use
singly linked lists. A binary tree represents a knockout tournament.
All data stays in memory and is lost when the program exits.

## Build and run

Install GCC and GNU Make and make sure both are on PATH. In a terminal,
open this folder and run:

```sh
make
make run
make clean
```

`make` builds the program, `make run` starts it, and `make clean` removes
the executable. The compiler flags are `-Wall -Wextra -std=c99`.
On Windows, a Make installation named `mingw32-make` can use the same
targets, for example `mingw32-make run`.

If Make is unavailable, build directly with GCC:

```sh
gcc -Wall -Wextra -std=c99 -Iinclude src/main.c src/team.c src/match.c src/tournament.c src/ranking.c src/utils.c -o sports-league.exe
./sports-league.exe
```

## Menu

```text
1. Add team
2. View teams
3. Schedule match
4. View schedule
5. Enter or update match result
6. View rankings
7. View tournament tree
0. Exit
```

Add at least two teams before scheduling. The first scheduling attempt
creates the bracket in team entry order and fixes the roster. Any team
count is supported; some teams receive a bye when the count is not a
power of two. In the tree, indented entries feed into the match above them.

Schedule ready matches by ID using a date in `YYYY-MM-DD` format and a
numbered time slot. Slots are fixed, non-overlapping periods that you
assign consistently. A team cannot play twice in one date/slot, and a
later-round match must follow its feeder matches. Option 3 can reschedule
a match until it has a result. Enter `0` at a match ID prompt to return.

Scores range from 0 to 1000000; slots range from 1 to 1000000. Ties are
rejected because every knockout match needs a winner. Winners earn three
points and losers earn zero. Rankings are recalculated after every result
change, ordered by points, then score difference, then team ID.

If a correction changes a winner, affected later matches lose their
schedules and results. Schedule them again after their participants are
known. The tree decides advancement; the ranking table shows performance.
