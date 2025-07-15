#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_POINTS 10000
#define DEFAULT_ITER 400
#define EPSILON 0.001

typedef struct {
    double* coords;
} Point;

double euclidean_distance(Point p1, Point p2, int dim) {
    double sum = 0.0;
    for (int i = 0; i < dim; i++) {
        double diff = p1.coords[i] - p2.coords[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

int detect_dimension(const char* line) {
    int dim = 1;
    for (const char* p = line; *p; p++) {
        if (*p == ',') dim++;
    }
    return dim;
}

void kmeans(Point* points, int num_points, int K, int max_iter, int dim, Point* centroids) {
    Point prev_centroids[K];
    int cluster_assignments[num_points];

    for (int i = 0; i < K; i++) {
        centroids[i].coords = malloc(dim * sizeof(double));
        prev_centroids[i].coords = malloc(dim * sizeof(double));
        for (int d = 0; d < dim; d++) {
            centroids[i].coords[d] = points[i].coords[d];
            prev_centroids[i].coords[d] = centroids[i].coords[d];
        }
    }

    int iteration = 0;
    while (iteration < max_iter) {
        Point new_centroids[K];
        int cluster_sizes[K];

        for (int i = 0; i < K; i++) {
            cluster_sizes[i] = 0;
            new_centroids[i].coords = calloc(dim, sizeof(double));
        }

        for (int i = 0; i < num_points; i++) {
            double min_dist = euclidean_distance(points[i], centroids[0], dim);
            int cluster_index = 0;
            for (int j = 1; j < K; j++) {
                double dist = euclidean_distance(points[i], centroids[j], dim);
                if (dist < min_dist) {
                    min_dist = dist;
                    cluster_index = j;
                }
            }
            cluster_assignments[i] = cluster_index;
            for (int d = 0; d < dim; d++) {
                new_centroids[cluster_index].coords[d] += points[i].coords[d];
            }
            cluster_sizes[cluster_index]++;
        }

        for (int i = 0; i < K; i++) {
            if (cluster_sizes[i] > 0) {
                for (int d = 0; d < dim; d++) {
                    new_centroids[i].coords[d] /= cluster_sizes[i];
                }
            } else {
                for (int d = 0; d < dim; d++) {
                    new_centroids[i].coords[d] = centroids[i].coords[d];
                }
            }
        }

        int converged = 1;
        for (int i = 0; i < K; i++) {
            if (euclidean_distance(centroids[i], prev_centroids[i], dim) >= EPSILON) {
                converged = 0;
                break;
            }
        }

        for (int i = 0; i < K; i++) {
            for (int d = 0; d < dim; d++) {
                prev_centroids[i].coords[d] = centroids[i].coords[d];
                centroids[i].coords[d] = new_centroids[i].coords[d];
            }
            free(new_centroids[i].coords);
        }

        if (converged) break;
        iteration++;
    }

    for (int i = 0; i < K; i++) {
        free(prev_centroids[i].coords);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Invalid number of arguments!\n");
        return 1;
    }

    int K = atoi(argv[1]);
    int max_iter = (argc == 3) ? atoi(argv[2]) : DEFAULT_ITER;

    if (K <= 1 || max_iter <= 1 || max_iter >= 1000) {
        fprintf(stderr, "Invalid arguments!\n");
        return 1;
    }

    Point points[MAX_POINTS];
    int num_points = 0;
    int dim = 0;

    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        if (strlen(line) <= 1) continue;

        if (dim == 0) {
            dim = detect_dimension(line);
        }

        Point p;
        p.coords = malloc(dim * sizeof(double));

        char* token = strtok(line, ",\n");
        int d = 0;
        while (token && d < dim) {
            p.coords[d++] = atof(token);
            token = strtok(NULL, ",\n");
        }

        if (d == dim) {
            points[num_points++] = p;
        } else {
            free(p.coords);
        }

        if (num_points >= MAX_POINTS) break;
    }

    if (K >= num_points) {
        fprintf(stderr, "Invalid number of clusters!\n");
        for (int i = 0; i < num_points; i++) free(points[i].coords);
        return 1;
    }

    Point centroids[K];
    kmeans(points, num_points, K, max_iter, dim, centroids);

    for (int i = 0; i < K; i++) {
        for (int d = 0; d < dim; d++) {
            printf("%.4f", centroids[i].coords[d]);
            if (d < dim - 1) printf(",");
        }
        printf("\n");
        free(centroids[i].coords);
    }

    for (int i = 0; i < num_points; i++) {
        free(points[i].coords);
    }

    return 0;
}
