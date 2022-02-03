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
# List all progs connecting a Visited team to another (or an action)
usedProgs = []
progDest = {}
for line in inputFile:
    # Match Edge Team > Prog > Team lines
    if(re.match(r'\s*(T[0-9]+) -> (P[0-9]+) -> (T[0-9]+)', line)):
        teamSrc = re.sub(r'\s*(T[0-9]+) -> (P[0-9]+) -> (T[0-9]+)',r'\1',line).strip()
        prog = re.sub(r'\s*(T[0-9]+) -> (P[0-9]+) -> (T[0-9]+)',r'\2',line).strip()
        teamDst = re.sub(r'\s*(T[0-9]+) -> (P[0-9]+) -> (T[0-9]+)',r'\3',line).strip()
        
        # Keep a record of the destination team.
        progDest[prog] = teamDst
        
        # Prog is used if Src AND DstTeam are visited
        if(teamSrc in teams and teamDst in teams):
            # The program is in the list of executed programs, keep it
            if(prog in progs):
                usedProgs.append(prog)
                
    # Match Edge Team > Prog > Action lines          
    if(re.match(r'\s*(T[0-9]+) -> (P[0-9]+) -> (A[0-9]+)', line)):
        teamSrc = re.sub(r'\s*(T[0-9]+) -> (P[0-9]+) -> (A[0-9]+)',r'\1',line).strip()
        prog = re.sub(r'\s*(T[0-9]+) -> (P[0-9]+) -> (A[0-9]+)',r'\2',line).strip()
        actionDst = re.sub(r'\s*(T[0-9]+) -> (P[0-9]+) -> (A[0-9]+)',r'\3',line).strip()
        
        # Keep a record of the destination team.
        progDest[prog] = actionDst
        
        # Prog is used if Src team is
        if(teamSrc in teams):
            # The program is in the list of executed programs, keep it
            if(prog in progs):
                usedProgs.append(prog)
                
    # Match Edge Team > Prog lines 
    # (Dest of the prog must have been declared before)
    if(re.match(r'\s*(T[0-9]+) -> (P[0-9]+)\s*\n', line)):
        teamSrc = re.sub(r'\s*(T[0-9]+) -> (P[0-9]+)',r'\1',line).strip()
        prog = re.sub(r'\s*(T[0-9]+) -> (P[0-9]+)',r'\2',line).strip()
        dst = progDest[prog]
        
        if(re.match(r'T[0-9]+',dst)):
            isDstTeam = True
            isDstAction = False
        
        if(re.match(r'A[0-9]+',dst)):
            isDstTeam = False
            isDstAction = True
        
        # Prog is used if src team and dstTeam are
        if(isDstTeam and teamSrc in teams and dst in teams):
            # The program is in the list of executed programs, keep it
            if(prog in progs):
                usedProgs.append(prog)

        # Prog is used if src team is
        if(isDstAction and teamSrc in teams):
            # The program is in the list of executed programs, keep it
            if(prog in progs):
                usedProgs.append(prog)

# Keep used progs only
progs = usedProgs

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
            if(prog in progDest):
                dst = progDest[prog]
                progDest.pop(prog)

            if(dst):
                dstString = " -> {}".format(dst)
            else:
                dstString = ""

            outputFile.write("\t\t{} -> {}{}\n".format(teamSrc, prog, dstString))


# Close dot files
inputFile.close()
outputFile.close()