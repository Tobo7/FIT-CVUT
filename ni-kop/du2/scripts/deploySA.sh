SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

g++ -o $SCRIPT_DIR/SA $SCRIPT_DIR/../source/simulated_annealing/main.cpp 
