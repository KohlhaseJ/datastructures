#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
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
int insertionSteps;
int querySteps;
int nodeCount = 0;
int leafCount = 0;
int height = 0;

// functions
Node* insert(Point *p, Node *v, Node *w);
Point* search(Point *z, Node *v);
Point* naiveSearch(Point *query, Point *a[], int n);
Point* downtown(Node *v);
Point* uptown(Node *v);
float distance(Point *p, Point *q);
enum movementDecision decision(float dL, float dR, Node *v);
void traverse(Node *root);
void updateStatistics(Node *root, int currentHeight);


// IMPLEMENTATION
Node* insert(Point *p, Node *v, Node *w) {  
    // increase step counter  
    insertionSteps++;

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

Point* naiveSearch(Point *query, Point *a[], int n) {
    // init global search parameters
    z = query;
    q = NULL;
    NDIST = DBL_MAX;
    // search closest
    for(int i = 0; i < n; i++){
        float dist = distance(a[i], z);
        if(dist < NDIST) {
            NDIST = dist;
            q = a[i];
        }
    }
    return q;
}

Point* downtown(Node *v) {
    // increase step counter
    querySteps++;

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
    // increase step counter
    querySteps++;
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

void updateStatistics(Node *root, int currentHeight) {
    nodeCount++;
    if (root->full == false) {
        leafCount++;
        if (height < currentHeight) height = currentHeight;
        return;
    }
    if (root->lChild != NULL) {
        updateStatistics(root->lChild, currentHeight + 1);
    }
    if(root->rChild != NULL) {
        updateStatistics(root->rChild, currentHeight + 1);
    }
}

int randomIntegerIn(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower; 
}

int main(int argc, char** argv) {
    // experimental setup
    const int SIZE = 512;
    const int LOWER_BOUND = 0;
    const int UPPER_BOUND = 100;
    const int TIMES = 10;

    // init step counters
    int avgInsertionSteps = 0;
    int minInsertionSteps = INT_MAX;
    int maxInsertionSteps = 0;
    int avgQuerySteps = 0;
    int minQuerySteps = INT_MAX;
    int maxQuerySteps = 0;

    // init tree statistics
    int avgNodeCount = 0;
    int minNodeCount = INT_MAX;
    int maxNodeCount = 0;
    int avgLeafCount = 0;
    int minLeafCount = INT_MAX;
    int maxLeafCount = 0;
    int avgHeight = 0;
    int minHeight = INT_MAX;
    int maxHeight = 0;

    // init timing stats
    double naiveAvgCreationTime = 0;
    double naiveMinCreationTime = DBL_MAX;
    double naiveMaxCreationTime = 0;
    double naiveAvgQueryTime = 0;
    double naiveMinQueryTime = DBL_MAX;
    double naiveMaxQueryTime = 0;
    double bisectorAvgCreationTime = 0;
    double bisectorMinCreationTime = DBL_MAX;
    double bisectorMaxCreationTime = 0;
    double bisectorAvgQueryTime = 0;
    double bisectorMinQueryTime = DBL_MAX;
    double bisectorMaxQueryTime = 0;    
    
    printf("EXPERIMENTAL SETUP:\n");
    printf("---------------------\n");
    printf("%d points (values randomly drawn)\n", SIZE);
    printf("Interval [%d, %d]\n", LOWER_BOUND, UPPER_BOUND);
    printf("Tested for %d trees\n\n", TIMES);

    // run experiments
    clock_t start, end;
    // construction of TIMES trees
    for(int i = 0; i < TIMES; i++) {
        Node *root = NULL;
        Point *a[(sizeof (Point)*SIZE)];
        // draw randoms
        start = clock();
        for (int i = 0; i < SIZE; i++) {
            Point *p = malloc(sizeof(Point));
            p->x = randomIntegerIn(LOWER_BOUND, UPPER_BOUND);
            p->y = randomIntegerIn(LOWER_BOUND, UPPER_BOUND);
            a[i] = p;
        }
        end = clock();
        double creationTime = ((double) (end - start)) / (CLOCKS_PER_SEC/1000);
        if(creationTime < naiveMinCreationTime) naiveMinCreationTime = creationTime;
        if(creationTime > naiveMaxCreationTime) naiveMaxCreationTime = creationTime;
        naiveAvgCreationTime += creationTime;
        
        // fill tree
        insertionSteps = 0;
        start = clock();

        for (int i = 0; i < SIZE; i++) {
            root = insert(a[i], root, NULL);
        }
        end = clock();
        creationTime = ((double) (end - start)) / (CLOCKS_PER_SEC/1000);
        if(creationTime < bisectorMinCreationTime) bisectorMinCreationTime = creationTime;
        if(creationTime > bisectorMaxCreationTime) bisectorMaxCreationTime = creationTime;
        bisectorAvgCreationTime += creationTime;
        if(insertionSteps < minInsertionSteps) minInsertionSteps = insertionSteps;
        if(insertionSteps > maxInsertionSteps) maxInsertionSteps = insertionSteps;
        avgInsertionSteps += insertionSteps;

        // tree statistics        
        nodeCount = 0;
        leafCount = 0;
        height = 0;
        updateStatistics(root, 0);
        if(nodeCount < minNodeCount) minNodeCount = nodeCount;
        if(nodeCount > maxNodeCount) maxNodeCount = nodeCount;
        avgNodeCount += nodeCount;
        if(leafCount < minLeafCount) minLeafCount = leafCount;
        if(leafCount > maxLeafCount) maxLeafCount = leafCount;
        avgLeafCount += leafCount;
        if(height < minHeight) minHeight = height;
        if(height > maxHeight) maxHeight = height;
        avgHeight += height;

        // run experiments
        // search in and outside the interval
        for (int x = LOWER_BOUND-(UPPER_BOUND-LOWER_BOUND); x <= UPPER_BOUND+(UPPER_BOUND-LOWER_BOUND); x++) {
            for (int y = LOWER_BOUND-(UPPER_BOUND-LOWER_BOUND); y <= UPPER_BOUND+(UPPER_BOUND-LOWER_BOUND); y++) {
                Point *query = malloc(sizeof(Point));
                query->x = x;
                query->y = y;
                Point *found = NULL;
                querySteps = 0;

                // naive search
                start = clock();
                found = naiveSearch(query, a, SIZE);
                end = clock();
                float queryTime = ((double) (end - start)) / (CLOCKS_PER_SEC/1000);
                if (queryTime < naiveMinQueryTime) naiveMinQueryTime = queryTime;
                if (queryTime > naiveMaxQueryTime) naiveMaxQueryTime = queryTime;
                naiveAvgQueryTime += queryTime;
                
                // tree search
                start = clock();
                found = search(query, root);
                end = clock();
                queryTime = ((double) (end - start)) / (CLOCKS_PER_SEC/1000);
                if (queryTime < bisectorMinQueryTime) bisectorMinQueryTime = queryTime;
                if (queryTime > bisectorMaxQueryTime) bisectorMaxQueryTime = queryTime;
                bisectorAvgQueryTime += queryTime;
                if(querySteps < minQuerySteps) minQuerySteps = querySteps;
                if(querySteps > maxQuerySteps) maxQuerySteps = querySteps;
                avgQuerySteps += querySteps;
            }
        }

        free(a);
        free(root);
    }
    // calc averages
    naiveAvgCreationTime = naiveAvgCreationTime / TIMES;
    bisectorAvgCreationTime = bisectorAvgCreationTime / TIMES;
    avgInsertionSteps = avgInsertionSteps / TIMES;
    avgNodeCount = avgNodeCount / TIMES;
    avgLeafCount = avgLeafCount / TIMES;
    avgHeight = avgHeight / TIMES;
    int n = (UPPER_BOUND+(UPPER_BOUND-LOWER_BOUND))-(LOWER_BOUND-(UPPER_BOUND-LOWER_BOUND));
    naiveAvgQueryTime = naiveAvgQueryTime / (n*n*TIMES);
    bisectorAvgQueryTime = bisectorAvgQueryTime / (n*n*TIMES);
    avgQuerySteps = avgQuerySteps / (n*n*TIMES);
    
    printf("EXPERIMENTAL RESULTS:\n");
    printf("---------------------\n");
    printf("Naive search:\n");
    printf("   Construction time: %f (%f, %f)\n", naiveAvgCreationTime, naiveMinCreationTime, naiveMaxCreationTime);
    printf("   Construction steps: %d\n", SIZE);
    printf("   Query time: %f (%f, %f)\n", naiveAvgQueryTime, naiveMinQueryTime, naiveMaxQueryTime);
    printf("   Query steps: %d\n\n", SIZE);
    printf("Bisector tree search:\n");
    printf("   Construction time: %f (%f, %f)\n", bisectorAvgCreationTime, bisectorMinCreationTime, bisectorMaxCreationTime);
    printf("   Construction steps: %d (%d, %d)\n", avgInsertionSteps, minInsertionSteps, maxInsertionSteps);
    printf("   Query time: %f (%f, %f)\n", bisectorAvgQueryTime, bisectorMinQueryTime, bisectorMaxQueryTime);
    printf("   Query steps: %d (%d, %d)\n\n", avgQuerySteps, minQuerySteps, maxQuerySteps);
    printf("Bisector tree statistics:\n");
    printf("   Node count: %d (%d, %d)\n", avgNodeCount, minNodeCount, maxNodeCount);
    printf("   Leaf count: %d (%d, %d)\n", avgLeafCount, minLeafCount, maxLeafCount);
    printf("   Height: %d (%d, %d)\n\n", avgHeight, minHeight, maxHeight);
    
    return (EXIT_SUCCESS);
}