#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <math.h>
#include <random>
#include <chrono>


using namespace std;
using namespace std::chrono;

int randCalled = 0;


mt19937 e2;

uint_fast32_t getRandInt() {
    randCalled+=1;
    return e2();
}

double getRandDouble() {
    return (double)getRandInt() / std::mt19937::max();
}



struct MWSATInstance {
    int varsCount;
    int clausesCount;
    vector<int> weights;
    double sumWeight;
    vector<vector<int>> clauses;
};

struct Config {
    int T0 = 0;
    double N;
    int Tmin;
    double alpha;
    double TinitProb;
    int TinitTries;
};


struct StepValue {
    int step;
    double temp;
    int weight;
    int cost;
    int bestWeight;
    int clausesSat;
    int clausesSatBest;
};

struct Result {
    string name;
    int steps;
    int startT;
    int endT;
    int clauses;
    bool sat;
    int satClauses;
    int unsatClauses;
    vector<bool> resultState;
    vector<bool> stepsWeight;
    vector<StepValue> stepValues;
    int resultWeight;
};


bool loadFile(string fileName, MWSATInstance &problem) {
    ifstream inputInstanceFile;

    // Check if the file exists.
    inputInstanceFile.open(fileName, std::ios::in);

    if (!inputInstanceFile.is_open() || !inputInstanceFile) {
        cerr << "File does not exist" << std::endl;
        return false;
    }

    string line;


    while (getline (inputInstanceFile, line)) {
        // Output the text from the file
        if (line[0] == 'c') {
            continue;
        } else if(line[0] == 'p') {
            string tmp;
            istringstream s(line);
            s >> tmp >> tmp >> tmp;
            problem.varsCount = stoi(tmp);
            s >> tmp;
            problem.clausesCount = stoi(tmp);
        } else if(line[0] == 'w') {
            string tmp;
            istringstream s(line);
            s >> tmp;
            problem.sumWeight = 0;
            for (int i = 0; i < problem.varsCount; ++i) {
                s >> tmp;
                problem.sumWeight += stoi(tmp);
                problem.weights.push_back(stoi(tmp));
            }
        } else {
            string tmp;
            istringstream s(line);

            vector<int> clause;

            while (!s.eof()) {
                s >> tmp;
                if(tmp == "0") {
                    break;
                }
                clause.push_back(stoi(tmp));
            }
            problem.clauses.push_back(clause);
        }

    }
    return true;

}

class StateSpace {
public:
    StateSpace(MWSATInstance instance): instance(instance) {}
    StateSpace() {}

    vector<bool> getRandomState() {
        vector<bool> state;

        for (int i = 0; i < instance.varsCount; ++i) {
            state.push_back(getRandInt() % 2);
        }

        return state;
    }


    double getFlipGain(vector<bool> & oldState, vector<bool> & newState) {
        int ma = 0;
        int br = 0;

        for (int i = 0; i < instance.clausesCount; ++i) {


            bool oldSat = false;
            for (int j : instance.clauses[i]) {
                if(j > 0 == oldState[abs(j) - 1]) {
                    oldSat = true;
                    break;
                }
            }

            bool newSat = false;
            for (int j : instance.clauses[i]) {
                if(j > 0 == newState[abs((int) j) - 1]) {
                    newSat = true;
                    break;
                }
            }



            if(oldSat && !newSat) {
                br++;
            } else if(!oldSat && newSat) {
                ma++;
            }
        }


        double cb = 2.3;
        return 1 / pow(br + 1, cb);
    }


    vector<bool> getRandomNeighbour(vector<bool> state, double T) {

        if(isSat(state) || getRandDouble() < pow(T, 0.4) / 100) {
//            for (int i = 0; i < getRandInt() % 3 +1; ++i) {
                int bitToFlip = getRandInt() % instance.varsCount;
                state[bitToFlip] = !state[bitToFlip];
//            }
        } else {

            for (int i = 0; i < instance.clausesCount; ++i) {
                bool sat = false;
                for (int j : instance.clauses[i]) {
                    if((j > 0) == state[abs(j) -1]) {
                        sat = true;
                        break;
                    }
                }
                if (!sat) {
                    int bitToFlip = abs(instance.clauses[i][getRandInt() % instance.clauses[i].size()]) -1 ;
                    state[bitToFlip] = !state[bitToFlip];
                    break;
                }
            }
        }

        return state;
    }

    int getStateWeight(vector<bool> state) {
        int weight = 0;
        for (int i = 0; i < state.size(); ++i) {
            weight += state[i] * instance.weights[i];
        }

        return weight;
    }

    int getSatisfiedClauses(vector<bool> state) {
        int satClauses = 0;

        for (auto & clause : instance.clauses) {
            for (int literal : clause) {
                if((literal > 0) == state[abs(literal) - 1]) {
                    satClauses += 1;
                    break;
                }
            }
        }

        return satClauses;
    }

    bool isSat(vector<bool> state) {
        return isSat(state, getSatisfiedClauses(state));
    }

    bool isSat(vector<bool> state, int satClauses) {
        return satClauses == instance.clausesCount;
    }

private:
    MWSATInstance instance;

};

class SimulatedAnnealing {
public:
    SimulatedAnnealing(MWSATInstance instance, Config config):config(config), instance(instance) {
        stateSpace = StateSpace(instance);
    }
    Result eval() {
        Result result;
        vector<bool> state = stateSpace.getRandomState();
        vector<bool> bestState = state;

        int totalSteps = 0;
        int bestStateCost = getStateCost(state);
        int bestStateWeight = stateSpace.getStateWeight(state);

        if(!config.T0) {
            startTemp = (int) getInitialTemperature(state);
        } else {
            startTemp = config.T0;
        }

        double T = startTemp;

        this->accepted = -1;

        while (!frozen(T)) {
            initInner();

            while (equilibrium()) {
                state = tryState(T, state);
                totalSteps++;

                int newCost = getStateCost(state);



                StepValue stepValue;
                stepValue.step = totalSteps;
                stepValue.temp = T;
                stepValue.weight = stateSpace.getStateWeight(state);
                stepValue.cost = newCost;
                stepValue.bestWeight = bestStateWeight;
                stepValue.clausesSat = stateSpace.getSatisfiedClauses(state);
                stepValue.clausesSatBest = stateSpace.getSatisfiedClauses(bestState);
                result.stepValues.push_back(stepValue);



                if(newCost > bestStateCost) {
                    bestState = state;
                    bestStateCost = newCost;
                    bestStateWeight = stateSpace.getStateWeight(state);

                }

            }
            T = cool(T);
        }

        result.resultState = bestState;
        result.resultWeight = stateSpace.getStateWeight(bestState);
        result.clauses = instance.clausesCount;
        result.satClauses = stateSpace.getSatisfiedClauses(bestState);
        result.unsatClauses = result.clauses - result.satClauses;
        result.steps= totalSteps;
        result.sat = stateSpace.isSat(bestState, result.satClauses);
        result.startT = startTemp;
        result.endT = T;

        return result;

    }

private:
    int steps = 0;
    int accepted = -1;
    int startTemp = 0;
    StateSpace stateSpace;
    MWSATInstance instance;
    Config config;

    bool frozen(double T) {
        return T < config.Tmin || (accepted >= 0 && ((double)accepted / config.N) < 0.1);
    }

    double cool(double T) {
        return T * config.alpha;
    }

    bool equilibrium() {
//        int min = config.N;
//        int max = min *2;


        steps +=1;
        return steps < config.N;
    }

    void initInner() {
        steps = 0;
        accepted = 0;
    }

    vector<bool> tryState(double T, vector<bool> state) {
        vector<bool> neighbour = stateSpace.getRandomNeighbour(state, T);

        double nCost = getStateCost(neighbour);
        double oCost = getStateCost(state);

        if (nCost > oCost) {
            accepted++;
            return neighbour;
        }
        double rnd = getRandDouble();

        if(rnd < exp((nCost - oCost) / T)) {
            accepted++;
            return  neighbour;
        }

        return state;
    }

    double getStateCost(vector<bool> state) {
        int satClauses = stateSpace.getSatisfiedClauses(state);
        int weight = stateSpace.getStateWeight(state);

        if(stateSpace.isSat(state, satClauses)){
            return weight;
        }

//        double mult = ((double)satClauses) / ((double)instance.clausesCount) ;

        return weight  - instance.sumWeight;


//        double koef = pow(instance.sumWeight, 1);
//
//        return weight * mult / koef;
    }

    double getInitialTemperature(vector<bool> state) {

        double temp = 1;

        while (true) {
            int worse = 0;

            for (int i = 0; i < config.TinitTries; ++i) {
                vector<bool> neighbor = tryState(temp, state);

                if (getStateCost(state) > getStateCost(neighbor)) {
                    worse +=1;
                }
                    state = neighbor;
            }

            double prob = (double) worse / config.TinitTries;

            if(prob > config.TinitProb) {
                break;
            }

            temp *= 1.5;

        }

        return temp;
    }

};


void printResult(Result &result) {

    cout << result.name << ' ';
    cout << result.steps << ' ';
    cout << result.sat << ' ';
    cout << result.satClauses << ' ';
    cout << result.unsatClauses << ' ';
    cout << result.clauses << ' ';
    cout << result.startT << ' ';
    cout << result.endT << ' ';
    cout << result.resultWeight << ' ';

//    cout<< endl;

}

void printCsv(Result &result, string filename) {
    std::ofstream myfile;
    myfile.open (filename);

    myfile << "step,temp,weight,cost,best_weight,sat_clauses,sat_clauses_best\n";

    for (auto & step : result.stepValues) {
        myfile << step.step << ',' << step.temp << ',' << step.weight << ',' << step.cost << ',' << step.bestWeight << ',' << step.clausesSat <<  ',' << step.clausesSatBest << '\n';
    }

    myfile.close();
}


int main(int argc, char *argv[]) {

    Config config;

    if(argc < 6) {
        cerr << "Wrong number of arguments" << endl;
        return 1;
    }
    long long seed = std::time(0);
    e2 = mt19937(seed);

    config.alpha = stod(argv[1]);
    config.Tmin = stoi(argv[2]);
    config.N = stoi(argv[3]);
    config.T0 = stoi(argv[4]);
    string fileName = argv[5];

    if (config.Tmin < 0 || (config.T0 != 0 && config.T0 < config.Tmin) || config.N < 1 || config.alpha < 0.5 || config.alpha >=1) {
        cerr << "Wrong parameters" << endl;
        return 1;
    }

    config.TinitProb = 0.5;
    config.TinitTries = 200;

//    config.T0 = 1000;
//    config.N = 100;
//    config.Tmin = 100;
//    config.alpha = 0.995;
//    config.TinitProb = 0.5;

    MWSATInstance problem;
//    fileName = "/home/wpj/Documents/kop/du2/wuf20-71R/wuf20-71R-M/wuf20-03.mwcnf";

    if(!loadFile(fileName, problem)){
        return 1;
    }

    config.N = config.N * problem.varsCount;


//    cout<< "sumWeight: " << problem.sumWeight;

    SimulatedAnnealing sa(problem, config);


    Result result = sa.eval();

    string path = argv[5];
    path = path.substr(path.find_last_of('/')+1);
    path = path.substr(1, path.find_last_of('.')-1);

    result.name = path;
    printResult(result);

    if (argc == 7) {
        printCsv(result, argv[6]);
    }

    return 0;
}
