#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <ctype.h>

#define Epsilon 0.001
#define assertinput(expr) \
 if (!(expr)) \
 { \
     printf("Invalid Input!"); \
     exit(1); \
 }
 #define assertruntime(expr) \
 if (!(expr)) \
 { \
     printf("An Error Has Occurred"); \
     exit(1); \
 }

int *calc_lengths(char *input_filename);
double *read_input_file(char *input_filename, int vector_len, int N);
double *init_centroids(double *coordinates, int vector_len, int K);
int *get_closest_clusters_indeces(double *centroids, double *coordinates, int N, int vector_len, int K);
double distance(double *vec_1, double *vec_2, int vector_len);
double *update_centroids(int K, int N, int vector_len, int *closest_clusters_indeces, double *coordinates);
void write_centroids(char *output_filename, int vector_len, double *centroids, int K);
void not_txt_file(char *file_name);

int main(int argc, char *argv[])
{
    int K;
    int max_iter;
    char *input_filename;
    char *output_filename;

    int *lenght_arr;
    int vector_len;
    int N;
    int i;
    double *coordinates;
    double *centroids;

    /* check arguments are correct */
    if (argc < 4 || argc > 5)
    {
        printf("Invalid Input!");
        return 1;
    }

    /* read arguments into variables */
    assertinput(atoi(argv[1]));
    K = atoi(argv[1]);  
    assertinput(K > 1);
    max_iter = 200; /* default */

    if (argc == 5)
    {
        assertinput(atoi(argv[2]));
        max_iter = atoi(argv[2]);
        assertinput(max_iter > 0);
        input_filename = argv[3];
        not_txt_file(input_filename);
        output_filename = argv[4];
        not_txt_file(output_filename);
    }
    else
    {
        input_filename = argv[2];
        not_txt_file(input_filename);
        output_filename = argv[3];
        not_txt_file(output_filename);
    }

    /* calc vector length, and num of vectors */
    lenght_arr = calc_lengths(input_filename);
    vector_len = lenght_arr[0];
    N = lenght_arr[1];
    assertinput(K < N);

    free(lenght_arr);

    /* read datapoint from input file into an array of coordinates */
    coordinates = read_input_file(input_filename, vector_len, N);

    /* Initialize centroids µ1, µ2, . . . , µK as first k datapoints x1, x2, . . . , xK */
    centroids = init_centroids(coordinates, vector_len, K);

    /* repeat until: convergence: ||∆µk|| < epsilon) -> "break" OR: iteration number = max iter -> for loop */
    for (i = 0; i < max_iter; i++)
    {
        double delta;
        int j;

        /* assign xi to the closest cluster Sj : argmin Sj (xi − µj)**2, ∀j 1 ≤ j ≤ K */
        int *closest_clusters_indeces = get_closest_clusters_indeces(centroids, coordinates, N, vector_len, K);

        /* for µk, 0 < k ≤ K: Update the centroids µk = (sum(x in Sk))/|Sk| */
        double *updated_centroids = update_centroids(K, N, vector_len, closest_clusters_indeces, coordinates);

        free(closest_clusters_indeces);

        /* if ||∆µk|| < epsilon then break */
        delta = 0;
        for (j = 0; j < K; j++)
        {
            double j_dist = distance(&centroids[j * vector_len], &updated_centroids[j * vector_len], vector_len);
            if (j_dist > delta)
            {
                delta = j_dist;
            }
        }

        /* update centroids */
        free(centroids);
        centroids = updated_centroids;

        if (delta < Epsilon)
        {
            break;
        }
    }

    free(coordinates);

    /* write k_means output to output file */
    write_centroids(output_filename, vector_len, centroids, K);

    free(centroids);

    return 0;
}

void not_txt_file(char *file_name){
    int len;
        if(!((len = strlen(file_name)) > 3 && !strcmp(file_name + len - 4, ".txt"))) {
            printf("Invalid Input!"); 
            exit(1);
        }
}

/* returns an array: [vector length, num of vectors] */
int *calc_lengths(char *input_filename)
{
    const char COMMA = ',';
    const char NEWLINE = '\n';
    int first_line = 0;
    int c = 0;
    FILE *f;

    /* initilize an array of lenghts: [vector_lenght, num of vectors] */
    int *lengths = calloc(2, sizeof(int));
    assertruntime(lengths!=NULL);
    lengths[0] = 1;

    f = fopen(input_filename, "r");
    assertruntime(f != NULL);
    while ((c = fgetc(f)) != EOF)
    {
        if (first_line == 0 && c == COMMA)
        {
            lengths[0] += 1;
        }
        else if (c == NEWLINE)
        {
            lengths[1] += 1;
            if (first_line == 0)
            {
                first_line = 1;
            }
        }
    }
    
    fclose(f);
    return lengths;
}

/* reads doubles coordinates seperated by commas and new lines into an array, and return it */
double *read_input_file(char *input_filename, int vector_len, int N)
{
    /* allocate an array of size (num of vectors * len of vector) */
    double *coordinates;
    int i = 0;
    double coordinate;
    int result;
    FILE *f;
    coordinates = calloc(N * vector_len, sizeof(double));
    assertruntime(coordinates!=NULL);

    f = fopen(input_filename, "r");
    assertruntime(f != NULL);

    do
    {
        result = fscanf(f, "%lf", &coordinate);
        if (result == 0)
        {
            result = fscanf(f, "%*c");
        }
        else
        {
            coordinates[i] = coordinate;
            i++;
        }

    } while (result != EOF);

    fclose(f);
    return coordinates;
}

/* returns an array of centroids - initialize centroids µ1, µ2, . . . , µK as first k datapoints x1, x2, . . . , xK */
double *init_centroids(double *coordinates, int vector_len, int K)
{
    double *centroids;
    int i;
    centroids = calloc(K * vector_len, sizeof(double));
    assertruntime(centroids!=NULL);
    for (i = 0; i < K * vector_len; i++) /* K vectors, vector_len coordinates in each one */
    {
        centroids[i] = coordinates[i];
    }
    return centroids;
}

/* returns an array of size N (num of vectors), arr[i] = index of closest cluster for xi */
int *get_closest_clusters_indeces(double *centroids, double *coordinates, int N, int vector_len, int K)
{
    double *vector;
    double min_dist;
    int closest_index;
    double dist;
    int i;

    int *closest_clusters_index = calloc(N, sizeof(int));
    assertruntime(closest_clusters_index!=NULL);

    for (i = 0; i < N; i++)
    {
        int j;
        vector = &coordinates[i * vector_len];
        min_dist = DBL_MAX;
        closest_index = 0;
        for (j = 0; j < K; j++)
        {
            dist = distance(vector, &centroids[j * vector_len], vector_len);
            if (dist < min_dist)
            {
                min_dist = dist;
                closest_index = j;
            }
        }
        closest_clusters_index[i] = closest_index;
    }
    return closest_clusters_index;
}

/* returns the euclidean-dist between two vectors */
double distance(double *vec_1, double *vec_2, int vector_len)
{
    double curr_sum = 0;
    int i;
    for (i = 0; i < vector_len; i++) /* for each coordinate */
    {
        curr_sum = curr_sum + pow(*(vec_1 + i) - *(vec_2 + i), 2);
    }
    return sqrt(curr_sum);
}

double *update_centroids(int K, int N, int vector_len, int *closest_clusters_indeces, double *coordinates)
{
    double *new_centroids;
    int *cluster_sizes;
    int closest_cluster_index;
    int i;

    new_centroids = calloc(K * vector_len, sizeof(double));
    assertruntime(new_centroids!=NULL);
    cluster_sizes = calloc(K, sizeof(int));
    assertruntime(cluster_sizes!=NULL);    

    /* sum vectors into clusters */
    for (i = 0; i < N; i++) /* for every vector */
    {
        int j;
        closest_cluster_index = closest_clusters_indeces[i];
        for (j = 0; j < vector_len; j++) /* for every coordinate in vector */
        {
            new_centroids[closest_cluster_index * vector_len + j] += coordinates[i * vector_len + j];
        }
        cluster_sizes[closest_cluster_index] += 1;
    }

    /* divide cluster by size */
    for (i = 0; i < K; i++)
    {
        int j;
        for (j = 0; j < vector_len; j++) /* for every coordinate-sum in cluster */
        {
            assertruntime(cluster_sizes[i] != 0);
            new_centroids[i * vector_len + j] /= cluster_sizes[i];
        }
    }

    free(cluster_sizes);

    return new_centroids;
}

/* reads doubles coordinates seperated by commas and new lines into an array, and return it */
void write_centroids(char *output_filename, int vector_len, double *centroids, int K)
{
    int i;
    FILE *f = fopen(output_filename, "w");
    assertruntime(f != NULL);

    for (i = 0; i < K; i++)
    {
        int j;
        for (j = 0; j < vector_len; j++)
        {
            fprintf(f, "%.4f", centroids[i * vector_len + j]);
            if (j < vector_len - 1)
            {
                fprintf(f, "%c", ',');
            }
        }
        fprintf(f, "%c", '\n');
    }
    fclose(f);
}
