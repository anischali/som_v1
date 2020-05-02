#ifndef TOOLBOX_H
#define TOOLBOX_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <assert.h>

struct data_t;
typedef struct data_t data_t;

void shuffle(int *vec, int size);
double myRand(double min, double max);
int myRandInt(int min, int max);
int *generate_indices(int size);
char *mystrdup(char *dst, char *src);
double get_digits(char *str, double *vec, int nfields, int pos);
double get_norm(double *vec, int nfeilds);
int is_digit(char *str);
void normilize_vec(double *vec, double norm, int nfields);
double *mean_vec(data_t *vectors, int nbr_vecs, int dim);
#endif