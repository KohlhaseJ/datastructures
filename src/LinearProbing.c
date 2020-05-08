#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#define TABLE_SIZE 1000

int probingCount;

bool VERBOSE = false;

void insertFCFS(int value, int table[]);
void insertLCFS(int value, int table[]);
void insertRobinHood(int value, int table[]);
int hashfunction(int value);


// ON COLLISION: x stays, y continues
void insertFCFS(int value, int table[]) {
    int key = hashfunction(value);
    for (int i = 0; i < TABLE_SIZE; i++) {
        int index = (key+i)%TABLE_SIZE;
        // already existing?
        if(table[index] == value) {
            break;
        }
        // empty?
        if (table[index] == 0) {
            table[index] = value;
            break;
        }
        probingCount++;
    }
}

// ON COLLISION: y kicks out x, x continues
void insertLCFS(int value, int table[]) {
    int key = hashfunction(value);
    for (int i = 0; i < TABLE_SIZE; i++) {
        int index = (key+i)%TABLE_SIZE;
        // already existing?
        if(table[index] == value) {
            break;
        }
        // kick out x
        int x = table[index];
        table[index] = value;
        value = x;
        // empty?
        if (x == 0) {
            break;
        }
        probingCount++;
    }
}

// ON COLLISION: item furthest away from home slot stays
void insertRobinHood(int value, int table[]) {
    int key = hashfunction(value);
    for (int i = 0; i < TABLE_SIZE; i++) {
        int index = (key+i)%TABLE_SIZE;
        // already existing?
        if(table[index] == value) {
            break;
        }
        // empty?
        if (table[index] == 0) {
            table[index] = value;
            break;  
        }

        // kick out x if it is closer to home slot
        int x = table[index];
        int pslY = abs(index - (hashfunction(value)%TABLE_SIZE));
        int pslX = abs(index - (hashfunction(x)%TABLE_SIZE));

        if(pslX < pslY) {
            table[index] = value;
            value = x;
        }
        
        probingCount++;
    }
}

int search(int value, int table[]) {
    int key = hashfunction(value);
    int returnValue = -1;
    for (int i = 0; i < TABLE_SIZE; i++) {
        int index = (key+i)%TABLE_SIZE;
        if (table[index] == value) {
            returnValue = index;
            break;
        }
        probingCount++;
    }
    return returnValue;
}

int searchRobinHood(int value, int table[]) {
    int key = hashfunction(value);
    int returnValue = -1;
    for (int i = 0; i < TABLE_SIZE; i++) {
        int index = (key+i)%TABLE_SIZE;
        if (table[index] == value) {
            returnValue = index;
            break;
        }
        // special search: we can include the psl into the search for fasten up unsuccesful searches
        int x = table[index];
        int pslY = abs(index - (hashfunction(value)%TABLE_SIZE));
        int pslX = abs(index - (hashfunction(x)%TABLE_SIZE));
        if(pslX < pslY) {
            break;
        }

        probingCount++;
    }
    return returnValue;
}

int hashfunction(int value) {
    return value % TABLE_SIZE;
}


int main(int argc, char** argv) {
    FILE *fp;

    // delete existing output files
    remove("../out/FCFS_insertion.txt");
    remove("../out/LCFS_insertion.txt");
    remove("../out/RobinHood_insertion.txt");
    remove("../out/FCFS_search.txt");
    remove("../out/LCFS_search.txt");
    remove("../out/RobinHood_search.txt");

    // experiments: insertion and search cost in hash table
    // - 10 trials
    // - build table by FCFS, LCFS, RH
    // - different load factors 0.1, 0.2, ..., 1    
    int randoms[TABLE_SIZE];
    int tableFC[TABLE_SIZE];
    int tableLC[TABLE_SIZE];
    int tableRH[TABLE_SIZE];
    for (int t = 0; t < 10; t++) {
        memset(randoms, 0, sizeof(randoms));
        memset(tableFC, 0, sizeof(tableFC));
        memset(tableLC, 0, sizeof(tableLC));
        memset(tableRH, 0, sizeof(tableRH));
        // generate random values
        srand((int)time(NULL));
        for (int i = 0; i < TABLE_SIZE; i++) {
            randoms[i] = rand();
        }

        for (int i = 0; i < TABLE_SIZE; i+=(TABLE_SIZE/20)) {
            int n = i+(TABLE_SIZE/20);
            double loadingFactor = n/(double)TABLE_SIZE;
            // insert next 10 percent of load
            for (int j = i; j < n; j++) {
                probingCount = 0;
                insertFCFS(randoms[j], tableFC);
                fp = fopen("../out/FCFS_insertion.txt", "a");
                fprintf(fp, "%d, %f, %d\n", t, loadingFactor, probingCount);
                fclose(fp);
                probingCount = 0;
                insertLCFS(randoms[j], tableLC);
                fp = fopen("../out/LCFS_insertion.txt", "a");
                fprintf(fp, "%d, %f, %d\n", t, loadingFactor, probingCount);
                fclose(fp);
                probingCount = 0;
                insertRobinHood(randoms[j], tableRH);
                fp = fopen("../out/RobinHood_insertion.txt", "a");
                fprintf(fp, "%d, %f, %d\n", t, loadingFactor, probingCount);
                fclose(fp);
            }
            // search for all existing values
            for (int j = 0; j < n; j++) {
                probingCount = 0;
                search(randoms[j], tableFC);
                fp = fopen("../out/FCFS_search.txt", "a");
                fprintf(fp, "%d, %f, %d\n", t, loadingFactor, probingCount);
                fclose(fp);
                probingCount = 0;
                search(randoms[j], tableLC);
                fp = fopen("../out/LCFS_search.txt", "a");
                fprintf(fp, "%d, %f, %d\n", t, loadingFactor, probingCount);
                fclose(fp);
                probingCount = 0;
                search(randoms[j], tableRH);
                fp = fopen("../out/RobinHood_search.txt", "a");
                fprintf(fp, "%d, %f, %d\n", t, loadingFactor, probingCount);
                fclose(fp);

            }
        }
    }
}