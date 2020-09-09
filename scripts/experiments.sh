#!/bin/bash

declare -a games=("alien"  "asteroids" "centipede"  "fishing_derby"  "frostbite")

nbThreads=`seq 2 24`

seeds=`seq 0 4`
seedsInParallel=false

for game in "${games[@]}"; do
    echo "Training on $game"

    for threads in ${nbThreads[@]}; do
        echo "Nb thread $threads"

        # Start games
        if $seedsInParallel; then
            echo "Launch games in parallel"
            for i in ${seeds[*]}; do
                 ../build/Release/ALEGegelati -s $i -r $game -p $threads 1>dirtyLog 2> /dev/null &
                 pids[${i}]=$!
            done

            # Wait for completion
            for pid in ${pids[*]}; do
                wait $pid
            done
        else
            echo "Launch games sequentially"
            for i in ${seeds[*]}; do
                ../build/Release/ALEGegelati -s $i -r $game -p $threads 1>dirtyLog 2> /dev/null
                # Print last line of log
                id=$(printf "%02d" $threads)
                tail -n 1 "out.$game.$i.t$id.std"
            done
        fi
    done
done

echo Done


