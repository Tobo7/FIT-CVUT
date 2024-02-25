#include <iostream>
#include <utility>
#include <fstream>
#include <sstream>
#include <climits>
#include <vector>
#include <queue>
#include <omp.h>
#include <chrono>
#include <mpi.h>
#include <queue>


#define initialBFSDepth 7
#define slaveBFSDepth 17
#define finishSeqWhenRemain 10

using namespace std;

enum NodeGroup { GROUP_X, GROUP_Y, GROUP_UNASSIGNED };
enum MpiTags { WORK, STOP, DONE, RESULT  };

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


// Structure for sharing data between processing with mpi messages
typedef struct Package {
    Package(){};
    int values[100];
    int groupX = 0;
    int groupY = 0;
    int weight = 0;
} Package;



// Class for storing current state of the step
class State {
public:
    State() {
        inputMatrix = nullptr;
    };

    State(InputMatrix * inputMatrix):inputMatrix(inputMatrix) {
        stateWeight = 0;
    };

    State(InputMatrix * inputMatrix, Package p):inputMatrix(inputMatrix) {
        int size = p.groupX + p.groupY;
        stateWeight = p.weight;
        groupXCount = p.groupX;
        groupYCount = p.groupY;
        for (int i = 0; i < size; ++i) {
            nodesGroups.push_back((NodeGroup) p.values[i]);
        }
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

    Package serialize() {
        Package p = Package();
        for(int i = 0; i < nodesGroups.size(); ++i){
            p.values[i] = nodesGroups[i];
        }
        p.groupX = groupXCount;
        p.groupY = groupYCount;
        p.weight = stateWeight;

        return p;
    }

    void print() {
        for (auto &group : nodesGroups) {
            cout << (group == GROUP_X) << ' ';
        }

        cout << " X: " << groupXCount << " Y: " << groupYCount << " W: " << stateWeight << endl;
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

        if(!isStateValidFast(actualState)){
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

    // conditions that can be checked in constant time
    bool isStateValidFast(State &actualState) {

        if(actualState.getGroupCount(GROUP_X) > aParam) {
            return false;
        }

        if(actualState.getGroupCount(GROUP_Y) > inputMatrix->getNodesCount() - aParam) {
            return false;
        }

        return true;
    }


    void updateBestState(State state) {
        foundMinimumWeight = state.getWeight();
        foundMinimumState = state;
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
                    updateBestState(actualState);
                }
            }
            return;
        }

        recursiveSearchSeq(actualState.getNextState(GROUP_X));
        recursiveSearchSeq(actualState.getNextState(GROUP_Y));
    }

    vector<State> initialBFSExpansion(const State initialState, int depth) {

        queue<State> q;
        q.push(initialState);

        while (!q.empty()) {
            State actualState = q.front();
            q.pop();

            if(actualState.getLength() > depth) {
                break;
            }
            State nextX = actualState.getNextState(GROUP_X);
            State nextY = actualState.getNextState(GROUP_Y);

            if(isStateValidFast(nextX)) q.push(nextX);
            if(isStateValidFast(nextY)) q.push(nextY);

        }

        std::vector<State> states;
        while (!q.empty())
        {
            states.emplace_back(std::move(q.front()));
            q.pop();
        }

        return states;
    }

    void master() {
        int num_procs;
        MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

        cout << "[Master] generating initial states" << endl;
        vector<State> initialStates = initialBFSExpansion(State(inputMatrix), initialBFSDepth);
        queue<State> initialStatesQueue;

        for (State &state: initialStates) {
            initialStatesQueue.push(state);
        }

        cout << "[Master] sending initial work to the slaves" << endl;
        for (int dest = 1; dest < num_procs; dest++) {
            Package pack = initialStatesQueue.front().serialize();
            initialStatesQueue.pop();
            MPI_Send(&pack, sizeof(struct Package), MPI_PACKED, dest, MpiTags::WORK, MPI_COMM_WORLD);
        }
        cout << "[Master] sent initial work to the slaves" << endl;

        int workingSlaves = num_procs - 1;

        while (workingSlaves > 0) {
            MPI_Status status;

            cout << "[Master] waiting for done" << endl;
            MPI_Recv(nullptr, 0, MPI_PACKED, MPI_ANY_SOURCE, MpiTags::DONE, MPI_COMM_WORLD, &status);
            cout << "[Master] received done from " << status.MPI_SOURCE << endl;

            if(!initialStatesQueue.empty()) {
                Package pack = initialStatesQueue.front().serialize();
                cout << "[Master] sending next work to the slave " << status.MPI_SOURCE << ": " << endl;
                initialStatesQueue.pop();
                MPI_Send(&pack, sizeof(struct Package), MPI_PACKED, status.MPI_SOURCE, MpiTags::WORK, MPI_COMM_WORLD);
            } else {
                workingSlaves--;
                cout << "[Master] stopping slave " << status.MPI_SOURCE << endl;
                MPI_Send(nullptr, 0, MPI_PACKED, status.MPI_SOURCE, MpiTags::STOP, MPI_COMM_WORLD);

                Package receivedPackage = Package();
                MPI_Recv(&receivedPackage, sizeof(struct Package), MPI_PACKED, status.MPI_SOURCE, MpiTags::RESULT, MPI_COMM_WORLD, &status);
                State receivedState = State(inputMatrix, receivedPackage);
                if (receivedState.getWeight() <= foundMinimumWeight) {
                    cout << "[Master] updating best state from  " << status.MPI_SOURCE << "; value=" << receivedState.getWeight() << endl;
                    updateBestState(receivedState);
                }
            }

        }

        cout << "[Master] no working slaves " << endl;

    }

    void slave() {
        int proc_num;
        MPI_Comm_rank(MPI_COMM_WORLD, &proc_num);

        while (true) {
            MPI_Status status_mpi;
            Package pack;
            cout << "[Slave "<< proc_num <<"] " << proc_num << " waiting for message" <<endl;
            MPI_Recv(&pack, sizeof(struct Package), MPI_PACKED, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status_mpi);
            cout << "[Slave "<< proc_num <<"] " << proc_num << " received message" <<endl;

            if(status_mpi.MPI_TAG == MpiTags::WORK) {
                cout << "[Slave "<< proc_num <<"] " << proc_num << " calculating" << endl;
                State startState = State(inputMatrix, pack);

                vector<State> initialStates = initialBFSExpansion(startState, slaveBFSDepth);

                #pragma omp parallel for schedule(dynamic) num_threads(maxThreads)
                for (int i = 0; i < initialStates.size(); i++) {
                    recursiveSearchSeq(initialStates[i]);
                }

                cout << "[Slave "<< proc_num <<"] " << " work done " << endl;
                MPI_Send(nullptr, 0, MPI_PACKED, 0, MpiTags::DONE, MPI_COMM_WORLD);
            } else {
                Package packRes = foundMinimumState.serialize();
                cout << "[Slave "<< proc_num <<"] " << proc_num << " sending result " << endl;
                MPI_Send(&packRes, sizeof(struct Package), MPI_PACKED, 0, MpiTags::RESULT, MPI_COMM_WORLD);
                cout << "[Slave "<< proc_num <<"] " << proc_num << " Stopping" <<endl;
                return;
            }

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

    MPI_Init(&argc, &argv);

    int proc_num, num_procs; // číslo procesu, počet procesů
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_num);

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

    if (proc_num == 0) {
        chrono::steady_clock::time_point timeStart(chrono::steady_clock::now());
        solutionFinder.master();
        chrono::steady_clock::time_point timeEnd(chrono::steady_clock::now());
        cout << "Time: " << chrono::duration_cast<chrono::duration<double>>(timeEnd - timeStart).count() << endl;
        solutionFinder.printSolution();
    } else {
        solutionFinder.slave();
    }

    MPI_Finalize();

    delete inputMatrix;

    cout << "Process " << proc_num << "exit" << endl;
    return 0;
}




