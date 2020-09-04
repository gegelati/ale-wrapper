#!/bin/bash

declare -a games=("alien"  "asteroids" "centipede"  "fishing_derby"  "frostbite")

for game in "${games[@]}"; do
    echo "Training on $game"

    # Start games
    for i in `seq 0 4`; do
        ../build/Release/ALEGegelati -s $i -r $game 2> /dev/null &
        pids[${i}]=$!
    done

    # Wait for completion
    for pid in ${pids[*]}; do
        wait $pid
    done


done

echo Done


