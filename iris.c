#include "iris.h"

static void normlize_iris(data_t *vectors, int n, int fields)
{
    for (int iterator = 0; iterator < n; ++iterator)
    {
        normilize_vec(vectors[iterator].vec, vectors[iterator].norm, fields);
    }
}

/*
* récupère le nombre de champs des données
*/
static int getfield_number(char *line)
{
    char *save_ptr = NULL, *token = NULL;
    int count = 0;
    token = line;
    for (char *str = token; token != NULL; str = NULL)
    {
        token = strtok_r(str, ",", &save_ptr);
        if (token != NULL)
        {
            if (is_digit(token))
            {
                ++count;
            }
        }
    }
    return count;
}

/*
* récupère le nombre de lignes.
*/

static int getline_number(FILE *stream)
{
    size_t len = 0;
    char *line = NULL;
    ssize_t nread;
    int count = -1;
    fpos_t pos;
    fgetpos(stream, &pos);
    while ((nread = getline(&line, &len, stream)) != -1)
    {
        ++count;
    }
    fsetpos(stream, &pos);
    free(line);
    return count;
}

iris_t *init_data(FILE *stream)
{
    iris_t *data = NULL;
    ssize_t nread;
    size_t len = 0;
    char *line = NULL;
    fpos_t pos;
    int ndata = 0, nfields = 0;
    data = (iris_t *)malloc(sizeof *data);
    assert(data != NULL);
    fgetpos(stream, &pos);
    data->ndata = getline_number(stream);
    nread = (getline(&line, &len, stream) > -1);
    data->nfields = getfield_number(line);
    ndata = data->ndata;
    nfields = data->nfields;
    data->data = (data_t *)malloc(ndata * sizeof *data->data);
    assert(data->data != NULL);
    for (int i = 0; i < ndata; ++i)
    {
        data->data[i].vec = (double *)malloc(nfields * sizeof *data->data[i].vec);
        assert(data->data[i].vec != NULL);
    }
    nread += ndata * 2;
    free(line);
    fsetpos(stream, &pos);
    return data;
}

/*
*   récupère les datas depuis la base de données. 
*/

void get_data(FILE *stream, data_t *data, int ndata, int fields)
{
    size_t len = 0;
    char *line = NULL, *token = NULL, *save_ptr = NULL;
    ssize_t nread;
    int count = -1;
    fpos_t pos;
    int count_fields = 0;
    fgetpos(stream, &pos);
    while ((nread = getline(&line, &len, stream)) > 1)
    {
        ++count;
        token = line;
        count_fields = 0;
        for (char *str = token; count_fields <= fields; str = NULL)
        {
            token = strtok_r(str, ",", &save_ptr);
            if (token == NULL)
                break;
            if (token != NULL && count_fields < fields)
            {
                data[count].norm = get_digits(token, data[count].vec, fields, count_fields);
            }
            else
            {
                data[count].label = mystrdup(data[count].label, token);
            }
            ++count_fields;
        }
    }
    normlize_iris(data, ndata, fields);
    fsetpos(stream, &pos);
    free(line);
    line = NULL;
}

/*
*   libère la mémoire allouée.
*/

void free_data(iris_t *data)
{
    int ndata = data->ndata;
    for (int i = 0; i < ndata; ++i)
    {
        free(data->data[i].vec);
        free(data->data[i].label);
        data->data[i].vec = NULL;
        data->data[i].norm = 0;
        data->data[i].label = NULL;
    }
    free(data->data);
    data->data = NULL;
    free(data);
    data = NULL;
}

/*
*   affichage des datas.
*/

void print_data(iris_t *data)
{
    int fields = data->nfields;
    int ndata = data->ndata;
    data_t *d = data->data;

    for (int count = 0; count < ndata; ++count)
    {
        fprintf(stdout, "%s :", d[count].label);
        for (int count_fields = 0; count_fields < fields; ++count_fields)
        {
            fprintf(stdout, "%0.2f ", d[count].vec[count_fields]);
        }
        fprintf(stdout, "norm: %0.2f\n", d[count].norm);
    }
}

iris_t *parse_database(const char *db_path)
{
    FILE *db_file = fopen(db_path, "r");
    iris_t *db = init_data(db_file);
    get_data(db_file, db->data, db->ndata, db->nfields);
    fclose(db_file);
    return db;
}