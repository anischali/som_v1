#ifndef NETWORK_H
#define NETWORK_H
#include "toolbox.h"


struct iris_t;
typedef struct iris_t iris_t;

typedef struct neuron_t neuron_t;
struct neuron_t
{
    int label;
    int l;
    int c;
    double act;
    double *data;
};

typedef struct network_t network_t;
struct network_t
{
    int nn;
    int nl;
    int nc;
    int nfields;
    neuron_t **neurons;
};

typedef struct bmu_t bmu_t;

struct bmu_t
{
    int l;
    int c;
    double act_state;
    int db_index;
};
void init_memory_values(network_t *network, double *min_max, int init_type);
double *center_data_space(int data_size, const double *meam_vec, double buttom_limit, double up_limit);
void free_network(network_t *network);
network_t *init_network(int data_size, int nmem_size, double *min_max, int init_type);
network_t *init_network_manual(int height, int width, int nmem_size, double *min_max, int init_type);
void free_layer(neuron_t *neurons, int size);
void learning_phase(network_t *network, iris_t *db, double alpha, int ray, int total_iter, int nu, int shuffle_iter, int bubble);
void tag_network(network_t *network, iris_t *db);
int test_network(network_t *network, iris_t *db_test);
void print_map(network_t *network);
void save_network(const char *path, network_t *network);
void network_to_image(network_t *network, const char *filename);
network_t *import_network(const char *path);
#endif
