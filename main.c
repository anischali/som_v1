#include "toolbox.h"
#include "network.h"
#include "iris.h"
#include "time.h"
#include "config.h"
#include <sys/stat.h>

static void produce_network(void)
{
    network_t *network = NULL;
    config_t *config = init_config();
    iris_t *db_learn = parse_database(config->db_learn);
    iris_t *db_test = parse_database(config->db_test);
    double *mean = mean_vec(db_learn->data, db_learn->ndata, db_learn->nfields);
    double *limits = center_data_space(db_learn->nfields, mean, config->buttom_limit, config->up_limit);
    if (config->auto_iter == 1)
        config->total_iteration = (500 * db_learn->nfields);

    int r = 0, phase2_iter = config->total_iteration - (config->total_iteration / 5),
        phase1_iter = (config->total_iteration / 5);
    double percent = 0.0f;

    if (config->dims == 1)
        network = init_network_manual(config->map_height, config->map_width, db_learn->nfields, limits, config->type_init);
    else
        network = init_network(db_learn->ndata, db_learn->nfields, limits, config->type_init);

    printf("Init network: map height: %d map width: %d\n\n", network->nl, network->nc);
    if (config->percent > 0)
    {
        while (percent < config->percent)
        {
            init_memory_values(network, limits, config->type_init);
            printf("Begin Learning....\n");
            learning_phase(network, db_learn, config->learning_rate, config->neighboring_ray, phase1_iter, 0,
                           config->shuffling_in_each_iter, config->bubble_update);
            printf("Learning phase: 1 total iter: %d phase 1 iter: %d neighboring: %d learning rate: %0.4f\n",
                   config->total_iteration, phase1_iter, config->neighboring_ray, config->learning_rate);

            learning_phase(network, db_learn, config->learning_rate / 10.0, 1, phase2_iter, -1,
                           config->shuffling_in_each_iter, config->bubble_update);
            printf("Learning phase: 2 total iter: %d phase 1 iter: %d neighboring: %d learning rate: %.4f\n",
                   config->total_iteration, phase2_iter, 1, config->learning_rate / 10.0f);
            tag_network(network, db_learn);
            printf("Tagging network...\n");
            r = test_network(network, db_learn);
            printf("Testing network...\n");
            percent = (double)(r * 100.0f / (double)db_learn->ndata);
            printf("Network good answers: %.2f/100.00\n", percent);
            printf("End Learning....\n\n");
            print_map(network);
        }
    }
    else
    {
        printf("Begin Learning....\n");
        learning_phase(network, db_learn, config->learning_rate, config->neighboring_ray, phase1_iter, 0,
                       config->shuffling_in_each_iter, config->bubble_update);
        printf("Learning phase: 1 total iter: %d phase 1 iter: %d neighboring: %d learning rate: %0.4f\n",
               config->total_iteration, phase1_iter, config->neighboring_ray, config->learning_rate);

        learning_phase(network, db_learn, config->learning_rate / 10.0, 1, phase2_iter, -1,
                       config->shuffling_in_each_iter, config->bubble_update);
        printf("Learning phase: 2 total iter: %d phase 1 iter: %d neighboring: %d learning rate: %.4f\n",
               config->total_iteration, phase2_iter, 1, config->learning_rate / 10.0f);
        tag_network(network, db_learn);
        printf("Tagging network...\n");
        r = test_network(network, db_learn);
        printf("Testing network...\n");
        percent = (double)(r * 100.0f / (double)db_learn->ndata);
        printf("Network good answers: %.2f/100.00\n", percent);
        printf("End Learning....\n\n");
    }
    printf("Printing map...\n");
    printf("+++++++++++++++++++++++++++++++++\n");
    print_map(network);
    printf("+++++++++++++++++++++++++++++++++\n");
    save_network(config->network_result_path, network);
    printf("Saving  network to %s\n", config->network_result_path);
    network_to_image(network, "./som.png");
    free(limits);
    free(mean);
    free_data(db_learn);
    free_data(db_test);
    free_config(config);
    free_network(network);
    printf("Freeing memory...\n");
}

static void view_network(const char *net_path)
{
    network_t *network = NULL;
    config_t *config = init_config();
    iris_t *db_learn = parse_database(config->db_learn);
    iris_t *db_test = parse_database(config->db_test);
    network = import_network(net_path);
    printf("Importing Network...\n");
    printf("network lines: %d columns: %d\n", network->nl, network->nc);
    printf("Printing map...\n");
    printf("+++++++++++++++++++++++++++++++++\n");
    print_map(network);
    printf("+++++++++++++++++++++++++++++++++\n");
    int r = test_network(network, db_learn);
    printf("Testing network...\n");
    printf("Network good answers: %.2f/100.00 for learning database\n", (double)(r * 100.0f / (double)db_learn->ndata));
    r = test_network(network, db_test);
    printf("Network good answers: %.2f/100.00 for testing database\n", (double)(r * 100.0f / (double)db_test->ndata));
    free_network(network);
    free_data(db_learn);
    free_data(db_test);
    free_config(config);
    printf("Freeing memory...\n");
}

static void usage(void)
{
    fprintf(stdout, "__________________________________________________________________________________________\n");
    fprintf(stdout, "Usage 1:\n./som\n\tTo produce new network at least 99/100 of performance and saving\n\t\tit in a name defined in config.cfg.\n");
    fprintf(stdout, "Usage 2:\n./som <network_name.bin>\n\tTo view a network saved previously.\n");
    fprintf(stdout, "__________________________________________________________________________________________\n\n\n");
}

static int file_exist(const char *filename)
{
    struct stat buffer;
    int exist = stat(filename, &buffer);
    if (exist == 0)
        return 0;
    return 1;
}

int main(int argc, char const **argv)
{

    srand(clock());
    usage();
    if (!(argc > 1))
    {
        produce_network();
    }
    else
    {
        if (file_exist(argv[1]))
        {
            fprintf(stderr, "filename %s doesn't exist verify path.\n\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        view_network(argv[1]);
    }
    return 0;
}
