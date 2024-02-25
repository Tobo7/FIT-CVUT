#include <iostream>


int main(int argc, char *argv[]) {
    int satClauses = stateSpace.getSatisfiedClauses(state);
    int weight = stateSpace.getStateWeight(state);

    if(stateSpace.isSat(state, satClauses)){
        return weight;
    }

    return weight - instance.avgWeight * instance.varsCount;
}

double getStateCost(vector<bool> state) {
    int satClauses = stateSpace.getSatisfiedClauses(state);
    int weight = stateSpace.getStateWeight(state);

    if(stateSpace.isSat(state, satClauses)){
        return weight;
    }

    return weight * ((double)satClauses/2) / ((double)instance.clausesCount);
}


double getStateCost(vector<bool> state) {
    int satClauses = stateSpace.getSatisfiedClauses(state);
    int weight = stateSpace.getStateWeight(state);

    if(stateSpace.isSat(state, satClauses)){
        return weight;
    }
    return weight - instance.avgWeight * instance.varsCount;

}