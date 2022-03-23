#!/bin/bash
set -e

declare -a games=("alien"  "asteroids" "centipede"  "fishing_derby"  "frostbite")

seeds=`seq 0 9`
threads=48

echo "# Build all targets  ($(date +'%D %X'))"
pushd .
cd ../build

# non-profiling
cmake .. -DPROFILING=0

# for profiling 
# cmake .. -DPROFILING=1

cmake --build . --target ALEGegelati
cmake --build . --target ALEGegelatiCodegen
cmake --build . --target ALEGegelatiInferenceTPG
popd

for game in "${games[@]}"; do
    echo "# Training on $game ($(date +'%D %X'))"

    # Start games
    for i in ${seeds[*]}; do
        echo "# Training on $game with seed $i.  ($(date +'%D %X'))"
        ../build/Release/ALEGegelati -s $i -r $game -p $threads -c ../params-kelly.json 1>dirtyLog 2> /dev/null

        # Print last line of log
        id=$(printf "%02d" $threads)
        tail -n 1 "out.$game.$i.t$id.std"
        
        echo "# Codegen on $game with seed $i.  ($(date +'%D %X'))"
        ../build/Release/ALEGegelatiCodegen -s $i -r $game -i "out_best.$game.$i.t$id.dot" -c ../params-kelly.json 
        
        echo "# Build generated code on $game with seed $i.  ($(date +'%D %X'))"
        pushd .
        cd ../build/
        cmake --build . --target ALEGegelatiInferenceCodegen
        popd
        
	# Commented for profiling
	echo "score;actions;total_time;env_time;tpg_time" > time.codegen.$game.$i.log
        for run in {1..5}; do
            echo "# Run $run/5 generated code on $game with seed $i.  ($(date +'%D %X'))"
            ../build/Release/ALEGegelatiInferenceCodegen -r $game >> time.codegen.$game.$i.log
        done
        
        # Uncomment for profiling
        # ../build/Release/ALEGegelatiInferenceCodegen -r $game > time_analysis.codegen.$game.$i.log
        # gprof ../build/Release/ALEGegelatiInferenceCodegen gmon.out > analysis.txt
        # NB_TEAM_VISITED=$(grep -Ec '^([0-9\.]*[[:space:]]*)* T[0-9]+$' analysis.txt)
        # NB_PROG_VISITED=$(grep -Ec '^([0-9\.]*[[:space:]]*)* P[0-9]+$' analysis.txt)
        # NB_PROG_EXEC=$(grep -Eoi "([0-9])*[[:space:]]*[0-9\.]+[[:space:]]*[0-9\.]+[[:space:]]*P[0-9]+$" analysis.txt | cut -d " " -f 1 | paste -sd+ | bc)
        # NB_TEAM_EXEC=$(grep -Eoi "([0-9])*[[:space:]]*[0-9\.]+[[:space:]]*[0-9\.]+[[:space:]]*T[0-9]+$" analysis.txt | cut -d " " -f 1 | paste -sd+ | bc)
        # echo "Nb teams visited:  $NB_TEAM_VISITED" > stats.codegen.$game.$i.log
        # echo "Nb teams executed: $NB_TEAM_EXEC" >> stats.codegen.$game.$i.log
        # echo "Nb progs visited:  $NB_PROG_VISITED" >> stats.codegen.$game.$i.log
        # echo "Nb progs executed: $NB_PROG_EXEC" >> stats.codegen.$game.$i.log
        # mv analysis.txt analysis.$game.$i.txt
        
	# Commented for profiling
	echo "score;actions;total_time;env_time;tpg_time" > time.tpg.$game.$i.log
        for run in {1..5}; do
            echo "# Run $run/5 TPG on $game with seed $i.  ($(date +'%D %X'))"
            ../build/Release/ALEGegelatiInferenceTPG -r $game -i "out_best_cleaned.$game.$i.t$id.dot" -c ../params-kelly.json >> time.tpg.$game.$i.log
        done
        
    done
done

echo "# Done  ($(date +'%D %X'))"
