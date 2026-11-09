CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
SOURCES = src/main.c src/team.c src/match.c src/tournament.c src/ranking.c src/utils.c
HEADERS = include/team.h include/match.h include/tournament.h include/ranking.h include/utils.h

ifeq ($(OS),Windows_NT)
TARGET = sports-league.exe
else
TARGET = sports-league
endif

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
ifeq ($(OS),Windows_NT)
	cmd /c "if exist $(TARGET) del /Q $(TARGET)"
else
	rm -f $(TARGET)
endif
