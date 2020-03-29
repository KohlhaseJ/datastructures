#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <float.h>

// HEADER
enum movementDecision {LEFT, LEFT_FLAGGED, RIGHT, RIGHT_FLAGGED, UP};

typedef struct point Point;

struct point {
    int x;
    int y;
};

typedef struct tempStore TempStore;

struct tempStore {
    float dL;
    float dR;
    bool wentRight;
};

typedef struct node Node;

struct node {
    Point *pL;
    Point *pR;
    Node *lChild;
    Node *rChild;
    Node *ancestor;
    float lRadius;
    float rRadius;
    bool full;
    TempStore *temp;
};

// globals
bool VERBOSE = false;
float NDIST = 0.0f;
Point *z = NULL;
Point *q = NULL;

// functions
Node* insert(Point *p, Node *v, Node *w);
Point* search(Point *z, Node *v);
Point* downtown(Node *v);
Point* uptown(Node *v);
float distance(Point *p, Point *q);
enum movementDecision decision(float dL, float dR, Node *v);
void traverse(Node *root);
void print(Node *root);


// IMPLEMENTATION
Node* insert(Point *p, Node *v, Node *w) {
    if (v == NULL) {
        // create new node
        v = malloc(sizeof (Node));
        v->pL = p;
        v->full = false;
        v->ancestor = w;
        v->pR = NULL;
        v->lChild = NULL;
        v->rChild = NULL;
        v->lRadius = 0.0f;
        v->rRadius = 0.0f;
        v->temp = NULL;
    } else if (v->full == false) {
        // fill node
        v->pR = p;
        v->full = true;
    } else {
        float dL = distance(v->pL, p);
        float dR = distance(v->pR, p);
        if (dL < dR) {
            // go left
            if (dL > v->lRadius) {
                v->lRadius = dL;
            }
            v->lChild = insert(p, v->lChild, v);
        } else {
            // go right
            if (dR > v->rRadius) {
                v->rRadius = dR;
            }
            v->rChild = insert(p, v->rChild, v);
        }
    }

    return v;    
}

Point* search(Point *query, Node *v) {
    // init global search parameters
    z = query;
    q = NULL;
    NDIST = 0.0f;
    // move down the tree
    return downtown(v);
}

Point* downtown(Node *v) {
    if(v->ancestor == NULL) {
        float dL = distance(v->pL, z);
        float dR = distance(v->pR, z);
        if(dL < dR) {
            q = v->pL;
            NDIST = dL;
        } else {
            q = v->pR;
            NDIST = dR;
        }
        if (VERBOSE) printf("Updated nearest: %d, %d\n",q->x, q->y);
    }

    if(v->full == false) {
        float dL = distance(v->pL, z);
        if(dL < NDIST) {
            NDIST = dL;
            q = v->pL;
            if (VERBOSE) printf("Updated nearest: %d, %d\n",q->x, q->y);            
        }
        if (VERBOSE) printf("U: reached leaf\n");
        return uptown(v->ancestor);
    } else {
        float dL = distance(v->pL, z);
        float dR = distance(v->pR, z);

        if (dL < dR && dL < NDIST) {
            NDIST = dL;
            q = v->pL;
            if (VERBOSE) printf("Updated nearest: %d, %d\n",q->x, q->y);
        } else if (dL >= dR && dR < NDIST) {
            NDIST = dR;
            q = v->pR;
            if (VERBOSE) printf("Updated nearest: %d, %d\n",q->x, q->y);
        }

        enum movementDecision direction = decision(dL, dR, v);
        
        if (direction == UP) {
            if (VERBOSE) printf("U: by decision\n");
            return uptown(v->ancestor);            
        } else if (direction == LEFT || direction == LEFT_FLAGGED) {
            if (direction == LEFT_FLAGGED) {
                v->temp = malloc(sizeof(TempStore));
                v->temp->dL = dL;
                v->temp->dR = dR;
                v->temp->wentRight = false;
            }
            if (VERBOSE) printf("L: by decision\n");
            return downtown(v->lChild);
        } else { // if (direction == RIGHT || direction == RIGHT_FLAGGED)
            if (direction == RIGHT_FLAGGED) {
                v->temp = malloc(sizeof(TempStore));
                v->temp->dL = dL;
                v->temp->dR = dR;
                v->temp->wentRight = true;
            }
            
            if (VERBOSE) printf("R: by decision\n");
            return downtown(v->rChild);
        }
    }
}

Point* uptown(Node *v) {
    if (v == NULL) {
        return q;
    }

    if (v->temp == NULL) {
        if (VERBOSE) printf("U: no temp storage\n");
        return uptown(v->ancestor);
    } else {
        float dL = v->temp->dL;
        float dR = v->temp->dR;
        bool wentRight = v->temp->wentRight;
        free(v->temp);
        v->temp = NULL;

        if((wentRight && (dL - v->lRadius) >= NDIST)
        || (!wentRight && (dR - v->rRadius) >= NDIST)) {
            // NDIST gotten smaller since last visit, move on upwards
            if (VERBOSE) printf("U: revisited\n");
            return uptown(v->ancestor);
        } else if (wentRight) {
            // check left subtree
            if (VERBOSE) printf("L: revisited\n");
            return downtown(v->lChild);
        } else {
            // check right subtree
            if (VERBOSE) printf("R: revisited\n");
            return downtown(v->rChild);
        }
    }
}

float distance(Point *p, Point *q) {
    // euclidean distance
    int dX = (q->x)-(p->x);
    int dY = (q->y)-(p->y);
    return (float)sqrt(dX*dX + dY*dY);
}

enum movementDecision decision(float dL, float dR, Node *v) {
    if ((dL - v->lRadius) < NDIST
        && (dR - v->rRadius) >= NDIST) {
        return LEFT;
    } else if ((dL - v->lRadius) < NDIST
        && (dR - v->rRadius) < NDIST) {
        if(dR <= dL) {
            return RIGHT_FLAGGED;
        } else {
            return LEFT_FLAGGED;
        }
    } else if ((dL - v->lRadius) >= NDIST
        && (dR - v->rRadius) < NDIST) {
        return RIGHT;
    } else {
        return UP;
    } 
}

void print(Node *root) { 
    if (root->lChild == NULL && root->rChild == NULL) {
        if(root->pR == NULL) {
            printf("(%d,%d/NIL)", root->pL->x, root->pL->y);
        } else {            
            printf("(%d,%d/%d,%d)", root->pL->x, root->pL->y, root->pR->x, root->pR->y);
        }
        return;
    }

    if (root->lChild != NULL) {
        print(root->lChild);
        printf(" - ");
    }
    if(root->pR == NULL) {
        printf("(%d,%d/NIL)", root->pL->x, root->pL->y);
    } else {            
        printf("(%d,%d/%d,%d)", root->pL->x, root->pL->y, root->pR->x, root->pR->y);
    }
    if (root->rChild != NULL) {
        printf(" - ");
        print(root->rChild);
    }
}

int randomIntegerIn(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower; 
}

int main(int argc, char** argv) {
    // experimental setup
    const int SIZE = 4096;
    const int LOWER_BOUND = 0;
    const int UPPER_BOUND = 100;
    const int TIMES = 1000;
    
    printf("EXPERIMENTAL SETUP:\n%d nodes (values randomly drawn)\nInterval [%d, %d]\n", SIZE, LOWER_BOUND, UPPER_BOUND);
    printf("Query all possible values from interval.\n\n");

    // init timing stats
    double creationTime = 0;
    double avgQueryTime = 0;
    double minQueryTime = DBL_MAX;
    double maxQueryTime = 0;

    // run experiments
    // build tree
    Node *root = NULL;
    Point *a[(sizeof (Point)*SIZE)];
    clock_t start, end;

    // draw randoms
    for (int i = 0; i < SIZE; i++) {
        Point *p = malloc(sizeof(Point));
        p->x = randomIntegerIn(LOWER_BOUND, UPPER_BOUND);
        p->y = randomIntegerIn(LOWER_BOUND, UPPER_BOUND);
        a[i] = p;
    }
    
    // fill tree
    start = clock();

    for (int i = 0; i < SIZE; i++) {
        root = insert(a[i], root, NULL);
    }

    free(a);
    end = clock();
    creationTime = ((double) (end - start)) / (CLOCKS_PER_SEC/1000);

    // run queries
    for (int x = LOWER_BOUND; x <= UPPER_BOUND; x++) {
        for (int y = LOWER_BOUND; y <= UPPER_BOUND; y++) {
            Point *query = malloc(sizeof(Point));
            query->x = x;
            query->y = y;
            
            start = clock();
            
            Point *found = NULL;
            found = search(query, root);

            end = clock();
            float queryTime = ((double) (end - start)) / (CLOCKS_PER_SEC/1000);
            if (queryTime < minQueryTime) minQueryTime = queryTime;
            if (queryTime > maxQueryTime) maxQueryTime = queryTime;
            avgQueryTime += queryTime;
        }
    }
    avgQueryTime = avgQueryTime / ((UPPER_BOUND-LOWER_BOUND)*(UPPER_BOUND-LOWER_BOUND));
   
    free(root);
    
    printf("EXPERIMENTS FINISHED:\n");
    printf("Creation time: %f ms\n", creationTime);
    printf("Average query time: %f ms\n", avgQueryTime);
    printf("Minimum query time: %f ms\n", minQueryTime);
    printf("Maximum query time: %f ms\n", maxQueryTime);
    return (EXIT_SUCCESS);
}