# This script prepares the release note for the latest version from the change log.
#
# Author: K. Desnos
# License: CeCILL-C

import sys
import re
import os
import shutil


# Initial info
inputFilePath = sys.argv[1];
teamFilePath = sys.argv[2];
progFilePath = sys.argv[3];
outputFilePath = sys.argv[4];
print('Filter dot: ' + inputFilePath)
print('Team list:  ' + teamFilePath)
print('Prog list:  ' + progFilePath)
print('Output dot: ' + outputFilePath)

# Open files
inputFile = open(inputFilePath, "r")
teamFile = open(teamFilePath, "r")
progFile = open(progFilePath, "r")
outputFile = open(outputFilePath, "w")

# Load team and progs into a list.
teams = teamFile.read().split('\n')
teams.remove("")
teamFile.close()

progs = progFile.read().split('\n')
progs.remove("")
progFile.close()

# Filter unused prog
# Remove progs Linking a visited team to a team never visited.
for line in inputFile:
    # Match Edge Team > Prog > Team lines
    if(re.match(r'\s*(T[0-9]+) -> (P[0-9]+) -> (T[0-9]+)', line)):
        teamSrc = re.sub(r'\s*(T[0-9]+) -> (P[0-9]+) -> (T[0-9]+)',r'\1',line).strip()
        prog = re.sub(r'\s*(T[0-9]+) -> (P[0-9]+) -> (T[0-9]+)',r'\2',line).strip()
        teamDst = re.sub(r'\s*(T[0-9]+) -> (P[0-9]+) -> (T[0-9]+)',r'\3',line).strip()

        # Check absence of any of the two teams in visited teams
        # Meaning the path of this program is never taken
        if(not (teamSrc in teams and teamDst in teams)):
            # If the program is in the list of executed programs, remove it
            if(prog in progs):
                progs.remove(prog)

# Rewind inputFile
inputFile.seek(0)

# Build list of visited actions
actions = []
for line in inputFile:
    # Match Edge Team > Prog (> Team|Action) lines
    if(re.match(r'\s*(T[0-9]+) -> (P[0-9]+)( -> ((T|A)[0-9]+))?', line)):
        teamSrc = re.sub(r'\s*(T[0-9]+) -> (P[0-9]+)( -> ((T|A)[0-9]+))?',r'\1',line).strip()
        prog = re.sub(r'\s*(T[0-9]+) -> (P[0-9]+)( -> ((T|A)[0-9]+))?',r'\2',line).strip()
        dst = re.sub(r'\s*(T[0-9]+) -> (P[0-9]+)( -> ((T|A)[0-9]+))?',r'\4',line).strip()

        # Checking source team and prog is sufficient since prog were already filtered.
        if(teamSrc in teams and prog in progs):
            if(re.match(r'A[0-9]+', dst)):
                actions.append(dst)

# Rewind inputFile
inputFile.seek(0)

# Scan input file line by line
for line in inputFile:
    # print(line, end='')

    # Identify the type of line
    # re.match(r'## Release version [0-9]+\.[0-9]+\.[0-9]+',line)
    # header/footer lines
    if(re.match(r'\s*(digraph|graph|node|\}|\{ rank).*',line)):
        outputFile.write(line)

    # Team lines
    if(re.match(r'\s*(T[0-9]+)\s*\[fillcolor=.*',line)):
        # Keep only team from teams list
        team = re.sub(r'\s*(T[0-9]+)\s*\[fillcolor=.*',r'\1',line).strip()
        if(team in teams):
            outputFile.write(line)

    # Prog and Instruction lines
    if(re.match(r'\s*((P|I)([0-9]+))\s*\[.*',line)):
        # Keep only prog from progs list
        prog = 'P' + re.sub(r'\s*((P|I)([0-9]+))\s*\[.*',r'\3',line).strip()
        if(prog in progs):
            outputFile.write(line)

    # Action lines
    if(re.match(r'\s*(A[0-9]+)\s*\[fillcolor=.*',line)):
        # Keep only Actions visited
        action = re.sub('\s*(A[0-9]+)\s*\[fillcolor=.*',r'\1',line).strip()
        if(action in actions):
            outputFile.write(line)

    # Edge Prog > Instructions
    if(re.match(r'\s*(P[0-9]+) -> I[0-9]+\[style=invis\]',line)):
        # Keep only prog from progs list
        prog = re.sub(r'\s*(P[0-9]+) -> I[0-9]+\[style=invis\]',r'\1',line).strip()
        if(prog in progs):
            outputFile.write(line)

    # Match Edge Team > Prog > Team lines
    if(re.match(r'\s*(T[0-9]+) -> (P[0-9]+)( -> ((T|A)[0-9]+))?', line)):
        teamSrc = re.sub(r'\s*(T[0-9]+) -> (P[0-9]+)( -> ((T|A)[0-9]+))?',r'\1',line).strip()
        prog = re.sub(r'\s*(T[0-9]+) -> (P[0-9]+)( -> ((T|A)[0-9]+))?',r'\2',line).strip()
        dst = re.sub(r'\s*(T[0-9]+) -> (P[0-9]+)( -> ((T|A)[0-9]+))?',r'\4',line).strip()

        # Checking source team and prog is sufficient since prog were already filtered.
        if(teamSrc in teams and prog in progs):
            outputFile.write(line)


# Close dot files
inputFile.close()
outputFile.close()