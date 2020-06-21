#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <time.h>

// HEADER
typedef struct node Node;

struct node {
    int u;
    int min;
    int max;
    Node *summary;
    Node **clusters;
};

int operationCounter;


void freeVEB (Node *root) {
    free(root->summary);
    free(root->clusters);
    free(root);
}

int getMin (Node *root) {
    return root->min;
}

Node* insert(int x, Node *root, int size) {
    operationCounter++;
    if (root == NULL) {
        root = (Node*)malloc(sizeof(Node));
        root->u = size;
        root->min = root->max = x;

        if(size <= 2) {
            root->summary = NULL;
            root->clusters = NULL;
        } else {
            root->summary = NULL;
            root->clusters = (Node**)calloc((long)sqrt(root->u), sizeof(Node*));
        }
    } else {
        if(x < root->min) {
            int aux = root->min;
            root->min = x;
            x = aux;
        }
        
        if (x > root->max) {
            root->max = x;
        }
        
        if(size > 2){
            int hi = floor(x / sqrt(root->u));
            int lo = x % (long)sqrt(root->u);

            if(root->clusters[hi] == NULL){
                root->summary = insert(hi, root->summary, sqrt(root->u));
            }
            root->clusters[hi] = insert(lo, root->clusters[hi], sqrt(root->u));
        }
    }

    return root;
}

Node* delete(int x, Node *root) {
    operationCounter++;
    if (root == NULL) {
        return NULL;
    }
    
    if (x == root->min && x == root->max) {
        freeVEB(root);
        return NULL;
    }

    if (root->u <= 2) {
        root->max = root->min = (x == 0 ? 1 : 0);
    } else {
        if (x == root->min) {
            if(root->summary == NULL) {
                root->min = root->max;
                return root;
            }
            int i = root->summary->min;
            root->min = i*sqrt(root->u) + root->clusters[i]->min;
            x = root->min;
        }

        if (x == root->max) {
            if(root->summary == NULL) {
                root->max = root->min;
                return root;
            }
            int i = root->summary->max;
            root->max = i*sqrt(root->u) + root->clusters[i]->max;
            x = root->max;
        }

        int hi = floor(x / sqrt(root->u));
        int lo = x % (long)sqrt(root->u);

        root->clusters[hi] = delete(lo, root->clusters[hi]);
        if(root->clusters[hi] == NULL) {
            root->summary = delete(hi, root->summary);
        }
    }

    return root;
}


int main(int argc, char** argv) {
    // experimental setup
    int h[4] = {1,2,3,4};
    for (int i = 0; i < (sizeof(h)/sizeof(h[0])); i++) {
        int k = pow(2, h[i]);
        int n = pow(2, k);

        Node *root = NULL;

        int values[n];
        for (int j = 0; j < n; j++) {
            values[j] = j;
        }
        // shuffle array
        for (int j = 0; j < n; j++) {
            int temp = values[j];
            int rj = rand() % n;
            values[j] = values[rj];
            values[rj] = temp;
        }
        // insertion
        float operationAverage = 0;
        int operationMin = INT_MAX;
        int operationMax = 0;

        for (int j = 0; j < n; j++) {
            int x = values[j];
            operationCounter = 0;
            root = insert(x, root, n);
            operationAverage += operationCounter;
            if (operationMax < operationCounter) {
                operationMax = operationCounter;
            }
            if (operationMin > operationCounter) {
                operationMin = operationCounter;
            }
        }
        operationAverage = operationAverage / (float)n;
        printf("%d & %d & %d & %f \\\\\n", n, operationMin, operationMax, operationAverage);

        // deletion
        operationAverage = 0;
        operationMin = INT_MAX;
        operationMax = 0;

        for (int j = 0; j < n; j++) {
            operationCounter = 0;
            root = delete(j, root);
            operationAverage += operationCounter;
            if (operationMax < operationCounter) {
                operationMax = operationCounter;
            }
            if (operationMin > operationCounter) {
                operationMin = operationCounter;
            }
        }
        operationAverage = operationAverage / (float)n;
        printf("%d & %d & %d & %f \\\\\n", n, operationMin, operationMax, operationAverage);

        //freeVEB(root);
    }
}