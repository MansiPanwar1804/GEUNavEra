#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>  // <-- Added for timing
#include <windows.h>//<--Memory Utilization
#include <stdbool.h>
#define A 6378137.0
#define F 1 / 298.257223563
#define B (A * (1 - F))
#define V 28
#define INF 9999999
#define THRESHOLD 10000
#define WALKING_SPEED 1.4
#define STEP_LENGTH 0.8
#define MAX_NODES 28

typedef struct
{
    char name[50];
    char shortName[20];
    double lat;
    double lon;
} Node;

typedef struct
{
    int index;
    double fScore;
} PQNode;

typedef struct
{
    PQNode data[V];
    int size;
} PriorityQueue;


Node nodes[V] =
{
    {"CSIT Block", "csit", 30.268781, 77.993598},
    {"Badminton Court", "badminton", 30.266824, 77.994993},
    {"B.Tech Block", "btech", 30.267528, 77.995130},
    {"Shantos Library", "library", 30.267410, 77.995693},
    {"Civil Block", "civil", 30.266824, 77.995543},
    {"Param Block", "param", 30.267394, 77.996095},
    {"Aryabhatt Lab", "aryabhatt", 30.267690, 77.995779},
    {"Petroleum Block", "petroleum", 30.267412, 77.996584},
    {"Chanakya Block", "chanakya", 30.267496, 77.997018},
    {"K.P Nautiyal Block", "kp", 30.267991, 77.996653},
    {"Priyadrashani Hostel", "priyadrashani", 30.268752, 77.996058},
    {"Dispensary", "dispensary", 30.268529, 77.994969},
    {"Caf� & Gym", "cafe", 30.267139, 77.994612},
    {"GEU Bus Stop", "busstop", 30.270921, 77.995932},
    {"Tuck Shop", "tuckshop", 30.268475, 77.995090},
    {"Graphic Era Hospital", "hospital", 30.269233, 77.994856},
    {"Vishwakarma Block", "vishwakarma", 30.268058, 77.996251},
    {"Gate No 1", "gate1", 30.268658, 77.994760},
    {"Quick Bite", "quickbite", 30.267901, 77.996417},
    {"Parking", "parking", 30.268313, 77.997160},
    {"Bus Stop Gate 2", "gate2 busstop", 30.268223, 77.997658},
    {"Gate No 2", "gate 2", 30.268712, 77.997871},
    {"Lakshmi Bai Hostel", "lakshmi bai", 30.267723, 77.993601},
    {"Char Khamba", "char khamba", 30.270050, 77.996573},
    {"Marina Hostel", "marina", 30.272040, 77.999548},
    {"GEHU Ground", "gehu ground", 30.272689, 78.000127},
    {"GEHU", "gehu", 30.273171, 78.000009},
    {"Ravi canteen", "ravi canteen", 30.267231, 77.994454}
};

// Windows memory snapshot
void getMemorySnapshot(SIZE_T* memKB)
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    if (GlobalMemoryStatusEx(&statex))
    {
        *memKB = (SIZE_T)(statex.ullAvailPhys / 1024);  // Available RAM in KB
    }
    else
    {
        *memKB = 0;
    }
}



double toRadians(double degree)
{
    return degree * (M_PI / 180.0);
}

double vincenty(Node a, Node b)
{
    double U1 = atan((1 - F) * tan(toRadians(a.lat)));
    double U2 = atan((1 - F) * tan(toRadians(b.lat)));
    double L = toRadians(b.lon - a.lon);
    double lambda = L, lambda_prev;
    int iterations = 100;

    double sinSigma, cosSigma, sigma, sinAlpha, cos2Alpha, C;

    do
    {
        double sinLambda = sin(lambda), cosLambda = cos(lambda);
        double sinU1 = sin(U1), cosU1 = cos(U1);
        double sinU2 = sin(U2), cosU2 = cos(U2);

        sinSigma = sqrt(pow(cosU2 * sinLambda, 2) +
                        pow(cosU1 * sinU2 - sinU1 * cosU2 * cosLambda, 2));
        if (sinSigma == 0) return 0;

        cosSigma = sinU1 * sinU2 + cosU1 * cosU2 * cosLambda;
        sigma = atan2(sinSigma, cosSigma);
        sinAlpha = cosU1 * cosU2 * sinLambda / sinSigma;
        cos2Alpha = 1 - sinAlpha * sinAlpha;
        C = (F / 16) * cos2Alpha * (4 + F * (4 - 3 * cos2Alpha));

        lambda_prev = lambda;
        lambda = L + (1 - C) * F * sinAlpha *
                 (sigma + C * sinSigma * (cosSigma + C * cosSigma * (-1 + 2 * cosSigma * cosSigma)));
    }
    while (fabs(lambda - lambda_prev) > 1e-12 && --iterations);

    double u2 = cos2Alpha * (A * A - B * B) / (B * B);
    double A_coeff = 1 + (u2 / 16384) * (4096 + u2 * (-768 + u2 * (320 - 175 * u2)));
    double B_coeff = (u2 / 1024) * (256 + u2 * (-128 + u2 * (74 - 47 * u2)));

    double deltaSigma = B_coeff * sinSigma *
                        (cosSigma + (B_coeff / 4) * (cos(2 * sigma) - 3 * sinSigma * sinSigma));

    return B * A_coeff * (sigma - deltaSigma);
}

int findNodeByName(char *name)
{
    for (int i = 0; i < V; i++)
    {
        if (strcasecmp(name, nodes[i].name) == 0 || strcasecmp(name, nodes[i].shortName) == 0)
            return i;
    }
    return -1;
}

void reconstructPath(int src, int dest, int next[V][V], char *url, int distance, double timeRequired)
{
    strcat(url, "https://www.openstreetmap.org/directions?engine=fossgis_osrm_foot&route=");
    char buffer[100];

    int u = src;
    while (u != dest)
    {
        sprintf(buffer, "%lf,%lf;", nodes[u].lat, nodes[u].lon);
        strcat(url, buffer);
        u = next[u][dest];
    }
    sprintf(buffer, "%lf,%lf", nodes[dest].lat, nodes[dest].lon);
    strcat(url, buffer);
    sprintf(buffer, "#map=19/%lf/%lf&distance=%d&time=%.2f", nodes[src].lat, nodes[src].lon, distance, timeRequired);
    strcat(url, buffer);
}

// Floyd War-shall Algorithm
void floydWarshall(int graph[V][V], int next[V][V])
{
    for (int k = 0; k < V; k++)
        for (int i = 0; i < V; i++)
            for (int j = 0; j < V; j++)
                if (graph[i][k] + graph[k][j] < graph[i][j])
                {
                    graph[i][j] = graph[i][k] + graph[k][j];
                    next[i][j] = next[i][k];
                }
}

// Dijkstra Algorithm
void dijkstra(int src, int dest, int *path, int *pathLength, double *totalDist)
{
    double dist[V];
    int visited[V] = {0}, prev[V];
    for (int i = 0; i < V; i++)
    {
        dist[i] = INF;
        prev[i] = -1;
    }
    dist[src] = 0;

    for (int i = 0; i < V - 1; i++)
    {
        double min = INF;
        int u = -1;
        for (int j = 0; j < V; j++)
            if (!visited[j] && dist[j] < min)
            {
                min = dist[j];
                u = j;
            }
        if (u == -1) break;

        visited[u] = 1;

        for (int v = 0; v < V; v++)
        {
            if (!visited[v] && u != v)
            {
                double d = vincenty(nodes[u], nodes[v]);
                if (d < THRESHOLD && dist[u] + d < dist[v])
                {
                    dist[v] = dist[u] + d;
                    prev[v] = u;
                }
            }
        }
    }

    if (dist[dest] == INF)
    {
        *pathLength = 0;
        *totalDist = INF;
        return;
    }

    int count = 0;
    for (int at = dest; at != -1; at = prev[at]) path[count++] = at;
    for (int i = 0; i < count / 2; i++)
    {
        int tmp = path[i];
        path[i] = path[count - i - 1];
        path[count - i - 1] = tmp;
    }

    *pathLength = count;
    *totalDist = dist[dest];
}


// Priority queue functions for A*

void pqInsert(PriorityQueue *pq, int index, double fScore)
{
    int i = pq->size++;
    while (i > 0 && pq->data[(i - 1) / 2].fScore > fScore)
    {
        pq->data[i] = pq->data[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    pq->data[i].index = index;
    pq->data[i].fScore = fScore;
}

int pqExtractMin(PriorityQueue *pq)
{
    int min = pq->data[0].index;
    PQNode last = pq->data[--pq->size];
    int i = 0, child;
    while (i * 2 + 1 < pq->size)
    {
        child = i * 2 + 1;
        if (child + 1 < pq->size && pq->data[child + 1].fScore < pq->data[child].fScore)
            child++;
        if (last.fScore <= pq->data[child].fScore)
            break;
        pq->data[i] = pq->data[child];
        i = child;
    }
    pq->data[i] = last;
    return min;
}




void reconstructAStarPath(int cameFrom[], int current, Node nodes[V], char *url, int distance, double timeRequired)
{
    char buffer[100];
    strcat(url, "https://www.openstreetmap.org/directions?engine=fossgis_osrm_foot&route=");
    int path[V], count = 0;
    while (current != -1)
    {
        path[count++] = current;
        current = cameFrom[current];
    }
    for (int i = count - 1; i >= 0; i--)
    {
        sprintf(buffer, "%lf,%lf;", nodes[path[i]].lat, nodes[path[i]].lon);
        strcat(url, buffer);
    }
    url[strlen(url) - 1] = '\0'; // remove trailing semicolon
    sprintf(buffer, "#map=19/%lf/%lf&distance=%d&time=%.2f", nodes[path[count - 1]].lat, nodes[path[count - 1]].lon, distance, timeRequired);
    strcat(url, buffer);
}

int aStar(int graph[V][V], Node nodes[V], int start, int goal, int *totalDist, int cameFrom[V])
{
    double gScore[V], fScore[V];
    int visited[V] = {0};
    PriorityQueue pq = {.size = 0};

    for (int i = 0; i < V; i++)
    {
        gScore[i] = INF;
        fScore[i] = INF;
        cameFrom[i] = -1;
    }

    gScore[start] = 0;
    fScore[start] = vincenty(nodes[start], nodes[goal]);

    pqInsert(&pq, start, fScore[start]);

    while (pq.size > 0)
    {
        int current = pqExtractMin(&pq);
        if (current == goal)
        {
            *totalDist = (int)gScore[goal];
            return 1;
        }

        visited[current] = 1;
        for (int neighbor = 0; neighbor < V; neighbor++)
        {
            if (graph[current][neighbor] == INF || visited[neighbor]) continue;
            double tentative_gScore = gScore[current] + graph[current][neighbor];
            if (tentative_gScore < gScore[neighbor])
            {
                cameFrom[neighbor] = current;
                gScore[neighbor] = tentative_gScore;
                fScore[neighbor] = tentative_gScore + vincenty(nodes[neighbor], nodes[goal]);
                pqInsert(&pq, neighbor, fScore[neighbor]);
            }
        }
    }
    return 0;
}


int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <algorithm_choice> <source> <destination>\n", argv[0]);
        return 1;
    }

    // Parse command-line arguments
    int choice = atoi(argv[1]);  // Algorithm choice (1, 2, 3, or 4)
    char *source = argv[2];      // Source node name
    char *dest = argv[3];        // Destination node name

    // Find source and destination node indexes
    int srcIdx = findNodeByName(source);
    int destIdx = findNodeByName(dest);

    if (srcIdx == -1 || destIdx == -1) {
        printf("Invalid source or destination.\n");
        return 1;
    }

    int graph[V][V], next[V][V];
    // Initialize graph
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i == j) graph[i][j] = 0;
            else {
                double d = vincenty(nodes[i], nodes[j]);
                graph[i][j] = (d < THRESHOLD) ? (int)d : INF;
            }
            next[i][j] = (graph[i][j] != INF) ? j : -1;
        }
    }

    SIZE_T memBeforeKB, memAfterKB;
    getMemorySnapshot(&memBeforeKB);
    clock_t start = clock();  // Start timing

    if (choice == 3) {
        floydWarshall(graph, next);
        int totalDist = graph[srcIdx][destIdx];
        if (totalDist == INF) {
            printf("No valid path found.\n");
            return 1;
        }
        double time = totalDist / WALKING_SPEED;
        printf("\nShortest Distance: %d meters\n", totalDist);
        printf("Walking Time: %.2f minutes\n", time / 60.0);
        printf("Total Steps: %d\n", (int)(totalDist / STEP_LENGTH));
        char url[1000] = "";
        reconstructPath(srcIdx, destIdx, next, url, totalDist, time);
        printf("Open the following URL in your browser:\n%s\n", url);
    }
    else if (choice == 1) {
        int path[V], pathLength;
        double totalDist;
        dijkstra(srcIdx, destIdx, path, &pathLength, &totalDist);
        if (pathLength == 0 || totalDist == INF) {
            printf("No valid path found.\n");
            return 1;
        }
        double time = totalDist / WALKING_SPEED;
        printf("\nShortest Distance: %.0f meters\n", totalDist);
        printf("Walking Time: %.2f minutes\n", time / 60.0);
        printf("Total Steps: %d\n", (int)(totalDist / STEP_LENGTH));
        printf("Open the following URL in your browser:\nhttps://www.openstreetmap.org/directions?engine=fossgis_osrm_foot&route=");
        for (int i = 0; i < pathLength; i++)
            printf("%lf,%lf%s", nodes[path[i]].lat, nodes[path[i]].lon, (i == pathLength - 1 ? "" : ";"));
        printf("\n");
    }
    else if (choice == 2) {
        int totalDist, cameFrom[V];
        if (!aStar(graph, nodes, srcIdx, destIdx, &totalDist, cameFrom)) {
            printf("No valid path found.\n");
            return 1;
        }
        double time = totalDist / WALKING_SPEED;
        printf("\nShortest Distance: %d meters\n", totalDist);
        printf("Walking Time: %.2f seconds\n", time);
        char url[1000] = "";
        reconstructAStarPath(cameFrom, destIdx, nodes, url, totalDist, time);
        printf("OpenStreetMap Route: %s\n", url);
    }
    else if (choice == 4) {
        // Comparison of all algorithms
        double dijkstraTime, aStarTime, floydTime;
        double dijkstraMem, aStarMem, floydMem;
        LARGE_INTEGER freq, tStart, tEnd;
        QueryPerformanceFrequency(&freq);

        // --- Dijkstra ---
        getMemorySnapshot(&memBeforeKB);
        QueryPerformanceCounter(&tStart);
        int path[V], pathLength;
        double totalDist;
        dijkstra(srcIdx, destIdx, path, &pathLength, &totalDist);
        QueryPerformanceCounter(&tEnd);
        getMemorySnapshot(&memAfterKB);
        dijkstraTime = (double)(tEnd.QuadPart - tStart.QuadPart) / freq.QuadPart;
        dijkstraMem = ((V * sizeof(int))     // dist[]
                       + (V * sizeof(bool))    // visited[]
                       + (V * sizeof(int))     // prev[]
                       + (V * sizeof(int)))    // heap
                      / 1024.0;

        // --- A* ---
        getMemorySnapshot(&memBeforeKB);
        QueryPerformanceCounter(&tStart);
        int totalDistAStar, cameFrom[V];
        aStar(graph, nodes, srcIdx, destIdx, &totalDistAStar, cameFrom);
        QueryPerformanceCounter(&tEnd);
        getMemorySnapshot(&memAfterKB);
        aStarTime = (double)(tEnd.QuadPart - tStart.QuadPart) / freq.QuadPart;
        aStarMem = ((V * sizeof(double))    // gScore[]
                    + (V * sizeof(double))  // fScore[]
                    + (V * sizeof(int))     // openSet[]
                    + (V * sizeof(int)))    // cameFrom[]
                   / 1024.0;

        // --- Floyd-Warshall ---
        getMemorySnapshot(&memBeforeKB);
        QueryPerformanceCounter(&tStart);
        floydWarshall(graph, next);
        QueryPerformanceCounter(&tEnd);
        getMemorySnapshot(&memAfterKB);
        int totalDistFloyd = graph[srcIdx][destIdx];
        floydTime = (double)(tEnd.QuadPart - tStart.QuadPart) / freq.QuadPart;
        floydMem = ((V * V * sizeof(int)) * 2) / 1024.0;

        // --- Output ---
        printf("\nAlgorithm\t\tDistance\tTime (s)\tMemory (KB)\n");
        printf("---------------------------------------------------------------\n");
        printf("Dijkstra\t\t%.0f\t\t%.6f\t%.2f\n", totalDist, dijkstraTime, dijkstraMem);
        printf("A*\t\t\t%d\t\t%.6f\t%.2f\n", totalDistAStar, aStarTime, aStarMem);
        printf("Floyd-Warshall\t\t%d\t\t%.6f\t%.2f\n", totalDistFloyd, floydTime, floydMem);
    }
    else {
        printf("Invalid choice.\n");
        return 1;
    }

    clock_t end = clock();  // End timing
    getMemorySnapshot(&memAfterKB);
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    SIZE_T memUsedKB;
    if (memAfterKB >= memBeforeKB)
        memUsedKB = memAfterKB - memBeforeKB;
    else
        memUsedKB = memBeforeKB - memAfterKB;

        // Display the input values
    printf("Choice : %d\n", choice);
    printf("Source : %s\n", source);
    printf("Destination : %s\n", dest);

    printf("Execution Time: %.4f seconds\n", elapsed);
    printf("Memory Used: %zu kB\n", memUsedKB);

    return 0;
}