#include "network.h"
#include "iris.h"
#include "time.h"
#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>

#define RED_PRINT "\x1b[31m"
#define GREEN_PRINT "\x1b[32m"
#define BLUE_PRINT "\x1b[34m"
#define DEFAULT_PRINT "\x1b[0m"

static const char *color[] = {GREEN_PRINT, RED_PRINT, BLUE_PRINT};
static const char classes_symbol[] = "ABC.";

/*! \brief Macros pour trouver le maximum entre deux entier.
 *  \param v1 un entier.       
 *  \param v2 un entier.
 */
static inline int maxInt(int v1, int v2)
{
    return (v1 > v2) ? v1 : v2;
}

/*! \brief Macros pour trouver le minimum entre deux entier.
 *  \param v1 un entier.       
 *  \param v2 un entier.
 */
static inline int minInt(int v1, int v2)
{
    return (v1 < v2) ? v1 : v2;
}

/*! \brief Fonction qui retourne un tableau contenant le vecteur 
 *          min et max autour de la moyenne des donnée.
 *  \param nmemb_size la taille des vecteurs des données.       
 *  \param mean_vec le vecteur représentant la moyenne des données.
 *  \param buttom_limit la valeur à ajouter à chaque composante de vecteur moyen pour avoir la barre inférieur.
 *  \param up_limit la valeur à ajouter à chaque composante de vecteur moyen pour avoir la barre supérieur.
 */
double *center_data_space(int nmemb_size, const double *meam_vec, double buttom_limit, double up_limit)
{
    double *limits = (double *)calloc((nmemb_size * 2), sizeof *limits);
    assert(limits != NULL);
    for (int ivec = 0; ivec < nmemb_size; ++ivec)
    {
        limits[ivec] = meam_vec[ivec] - buttom_limit;
        limits[ivec + nmemb_size] = meam_vec[ivec] + up_limit;
    }
    return limits;
}

/*! \brief Fonction qui calcul le nombre de neurones automatiquement.   
 *  \param data_size la taille de la base de données.       
 *  \param neuron_size variable reçuvant le nombre de neurones totale.
 *  \param lines variable reçuvant la hauteur de la map.
 *  \param columns variable reçuvant la largeur de la map.
 */
static void init_neuron_nbr(int data_size, int *neuron_size, int *lines, int *columns)
{
    double r = round(sqrt((5.0 * sqrt(data_size))));
    *columns = (int)r;
    *lines = (int)r;
    *neuron_size = (*lines) * (*columns);
}

/*! \brief Fonction qui initialise la mémoire des neurons.
* \param network la map à libérer. 
* \param min_max les vecteurs autour de la moyenne des données.
* \param init_type le type d'initialisation de la map sois 0 = pour initialisation avec 0, 
*                      1 = aléatoirement entre 0 et 1, 2 = autour de la moyenne des données.
*/

void init_memory_values(network_t *network, double *min_max, int init_type)
{
    int nlines = network->nl, ncols = network->nc, nmem_size = network->nfields;
    neuron_t **neurons = network->neurons;

    for (int iline = 0; iline < nlines; ++iline)
    {
        for (int icols = 0; icols < ncols; ++icols)
        {
            double *vec = neurons[iline][icols].data;
            switch (init_type)
            {
            case 0:
                for (int ivec = 0; ivec < nmem_size; ++ivec)
                {
                    vec[ivec] = 0.0;
                }
                break;
            case 1:
                for (int ivec = 0; ivec < nmem_size; ++ivec)
                {
                    vec[ivec] = myRand(0.0f, 1.0f);
                }
                break;
            case 2:
                for (int ivec = 0; ivec < nmem_size; ++ivec)
                {
                    vec[ivec] = myRand(min_max[ivec], min_max[ivec + nmem_size]);
                }
                break;
            }
        }
    }
}

/*! \brief Fonction qui initialise une map en déterminant automatiquement la hauteur et la largeur.   
 *  \param data_size la taille de la base de données.       
 *  \param nmem_size variable qui représente la taille des vecteur de mémoire de neurone.
 *  \param min_max les vecteurs autour de la moyenne des données.
 *  \param init_type le type d'initialisation de la map sois 0 = pour initialisation avec 0, 
 *                      1 = aléatoirement entre 0 et 1, 2 = autour de la moyenne des données.
 */
network_t *init_network(int data_size, int nmem_size, double *min_max, int init_type)
{
    network_t *network = (network_t *)malloc(sizeof *network);
    init_neuron_nbr(data_size, &(network->nn), &(network->nl), &(network->nc));
    neuron_t **neurons = (neuron_t **)malloc(network->nl * sizeof *neurons);
    int nlines = network->nl, ncols = network->nc;
    network->nfields = nmem_size;
    for (int iline = 0; iline < nlines; ++iline)
    {
        neurons[iline] = (neuron_t *)malloc(ncols * sizeof *neurons[iline]);
        for (int icols = 0; icols < ncols; ++icols)
        {
            neurons[iline][icols].l = iline;
            neurons[iline][icols].c = icols;
            neurons[iline][icols].data = (double *)malloc(nmem_size * sizeof *neurons[iline][icols].data);
        }
    }
    network->neurons = neurons;
    init_memory_values(network, min_max, init_type);
    return network;
}

/*! \brief Fonction qui initialise une map en déterminant manuellement la hauteur et la largeur.   
 *  \param height la hauteur de la map.
 *  \param width la largeur de la map.       
 *  \param nmem_size variable qui représente la taille des vecteur de mémoire de neurone.
 *  \param min_max les vecteurs autour de la moyenne des données.
 *  \param init_type le type d'initialisation de la map sois 0 = pour initialisation avec 0, 
 *                      1 = aléatoirement entre 0 et 1, 2 = autour de la moyenne des données.
 */
network_t *init_network_manual(int height, int width, int nmem_size, double *min_max, int init_type)
{
    network_t *network = (network_t *)malloc(sizeof *network);
    network->nl = height;
    network->nc = width;
    network->nn = height * width;
    neuron_t **neurons = (neuron_t **)malloc(network->nl * sizeof *neurons);
    int nlines = network->nl, ncols = network->nc;
    network->nfields = nmem_size;
    for (int iline = 0; iline < nlines; ++iline)
    {
        neurons[iline] = (neuron_t *)malloc(ncols * sizeof *neurons[iline]);
        for (int icols = 0; icols < ncols; ++icols)
        {
            neurons[iline][icols].l = iline;
            neurons[iline][icols].c = icols;
            neurons[iline][icols].data = (double *)malloc(nmem_size * sizeof *neurons[iline][icols].data);
        }
    }
    network->neurons = neurons;
    init_memory_values(network, min_max, init_type);
    return network;
}

/*! \brief Fonction libère la mémoire d'un neuron.   
 *  \param neuron le neuron à libérer.
 */
static void free_neuron(neuron_t *neuron)
{
    free(neuron->data);
    neuron->data = NULL;
    neuron->c = 0;
    neuron->l = 0;
    neuron = NULL;
}

/*! \brief Fonction libère la mémoire d'une map.   
 *  \param network la map à libérer.
 */
void free_network(network_t *network)
{
    for (int iline = 0; iline < network->nl; ++iline)
    {
        for (int icol = 0; icol < network->nc; ++icol)
        {
            free_neuron(&(network->neurons[iline][icol]));
        }
        free(network->neurons[iline]);
        network->neurons[iline] = NULL;
    }
    free(network->neurons);
    network->neurons = NULL;
    free(network);
    network = NULL;
}

/*! \brief Fonction qui calcule la distance euclidienne entre de vecteur de meme taille.   
 *  \param vec1 le premier vecteur.
 *  \param vec2 le deuxieme vecteur.
 *  \param size la taille des deux vecteurs.
 */
static double distance(double *vec1, double *vec2, int size)
{
    double dist = 0.0;
    double somme = 0.0;
    for (int i = 0; i < size; ++i)
    {
        somme = vec2[i] - vec1[i];
        dist += pow(somme, 2);
    }
    return sqrt(dist);
}

/*! \brief Fonction qui compare deux bmu utilisé pour le tri de tableau des bmus.   
 *  \param v1 le premier bmu.
 *  \param v2 le deuxieme bmu.
 */
static int compar(const void *v1, const void *v2)
{
    if ((*(bmu_t *)v1).act_state > (*(bmu_t *)v2).act_state)
        return 1;
    else if ((*(bmu_t *)v1).act_state < (*(bmu_t *)v2).act_state)
        return -1;
    else
        return 0;
}

/*! \brief Fonction qui trouve le bmu.   
 *  \param best reçoit les données de bmu.
 *  \param vec le tableau de toute les distance euclidiene entre la donnée et tout les neurones de la map.
 *  \param nmemb la taille de vecteur vec des bmu.
 *  \param margin est je prend le minimum au 1 parmis margin minimum.
 *  \param compar et la fonction de comparaison de deux bmu. 
 */
static void best_unit(bmu_t *best, bmu_t *vec, int nmemb, int margin, int (*compar)(const void *p1, const void *p2))
{
    int idx = 0;
    qsort(vec, nmemb, sizeof *vec, compar);
    if (margin != 0)
    {
        idx = myRandInt(0, margin);
    }
    best->act_state = vec[idx].act_state;
    best->l = vec[idx].l;
    best->c = vec[idx].c;
    best->db_index = vec[idx].db_index;
}

/*! \brief Fonction qui calcule la distance euclidienne entre de coordonnées de la map.   
 *  \param x1 la ligne dans la map de la premiere composante.
 *  \param y1 la colonne dans la map de la premiere composante.
 *  \param x2 la ligne dans la map de la deuxieme composante.
 *  \param y2 la colonne dans la map de la deuxieme composante.
*/
double node_dist(int x1, int y1, int x2, int y2)
{
    return (double)sqrt(pow((double)x2 - x1, 2.0) + pow((double)y2 - y1, 2.0));
}

/*! \brief Fonction qui applique la règle d'apprentissage au voisinage de bmu.   
 *  \param bmu le neuron best match unit.
 *  \param network la map complete.
 *  \param ray le rayon pour déterminé le voisinage.
 *  \param alpha le facteur d'apprentissage.
 *  \param v_data la données qu'on veut faire apprendre.
 *  \param nmem_size la taille de vecteur de donnée et de vecteur mémoire.
 *  \param bubble le type de mise à jour de voisinage si bubble = 0 donc on 
 *                  applique une gaussienne sinon on applique bubble.
*/
void apply_learning_rule(neuron_t *bmu, network_t *network, int ray, double alpha, double *v_data, int nmem_size, int bubble)
{
    double d = 0.0, gauss = 0.0, r2 = (2.0 * ray * ray);
    int begin_line, begin_row, end_row, end_line;
    begin_line = maxInt(0, bmu->l - ray);
    end_line = minInt(network->nl - 1, bmu->l + ray);
    begin_row = maxInt(0, bmu->c - ray);
    end_row = minInt(network->nc - 1, bmu->c + ray);
    neuron_t **neurons = network->neurons;
    for (int iline = begin_line; iline <= end_line; iline++)
    {
        for (int icol = begin_row; icol <= end_row; icol++)
        {
            double *ptr = neurons[iline][icol].data;
            if (bubble == 0)
            {
                d = sqrt(pow(neurons[iline][icol].l - bmu->l, 2.0) + pow(neurons[iline][icol].c - bmu->c, 2.0));
                gauss = exp(-d / r2);
                for (int ivec = 0; ivec < nmem_size; ++ivec)
                {
                    ptr[ivec] += (double)alpha * gauss * (v_data[ivec] - ptr[ivec]);
                }
            }
            else
            {
                for (int ivec = 0; ivec < nmem_size; ++ivec)
                {
                    ptr[ivec] += (double)alpha * (v_data[ivec] - ptr[ivec]);
                }
            }
        }
    }
}

/*! \brief Fonction d'apprentissage de réseau.   
 *  \param network la map complete.
 *  \param db la base de données à apprendre.
 *  \param alpha le facteur d'apprentissage.
 *  \param ray le rayon pour déterminé le voisinage.
 *  \param total_iter le nombre total d'itération.
 *  \param nu détermine si on déminue le voisinage ou pas nu = 0 
 *            le voisinage décrémente, nu = -1 le voisinage reste fixe.
 *  \param shuffle_iter détermine si on mélange les données à chaque itération ou pas 
 *                      shuffle_iter = 0 on mélange une seule fois sinon à chaque itération.
 *  \param bubble le type de mise à jour de voisinage si bubble = 0 donc on 
 *                  applique une gaussienne sinon on applique bubble.
*/
void learning_phase(network_t *network, iris_t *db, double alpha, int ray, int total_iter, int nu, int shuffle_iter, int bubble)
{
    int ndata = db->ndata, neurons_number = network->nn,
        nl_neurons = network->nl, nc_neurons = network->nc,
        nfields = db->nfields, neighbour_update = (int)((double)total_iter / ray),
        neiber = ray, idx = 0;
    double i_alpha = alpha;
    data_t *data = db->data;
    int *vec_indices = generate_indices(ndata);
    bmu_t *vec = (bmu_t *)calloc(neurons_number, sizeof *vec);
    neuron_t **neurons = network->neurons;
    bmu_t best;
    shuffle(vec_indices, ndata);
    for (int iter = 1; iter <= total_iter; ++iter)
    {
        i_alpha = alpha * (1.0 - (double)iter / (double)total_iter);
        neiber -= ((neiber > 1) && (iter % neighbour_update == nu)) ? 1 : 0;
        if (shuffle_iter == 1)
        {
            shuffle(vec_indices, ndata);
        }
        for (int idata = 0; idata < ndata; ++idata)
        {
            idx = 0;
            for (int iline_neuron = 0; iline_neuron < nl_neurons; ++iline_neuron)
            {
                for (int icol_neuron = 0; icol_neuron < nc_neurons; ++icol_neuron)
                {
                    vec[idx].c = neurons[iline_neuron][icol_neuron].c;
                    vec[idx].l = neurons[iline_neuron][icol_neuron].l;
                    vec[idx].db_index = vec_indices[idata];
                    vec[idx].act_state = distance(neurons[iline_neuron][icol_neuron].data, data[vec_indices[idata]].vec, nfields);
                    ++idx;
                }
            }
            best_unit(&best, vec, neurons_number, 0, compar);
            double *v_data = data[best.db_index].vec;
            apply_learning_rule(&(neurons[best.l][best.c]), network, neiber, i_alpha, v_data, nfields, bubble);
        }
    }
    free(vec);
    free(vec_indices);
}

/*! \brief Fonction déterminant la classe à partir d'une chainne de caractere et retourner un entier.   
 *  \param label chaine de caractère représentant le label.
*/
int get_class(char *label)
{
    char *classes[] = {"Iris-setosa", "Iris-versicolor", "Iris-virginica"};
    for (int iclass = 0; iclass < 3; ++iclass)
    {
        if (strncmp(label, classes[iclass], strlen(label)) == 0)
        {
            return iclass;
        }
    }
    return -1;
}

/*! \brief Fonction d'étiquetage de réseau.   
 *  \param network la map complete.
 *  \param db la base de données d'apprentissage.
*/
void tag_network(network_t *network, iris_t *db)
{
    int nl_neurons = network->nl, nc_neurons = network->nc, cat = 0,
        ndata = db->ndata, nfields = db->nfields;
    neuron_t **neurons = network->neurons;
    data_t *data = db->data;
    bmu_t best;
    bmu_t *vec = (bmu_t *)calloc(ndata, sizeof *vec);
    int *vec_indices = generate_indices(ndata);

    for (int iline_neuron = 0; iline_neuron < nl_neurons; ++iline_neuron)
    {
        for (int icol_neuron = 0; icol_neuron < nc_neurons; ++icol_neuron)
        {
            for (int idata = 0; idata < ndata; ++idata)
            {
                vec[idata].c = neurons[iline_neuron][icol_neuron].c;
                vec[idata].l = neurons[iline_neuron][icol_neuron].l;
                vec[idata].db_index = vec_indices[idata];
                vec[idata].act_state = distance(neurons[iline_neuron][icol_neuron].data,
                                                data[vec_indices[idata]].vec, nfields);
            }
            best_unit(&best, vec, ndata, 0, compar);
            cat = get_class(data[best.db_index].label);
            neurons[iline_neuron][icol_neuron].label = cat;
            neurons[iline_neuron][icol_neuron].act = best.act_state;
        }
    }
    free(vec);
    free(vec_indices);
}

/*! \brief Fonction de test de réseau qui à chaque fois le réseau trouve 
           la classe d'une donné on augmente une variable que on revoi a la fin et qui représente le nombre de bonnes réponse.   
 *  \param network la map complete.
 *  \param db la base de données avec quoi on teste.
*/
int test_network(network_t *network, iris_t *db_test)
{
    int nl_neurons = network->nl, nc_neurons = network->nc, idx = 0,
        cat = 0, ndata = db_test->ndata, nfields = db_test->nfields,
        neurons_number = network->nn, test_result = 0;

    neuron_t **neurons = network->neurons;
    data_t *data = db_test->data;
    bmu_t best;
    bmu_t *vec = (bmu_t *)calloc(neurons_number, sizeof *vec);
    int *vec_indices = generate_indices(ndata);
    shuffle(vec_indices, ndata);
    for (int idata = 0; idata < ndata; ++idata)
    {
        for (int iline_neuron = 0; iline_neuron < nl_neurons; ++iline_neuron)
        {
            for (int icol_neuron = 0; icol_neuron < nc_neurons; ++icol_neuron)
            {
                idx = iline_neuron * nc_neurons + icol_neuron;
                vec[idx].c = neurons[iline_neuron][icol_neuron].c;
                vec[idx].l = neurons[iline_neuron][icol_neuron].l;
                vec[idx].db_index = vec_indices[idata];
                vec[idx].act_state = distance(neurons[iline_neuron][icol_neuron].data, data[vec_indices[idata]].vec, nfields);
            }
        }
        best_unit(&best, vec, neurons_number, 0, compar);
        cat = get_class(data[best.db_index].label);
        idx = best.l * nc_neurons + best.c;
        test_result += (neurons[best.l][best.c].label == cat) ? 1 : 0;
    }
    free(vec);
    free(vec_indices);
    return test_result;
}

/*! \brief Fonction d'affichage d'une map étiqueté.   
 *  \param network la map complete.
*/
void print_map(network_t *network)
{

    int nl = network->nl, nc = network->nc;
    neuron_t **neurons = network->neurons;
    for (int iline = 0; iline < nl; ++iline)
    {
        for (int icol = 0; icol < nc; ++icol)
        {
            printf("%s%c ", color[neurons[iline][icol].label], classes_symbol[neurons[iline][icol].label]);
        }
        printf("%s\n", DEFAULT_PRINT);
    }
}

/*! \brief Fonction de sauvegarde de réseau de neurone.
 *  \param path le chemin de sauvegarde de réseau.   
 *  \param network la map complete.
*/
void save_network(const char *path, network_t *network)
{
    int wr = 0;
    FILE *dst_file = fopen(path, "wb");
    wr += fwrite(&(network->nl), sizeof(network->nl), 1, dst_file);
    wr += fwrite(&(network->nc), sizeof(network->nc), 1, dst_file);
    wr += fwrite(&(network->nfields), sizeof(network->nfields), 1, dst_file);
    neuron_t **neuron = network->neurons;
    for (int iline = 0; iline < network->nl; ++iline)
    {
        for (int icol = 0; icol < network->nc; ++icol)
        {
            wr += fwrite(neuron[iline][icol].data, sizeof(*neuron[iline][icol].data), 4, dst_file);
            wr += fwrite(&(neuron[iline][icol].label), sizeof(neuron[iline][icol].label), 1, dst_file);
        }
    }
    fclose(dst_file);
}

/*! \brief Fonction d'imporatation d'un réseau de neurone entrainé et sauvegardé auparavant et renvoi ce dernier.
 *  \param path le chemin de réseau.   
*/
network_t *import_network(const char *path)
{
    int reads = 0;
    network_t *network = (network_t *)malloc(sizeof *network);
    FILE *src_file = fopen(path, "rb");
    reads = fread(&(network->nl), sizeof(network->nl), 1, src_file);
    reads += fread(&(network->nc), sizeof(network->nc), 1, src_file);
    reads += fread(&(network->nfields), sizeof(network->nfields), 1, src_file);
    network->nn = network->nl * network->nc;
    network->neurons = (neuron_t **)calloc(network->nl, sizeof *network->neurons);
    for (int iline = 0; iline < network->nl; ++iline)
    {
        network->neurons[iline] = (neuron_t *)calloc(network->nc, sizeof *network->neurons[iline]);
        for (int icol = 0; icol < network->nc; ++icol)
        {
            network->neurons[iline][icol].l = iline;
            network->neurons[iline][icol].c = icol;
            network->neurons[iline][icol].data =
                (double *)calloc(network->nfields,
                                 sizeof *network->neurons[iline][icol].data);
            reads += fread((network->neurons[iline][icol].data),
                           sizeof(network->neurons[iline][icol].data), network->nfields, src_file);
            reads += fread(&(network->neurons[iline][icol].label),
                           sizeof(network->neurons[iline][icol].label), 1, src_file);
        }
    }
    fclose(src_file);
    return network;
}

void network_to_image(network_t *network, const char *filename)
{
    int c[3][3] = {
        {255, 0, 0},
        {0, 255, 0},
        {0, 0, 255}};
    int h = network->nl * 100, w = network->nc * 100;
    SDL_Surface *s = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA8888);
    int icolor = 0;
    neuron_t **neurons = (network->neurons);
    uint8_t *pix = (uint8_t *)s->pixels;
    int ipix = 0;
    SDL_LockSurface(s);
    for (int iline = 0; iline < network->nl; ++iline)
    {
        for (int icol = 0; icol < network->nc; ++icol)
        {
            for (int ilpix = iline * 100; ilpix < iline * 100 + 100; ++ilpix)
            {
                for (int icpix = icol * 100; icpix < icol * 100 + 100; ++icpix)
                {
                    ipix = ilpix * w + icpix;
                    icolor = 4 * ipix;
                    pix[icolor] = (uint8_t)maxInt((int)(255.0 - (neurons[iline][icol].act * 1000.0)), 0);
                    pix[icolor + 1] = (uint8_t)c[neurons[iline][icol].label][0];
                    pix[icolor + 2] = (uint8_t)c[neurons[iline][icol].label][1];
                    pix[icolor + 3] = (uint8_t)c[neurons[iline][icol].label][2];
                }
            }
        }
    }
    SDL_UnlockSurface(s);
    //    IMG_SavePNG(s, filename);
    SDL_FreeSurface(s);
}
