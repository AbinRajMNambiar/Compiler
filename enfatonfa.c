#include <stdio.h>
#include <stdlib.h>

struct Node {
    int state;
    struct Node *next;
};

// --- Function Declarations ---
void findEpsilonClosure(int, int);
void insertTransition(int, char, int);
int getSymbolIndex(char);
void findFinalStates(void);
void unionClosure(int);
void printClosure(int);

// --- Global Variables ---
int numStates, numSymbols, numTransitions;
int startState, numFinalStates;
int finalStates[20];
char symbols[20];
struct Node *transitionTable[20][20] = {NULL};

int eClosure[20][20] = {0}; // epsilon closure for each state
int tempSet[20];             // used for union
int visited[20];             // used in recursion
int counter;                 // used for indexing
int symbolIndex;             // global helper for scanning
int buffer[20];              // for visited tracking in closure

// --- Print epsilon closure {q1,q2,...} ---
void printClosure(int state) {
    printf("{");
    for (int j = 0; eClosure[state][j] != 0; j++) {
        printf("q%d", eClosure[state][j]);
        if (eClosure[state][j + 1] != 0)
            printf(",");
    }
    printf("}");
}

// --- Insert a transition into the table ---
void insertTransition(int from, char symbol, int to) {
    int index = getSymbolIndex(symbol);
    if (index == 999) {
        printf("Error: Invalid symbol\n");
        exit(0);
    }
    struct Node *temp = (struct Node *)malloc(sizeof(struct Node));
    temp->state = to;
    temp->next = transitionTable[from][index];
    transitionTable[from][index] = temp;
}

// --- Return index of the given symbol ---
int getSymbolIndex(char c) {
    for (int i = 0; i < numSymbols; i++)
        if (symbols[i] == c)
            return i;
    return 999;
}

// --- Recursive epsilon closure finder ---
void findEpsilonClosure(int state, int root) {
    if (visited[state]) return; // already visited
    eClosure[root][counter++] = state;
    visited[state] = 1;

    // if 'e' exists and epsilon transitions are present
    if (symbols[numSymbols - 1] == 'e' && transitionTable[state][numSymbols - 1] != NULL) {
        struct Node *temp = transitionTable[state][numSymbols - 1];
        while (temp != NULL) {
            findEpsilonClosure(temp->state, root);
            temp = temp->next;
        }
    }
}

// --- Add all states in e-closure(i) to tempSet ---
void unionClosure(int i) {
    for (int j = 0; eClosure[i][j] != 0; j++)
        tempSet[eClosure[i][j]] = 1;
}

// --- Find final states after removing epsilon ---
void findFinalStates() {
    int printed[20] = {0};
    printf("Final States: {");
    int first = 1;
    for (int i = 1; i <= numStates; i++) {
        for (int j = 0; eClosure[i][j] != 0; j++) {
            for (int k = 0; k < numFinalStates; k++) {
                if (eClosure[i][j] == finalStates[k]) {
                    if (!printed[i]) {
                        if (!first) printf(",");
                        printf("q%d", i);
                        printed[i] = 1;
                        first = 0;
                    }
                }
            }
        }
    }
    printf("}\n");
}

// --- Main Program ---
int main() {
    printf("Enter number of symbols: ");
    scanf("%d", &numSymbols);
    getchar();
    printf("NOTE: use 'e' as epsilon (last symbol if present)\n");
    printf("Enter symbols: ");
    for (int i = 0; i < numSymbols; i++) {
        symbols[i] = getchar();
        getchar();
    }

    printf("Enter number of states: ");
    scanf("%d", &numStates);
    printf("Enter start state: ");
    scanf("%d", &startState);
    printf("Enter number of final states: ");
    scanf("%d", &numFinalStates);
    printf("Enter final states: ");
    for (int i = 0; i < numFinalStates; i++)
        scanf("%d", &finalStates[i]);

    printf("Enter number of transitions: ");
    scanf("%d", &numTransitions);
    printf("Enter transitions (from symbol to):\n");
    for (int i = 0; i < numTransitions; i++) {
        int from, to;
        char sym;
        scanf("%d %c %d", &from, &sym, &to);
        insertTransition(from, sym, to);
    }

    // --- Compute epsilon closures ---
    for (int i = 1; i <= numStates; i++) {
        counter = 0;
        for (int j = 0; j < 20; j++) {
            visited[j] = 0;
            eClosure[i][j] = 0;
        }
        findEpsilonClosure(i, i);
    }

    // --- Print NFA without epsilon ---
    printf("\nEquivalent NFA (without epsilon)\n");
    printf("-----------------------------------\n");
    printf("Start State: ");
    printClosure(startState);
    printf("\nSymbols: ");
    for (int i = 0; i < numSymbols - 1; i++)
        printf("%c ", symbols[i]);

    printf("\nStates:\n");
    for (int i = 1; i <= numStates; i++) {
        printf("q%d = ", i);
        printClosure(i);
        printf("\n");
    }

    printf("\nTransitions:\n");
    for (int i = 1; i <= numStates; i++) {
        for (int s = 0; s < numSymbols - 1; s++) {
            for (int m = 0; m < 20; m++) tempSet[m] = 0;

            for (int k = 0; eClosure[i][k] != 0; k++) {
                int t = eClosure[i][k];
                struct Node *temp = transitionTable[t][s];
                while (temp != NULL) {
                    unionClosure(temp->state);
                    temp = temp->next;
                }
            }

            printClosure(i);
            printf(" --%c--> {", symbols[s]);
            int first = 1;
            for (int n = 1; n <= numStates; n++) {
                if (tempSet[n]) {
                    if (!first) printf(",");
                    printf("q%d", n);
                    first = 0;
                }
            }
            printf("}\n");
        }
    }

    findFinalStates();
    return 0;
}
