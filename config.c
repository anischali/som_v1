#include "config.h"
enum
{
    DB_LEARN = 0,
    DB_TEST,
    PATH_RESULT,
    RAY_NEIGHBORS,
    AUTO_ITER,
    TOTAL_ITERATION,
    SHUFFLING_EACH_ITER,
    BUBBLE_UPDATE,
    TYPE_INIT,
    UP_LIMIT,
    BUTTOM_LIMIT,
    LEARNING_RATE,
    DIMS,
    HEIGHT,
    WIDTH,
    PERCENT,
    END_CONF
};

config_t *init_config()
{
    char *parsed[END_CONF];
    config_t *cfg = malloc(sizeof *cfg);
    FILE *config_file = fopen("./config.cfg", "r");
    size_t len = 0;
    char *line = NULL, *conf = NULL;
    ssize_t nread;
    int iconfig = 0;
    while ((nread = getline(&line, &len, config_file)) != -1)
    {
        if (iconfig < 3)
            line[strlen(line) - 1] = '\0';
        conf = line;
        while (*conf != ':')
            ++conf;
        parsed[iconfig++] = strdup((++conf));
    }
    free(line);
    line = NULL;
    cfg->db_learn = strdup(parsed[DB_LEARN]);
    cfg->db_test = strdup(parsed[DB_TEST]);
    cfg->network_result_path = strdup(parsed[PATH_RESULT]);
    cfg->neighboring_ray = atoi(parsed[RAY_NEIGHBORS]);
    cfg->auto_iter = (strncmp("TRUE", parsed[AUTO_ITER], 4) == 0) ? 1 : 0;
    if (cfg->auto_iter == 0)
        cfg->total_iteration = atoi(parsed[TOTAL_ITERATION]);
    cfg->shuffling_in_each_iter = (strncmp("TRUE", parsed[SHUFFLING_EACH_ITER], 4) == 0) ? 1 : 0;
    cfg->bubble_update = (strncmp("TRUE", parsed[BUBBLE_UPDATE], 4) == 0) ? 1 : 0;
    if (strncmp("ZERO", parsed[TYPE_INIT], 4) == 0)
        cfg->type_init = 0;
    else if (strncmp("RAND", parsed[TYPE_INIT], 4) == 0)
        cfg->type_init = 1;
    else if (strncmp("MEAN_DATA", parsed[TYPE_INIT], 9) == 0)
        cfg->type_init = 2;
    line = NULL;
    if (cfg->type_init == 2)
    {
        cfg->up_limit = strtod(parsed[UP_LIMIT], &line);
        line = NULL;
        cfg->buttom_limit = strtod(parsed[BUTTOM_LIMIT], &line);
    }
    line = NULL;
    cfg->learning_rate = strtod(parsed[LEARNING_RATE], &line);
    if (strncmp("MANUAL", parsed[DIMS], 6) == 0)
    {
        cfg->dims = 1;
        cfg->map_height = atoi(parsed[HEIGHT]);
        cfg->map_width = atoi(parsed[WIDTH]);
    }
    else
    {
        cfg->dims = 0;
        cfg->map_height = -1;
        cfg->map_width = -1;
    }
    line = NULL;
    cfg->percent = strtod(parsed[PERCENT], &line);;
    for (int iparsed = 0; iparsed < END_CONF; ++iparsed)
    {
        free(parsed[iparsed]);
        parsed[iparsed] = NULL;
    }
    fclose(config_file);
    return cfg;
}

void free_config(config_t *cfg)
{
    free(cfg->db_learn);
    free(cfg->db_test);
    free(cfg->network_result_path);
    free(cfg);
    cfg = NULL;
}