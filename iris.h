#ifndef IRIS_H
#define IRIS_H
#include "toolbox.h"

/*
*   structures pour les données.
*/
typedef struct data_t data_t;
struct data_t
{
    double *vec;
    double norm;
    char *label;
};

/*
*   le tableau qui contient toutes les données.
*/
typedef struct iris_t iris_t;
struct iris_t
{
    int nfields;
    int ndata;
    data_t *data;
};
/*
*   
*
*/
struct iris_t *init_data(FILE *stream);

void get_data(FILE *stream, data_t *data, int ndata, int fields);

void free_data(iris_t *data);

void print_data(iris_t *data);

iris_t *parse_database(const char *db_path);

#endif
