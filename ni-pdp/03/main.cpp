#include <iostream>
#include <utility>
#include <fstream>
#include <sstream>
#include <climits>
#include <vector>
#include <queue>
#include <omp.h>
#include <chrono>

using namespace std;

enum NodeGroup { GROUP_X, GROUP_Y, GROUP_UNASSIGNED };

// Structure for storing data from input file
class InputMatrix {
public:
    InputMatrix(vector<vector<int>> matrix, int nodes): matrix(std::move(matrix)), nodes(nodes){};
    int getIndex(int x,int y) {
        return matrix[x][y];
    }
    int getNodesCount() {
        return nodes;
    }
private:
    vector<vector<int>> matrix;
    int nodes;
};

// function for loading input stream into the InputMatrix structure
InputMatrix * readInput(ifstream & inputStream) {
    vector<vector<int>> matrix;
    string line;

    getline(inputStream, line);

    int nodes = stoi(line);

    while (getline(inputStream, line)) {
        stringstream ss(line);

        string part;
        vector<int> matrixRow;
        while (getline(ss, part , ' ')) {
            if(!part.empty()) {
                matrixRow.push_back(stoi(part));
            }
        }
        matrix.push_back(matrixRow);

    }

    return new InputMatrix(matrix, nodes);
}

// Class for storing current state of the step in DFS recursion
class State {
public:
    State() {
        inputMatrix = nullptr;
    };

    State(InputMatrix * inputMatrix):inputMatrix(inputMatrix) {
        stateWeight = 0;
    };

    State(InputMatrix * inputMatrix, NodeGroup newGroup, vector<NodeGroup> nodesGroups):inputMatrix(inputMatrix) {
        nodesGroups.push_back(newGroup);
        this->nodesGroups = std::move(nodesGroups);
    };

    // current weight of the State
    int getWeight() {
        return stateWeight;
    }

    // Get amount of nodes assigned with the needed group
    int getGroupCount(NodeGroup neededGroup) {
        if(neededGroup == GROUP_X) {
            return groupXCount;
        } else if (neededGroup == GROUP_Y) {
            return groupYCount;
        }

        return 0;
    }

    // Get lower estimate of the weight of the remaining cut
    int getMinCutWeight() {
        vector<NodeGroup> nodesMinCut = nodesGroups;
        int minWeightCut = stateWeight;
        for (int i = nodesGroups.size(); i < inputMatrix->getNodesCount(); ++i) {

            const int wX = getWeightNextGroup(nodesMinCut, GROUP_X);
            const int wY = getWeightNextGroup(nodesMinCut, GROUP_Y);

            if(wX > wY) {
                minWeightCut += wY;
            } else {
                minWeightCut += wX;
            }
            nodesMinCut.push_back(GROUP_UNASSIGNED);

        }

        return minWeightCut;
    }

    // Return a new state extended with the new group
    State getNextState(NodeGroup newGroup) {
        State newState = State(inputMatrix, newGroup, nodesGroups);
        newState.stateWeight = stateWeight + getWeightNextGroup(nodesGroups, newGroup);
        newState.groupXCount = groupXCount;
        newState.groupYCount = groupYCount;

        if(newGroup == GROUP_X) {
            newState.groupXCount += 1;
        } else if (newGroup == GROUP_Y) {
            newState.groupYCount +=  1;
        }

        return newState;
    }

    // Returns the weight by which the state would be increased by adding the next node to the group
    int getWeightNextGroup(vector<NodeGroup> nodesGroups, NodeGroup newGroup) {
        int weight = 0;
        int newIndex = nodesGroups.size();
        for (int i = 0; i < newIndex; i++) {
            if (nodesGroups[i] != newGroup && nodesGroups[i] != GROUP_UNASSIGNED) {
                weight += inputMatrix->getIndex(i,newIndex);
            }
        }
        return weight;
    }

    // amount of the assigned nodes in hte State
    int getLength() {
        return nodesGroups.size();
    }

    //  returns group assignment to nodes
    vector<NodeGroup> getNodesGroups() {
        return nodesGroups;
    }

protected:
    vector<NodeGroup> nodesGroups;
    InputMatrix * inputMatrix;
    int groupXCount = 0;
    int groupYCount = 0;
    int stateWeight = 0;
};


class SolutionFinder {
public:

    SolutionFinder(InputMatrix * inputMatrix, int aParam, int maxThreads):inputMatrix(inputMatrix),aParam(aParam),maxThreads(maxThreads){};


    bool isStateValid(State &actualState) {

        if(actualState.getGroupCount(GROUP_X) > aParam) {
            return false;
        }

        if(actualState.getGroupCount(GROUP_Y) > inputMatrix->getNodesCount() - aParam) {
            return false;
        }

        // If the weight of the current solution is greater, do not continue in branching.
        if(actualState.getWeight() > foundMinimumWeight) {
            return false;
        }

        // If the min cut weight of the current solution is greater, do not continue in branching.
        if(actualState.getMinCutWeight() > foundMinimumWeight) {
            return false;
        }

        return true;
    }


    void recursiveSearchSeq(State actualState) {

        if(!isStateValid(actualState)) {
            return;
        }

        // When the state has assigned all nodes, stop recursion and replace the best solution if the current is better.
        if(actualState.getLength() == inputMatrix->getNodesCount()) {

            #pragma omp critical
            {
                if (actualState.getWeight() <= foundMinimumWeight) {
                    foundMinimumWeight = actualState.getWeight();
                    foundMinimumState = actualState;
                }
            }
            return;
        }


        recursiveSearchSeq(actualState.getNextState(GROUP_X));
        recursiveSearchSeq(actualState.getNextState(GROUP_Y));

    }

    vector<State> initialBFSExpansion(const State initialState) {

        queue<State> q;
        q.push(initialState);

        while (true) {
            State actualState = q.front();
            q.pop();

            if(actualState.getLength() > 10) {
                break;
            }
            q.push(actualState.getNextState(GROUP_X));
            q.push(actualState.getNextState(GROUP_Y));

        }

        std::vector<State> states;
        while (!q.empty())
        {
            states.emplace_back(std::move(q.front()));
            q.pop();
        }

        return states;
    }

    void findSolution(){
        vector<State> initialStates = initialBFSExpansion(State(inputMatrix));

        #pragma omp parallel for schedule(dynamic) num_threads(maxThreads)
        for (int i = 0; i < initialStates.size(); i++) {
//            std::cout << omp_get_thread_num() <<std::endl;
            recursiveSearchSeq(initialStates[i]);
        }

    }

    void printSolution() {
        cout << "A parameter: " << aParam << endl;
        cout << "Num. threads: " << maxThreads << endl;
        cout << "Min. cut assigment: ";
        for (auto &group : foundMinimumState.getNodesGroups()) {
            cout << (group == GROUP_X) << ' ';
        }
        cout<< endl;

        cout << "Min. cut weight: " << foundMinimumState.getWeight() << endl;
    }

protected:
    InputMatrix * inputMatrix;
    int aParam;
    int maxThreads;
    int foundMinimumWeight = INT_MAX;
    State foundMinimumState = State();

};


int main(int argc, char *argv[]) {

    if (argc <3 || argc >4) {
        std::cerr << "Invalid number of parameters" << std::endl;
        return 1;
    }

    chrono::steady_clock::time_point timeStart(chrono::steady_clock::now());

    string inputFile = argv[1];
    int aParam = stoi(argv[2]);
    int maxThreads = stoi(argv[3]);

    ifstream inputStream = ifstream(inputFile);

    if(inputStream.fail()) {
        std::cerr << "Could not read input file" << std::endl;
        return 1;
    }

    InputMatrix * inputMatrix = readInput(inputStream);

    SolutionFinder solutionFinder = SolutionFinder(inputMatrix, aParam, maxThreads);
    solutionFinder.findSolution();

    chrono::steady_clock::time_point timeEnd(chrono::steady_clock::now());
    solutionFinder.printSolution();
    cout << "Time: " << chrono::duration_cast<chrono::duration<double>>(timeEnd - timeStart).count() << endl;

    delete inputMatrix;
    return 0;
}




