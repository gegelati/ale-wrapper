#!/bin/bash
set -e

declare -a games=("alien"  "asteroids" "centipede"  "fishing_derby"  "frostbite")

seeds=0  #`seq 0 4`
threads=48

echo "# Build all targets  ($(date +'%D %X'))"
pushd .
cd ../build
cmake --build . --target ALEGegelati
cmake --build . --target ALEGegelatiCodegen
cmake --build . --target ALEGegelatiInferenceTPG
popd

for game in "${games[@]}"; do
    echo "# Training on $game ($(date +'%D %X'))"

    # Start games
    for i in ${seeds[*]}; do
        echo "# Training on $game with seed $i.  ($(date +'%D %X'))"
        ../build/Release/ALEGegelati -s $i -r $game -p $threads -c ../param-kelly.json 1>dirtyLog 2> /dev/null

        # Print last line of log
        id=$(printf "%02d" $threads)
        tail -n 1 "out.$game.$i.t$id.std"
        
        echo "# Codegen on $game with seed $i.  ($(date +'%D %X'))"
        ../build/Release/ALEGegelatiCodegen -i "out_best.$game.$i.t$id.dot" -c ../params-kelly.json 
        
        echo "# Build generated code on $game with seed $i.  ($(date +'%D %X'))"
        pushd .
        cd ../build/
        cmake --build . --target ALEGegelatiInferenceCodegen
        popd
        
        for run in {1..5}; do
            echo "# Run $run/5 generated code on $game with seed $i.  ($(date +'%D %X'))"
            ../build/Release/ALEGegelatiInferenceCodegen -r $game >> time.codegen.$game.$i.log
        done
        
        for run in {1..5}; do
            echo "# Run $run/5 TPG on $game with seed $i.  ($(date +'%D %X'))"
            ../build/Release/ALEGegelatiInferenceTPG -r $game -i "out_best.$game.$i.t$id.dot" -c ../params-kelly.json >> time.tpg.$game.$i.log
        done
        
    done
done

echo "# Done  ($(date +'%D %X'))"
