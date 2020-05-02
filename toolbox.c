#include "toolbox.h"
#include "iris.h"
#include "time.h"

/*
*   
*/
double myRand(double min, double max)
{

    double gen = (double)(((double)rand() / (double)(RAND_MAX + 1.0)));
    return gen * (max - min) + min;
}

/*
*
*/

int myRandInt(int min, int max)
{
    double r = max * (rand() / (RAND_MAX + 1.0));
    return (int)r % (max - min) + min;
}

/*
* verifie qu'une chaine de caractères est un nombre.
*/
int is_digit(char *str)
{
    char *p = str;
    while (*p != '\0')
    {
        if (!((*p >= '0' && *p <= '9') || (*p == '.')))
        {
            return 0;
        }
        p++;
    }
    return 1;
}

/*
*
*/

double get_norm(double *vec, int nfeilds)
{
    double norm = 0;
    double somme = 0;
    for (int i = 0; i < nfeilds; ++i)
    {
        somme += pow(vec[i], 2);
    }
    norm = sqrt(somme);
    return norm;
}

/*
*
*/

double *mean_vec(data_t *vectors, int nbr_vecs, int dim)
{
    double *ret_vec = calloc(dim, sizeof *ret_vec);
    for (int vec_i = 0; vec_i < nbr_vecs; ++vec_i)
    {
        for (int d = 0; d < dim; ++d)
            ret_vec[d] += vectors[vec_i].vec[d];
    }
    for (int d = 0; d < dim; ++d)
    {
        ret_vec[d] = ret_vec[d] / (double)nbr_vecs;
    }
    return ret_vec;
}

/*
*
*/

int *generate_indices(int size)
{
    int *vec = malloc(size * sizeof *vec);
    for (int count = 0; count < size; ++count)
    {
        vec[count] = count;
    }
    return vec;
}

/*
*
*/

static void swap(int *vd, int *vs)
{
    int tmp = *vd;
    *vd = *vs;
    *vs = tmp;
}

/*
*
*/
void shuffle(int *vec, int size)
{
    int rnd;
    for (int count = 0; count < size; ++count)
    {
        do
        {
            rnd = myRandInt(count, size);
        } while (count == rnd && count != size - 1);
        swap(&vec[count], &vec[rnd]);
    }
}

/*
*   duplique une chaine de caractères on allouant la mémoire qui faut.
*   pour éviter les problèmes de compatibilités.
*/

char *mystrdup(char *dst, char *src)
{
    dst = (char *)malloc(sizeof *dst * strlen(src));
    char *s = src;
    char *d = dst;
    while (*s != '\0')
    {
        *d = *s;
        if (*s == '\n')
        {
            *d = '\0';
            break;
        }
        d++;
        s++;
    }
    return dst;
}

/*
*   récupère tout les champs de données puis calcul la norm. 
*/

double get_digits(char *str, double *vec, int nfields, int pos)
{
    char *end;

    vec[pos] = strtod(str, &end);
    if (pos == nfields - 1)
    {
        return get_norm(vec, nfields);
    }
    return 0;
}

/*
*
*/
void normilize_vec(double *vec, double norm, int nfields)
{
    for (int count = 0; count < nfields; ++count)
    {
        vec[count] = vec[count] / norm;
    }
}
