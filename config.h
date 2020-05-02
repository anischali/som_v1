#ifndef CONFIG_H
#define CONFIG_H
#include "toolbox.h"

typedef struct config_t config_t;

struct config_t
{
    char *db_learn;
    char *db_test;
    char *network_result_path;
    int neighboring_ray;
    int auto_iter;
    int total_iteration;
    int shuffling_in_each_iter;
    int bubble_update;
    int type_init;
    double up_limit;
    double buttom_limit;
    double learning_rate;
    int dims;
    int map_height;
    int map_width;
    double percent;
};

config_t *init_config();
void free_config(config_t *cfg);

#endif