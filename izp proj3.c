/**
 * Kostra programu pro 3. projekt IZP 2018/19
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 *
 * Nikodém Babirád
 * xbabir01
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t
{
    int id;
    float x;
    float y;
};

struct cluster_t
{
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);
    if((c->obj = malloc(cap * sizeof(struct obj_t)))==NULL)
    {
        fprintf(stderr, "Nedostatocna pamat!\n" );
    }
    c->size = 0;
    c->capacity = cap;
}



/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    assert(c);
    free(c->obj);
    c->capacity = 0;
    c->size = 0;
}

void clear_all(int pocet, struct cluster_t *clusters)
{
    for(int i = 0; i < pocet; i++)
        clear_cluster(&clusters[i]);   // uvolni pamat po objektoch vo vyslednom clusteri
    free(clusters);   // uvolni pamat po clusteri
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if (c->size == c->capacity)   // ak sa velkost rovna kapacite, je treba zvysit kapacitu
    {
        if (resize_cluster(c, c->capacity + CLUSTER_CHUNK) == NULL) //kontroluje uspesnu zmenu kapacity
            fprintf(stderr, "Nepodarila sa zmena kapacity!\n");
    }
    c->obj[c->size] = obj;
    c->size++;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);
    if(c1->capacity < (c1->size + c2->size))
        resize_cluster(c1, c1->size + c2->size);

    for(int i=0; i < c2->size; i++)
        append_cluster(c1, c2->obj[i]); // Pridá na koniec clustera c1 objekt z clustera c2

    sort_cluster(c1); //Zoradi objekty v clusteri c1
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);
    clear_cluster(&carr[idx]); //Uvolni pamat po poli
    for(int i = 0; i < (narr-1); i++)
    {
        if(i >= idx)
            carr[i]=carr[i + 1];
    }
    return narr-1;  //Pocet clusterov po odstraneni
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);
    float x = o1->x - o2->x;
    float y = o1->y - o2->y;
    return sqrtf((x*x) + (y*y));
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);
    float vzdialenost = 0.0;
    float minvzdialenost=1500.0;
    for (int i = 0; i < c1 -> size; i++) //porovna vsetky objekty oboch clusterov
    {
        for (int j = 0; j < c2 -> size; j++)
        {
            vzdialenost = obj_distance(&c1->obj[i], &c2->obj[j]);
            if(vzdialenost<minvzdialenost)
               minvzdialenost = vzdialenost;
        }
    }
    return minvzdialenost;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);
    float vzdialenost = 0;
    float minvzdialenost = 1500;
    *c1 = 0, *c2 = 1;
    for(int i = 0; i < narr; i++)
    {
        for(int j = 0; j < narr; j++)
        {
            if (i!=j) // nemoze sa rovnat aby sa neporovnaval sam so sebou
            {
                vzdialenost = cluster_distance(&carr[i], &carr[j]);
                if (vzdialenost < minvzdialenost)
                {
                    minvzdialenost = vzdialenost;
                    *c1 = i;
                    *c2 = j;
                }
            }
        }
    }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id)
        return -1;
    if (o1->id > o2->id)
        return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i)
            putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);
    FILE *subor;
    int id;
    float xtmp, ytmp;
    int pocet = 0;
    char c;
    int count;
    if ((subor = fopen(filename, "r")) == NULL)   // kontroluje spravne otvorenie suboru
    {
        fprintf(stderr, "Nepodarilo sa otvorit subor!\n");
        return -1;
    }

    if (fscanf(subor, "count=%d", &count) != 1)   // kontroluje spravne nacitanie poctu objektov
    {
        fprintf(stderr, "Nespravne zadany count!\n");
        if (fclose(subor) == EOF)   // kontroluje spravne zavretie suboru
            fprintf(stderr, "Vstupny subor sa nepodarilo zavriet!\n");
        return -1;
    }
    *arr = malloc(count * sizeof(struct cluster_t)); // alokuje pamat pre pole clusteru
    if (*arr == NULL)   // kontroluje uspesnost alokacie pamate
    {
        fprintf(stderr, "Nepodarilo sa alokovat pamat!\n");
        if (fclose(subor) == EOF)    // kontroluje spravne zavretie suboru
            fprintf(stderr, "Vstupny soubor sa nepodarilo zavriet!\n");
        clear_all(pocet, *arr);   //uvolni pamat pred zavretim
        return -1;
    }

    for (int i = 0; i < count; i++)
    {
        init_cluster(&(*arr)[i], 1);
        (*arr)[i].size = 1; // nastavi na zaciatku kazdy cluster s 1 objektom
        pocet++; // zvysi pocet clusterov
        c = getc(subor);
        if ((*arr)[i].obj == NULL) // kontrola alokacie pamati pre obj
        {
            fprintf(stderr, "Nepodarilo sa alokovat pamat!");
            if (fclose(subor) == EOF)   // kontroluje spravne zavretie suboru
                fprintf(stderr, "Vstupny subor sa nepodarilo zavriet!\n");
            clear_all(pocet, *arr);
            return -1;
        }

        if (c != '\n' && i != count - 1)   // kontrola spravneho formatu
        {
            fprintf(stderr, "Vstupny subor je v zlom formate!");
            if (fclose(subor) == EOF)   // kontroluje spravne zavretie suboru
                fprintf(stderr, "Vstupny subor sa nepodarilo zavriet!\n");
            clear_all(pocet, *arr);
            return -1;
        }

        if (fscanf(subor,"%d %f %f", &id, &xtmp, &ytmp) == 3)   // kontrola nacitania 3parametrov pre kazdy objekt
        {
            if (xtmp < 0 || ytmp < 0 || xtmp > 1000 || ytmp > 1000 || xtmp - (int)xtmp != 0 || ytmp - (int)ytmp != 0)   // kontrola intervalov a zapornych cisel
            {
                fprintf(stderr, "Neplatne suradnice objektu!");
                fprintf(stderr, "Vstupny subor je v zlom formate!");
                if (fclose(subor) == EOF)   // kontroluje spravne zavretie suboru
                    fprintf(stderr, "Vstupny subor sa nepodarilo zavriet!\n");
                clear_all(pocet, *arr);
                return -1;
            }

            for (int j = 0; j < pocet - 1; j++)   // kontrola opakovania id objektov
            {
                if ((*arr)[j].obj->id == id)
                {
                    fprintf(stderr, "Chyba! Vyskytuji se objekty se stejnym ID.");
                    if (fclose(subor) == EOF)   // kontroluje spravne zavretie suboru
                        fprintf(stderr, "Vstupny subor sa nepodarilo zavriet!\n");
                    clear_all(pocet, *arr);
                    return -1;
                }
            }
            (*arr)[i].obj->x = xtmp;   // priradenie x
            (*arr)[i].obj->y = ytmp;   // priradenie y
            (*arr)[i].obj->id = id;   // priradenie id
        }
        else   // kontrola formatu suboru
        {
            fprintf(stderr, "Vstupny subor je v zlom formate!\n");
            if (fclose(subor) == EOF)   // kontroluje spravne zavretie suboru
                fprintf(stderr, "Vstupny subor sa nepodarilo zavriet!\n");
            clear_all(pocet, *arr);
            return -1;
        }
    }

    if (fclose(subor) == EOF)   // kontrola posledneho zavretia suboru
    {
        clear_all(pocet, *arr);
        fprintf(stderr, "Vstupny subor sa nepodarilo zavriet.\n");
        return -1;
    }
    return pocet;   // vracia pocet nacitanych shlukov (v kazdom jeden objekt)
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}


int main(int argc, char *argv[])
{
    struct cluster_t *clusters;
    int narg;   // pocet vyslednych shluku
    char subor[101];   // pole pre nazov vstupneho suboru
    int pocet;   // pocet clusterov na zaciatku
    int c1, c2;   // premenne pre clustere
    char *perr;   // chybova premenna
    char *pvstup;   // premenna pre 2.argument n
    if (argc > 1 && argc < 4)   // kontrola poctu argumentov
    {
        if (strlen(argv[1]) > 100)   // ak sa nazov suboru nezmesti do pola 100 znakov
        {
            fprintf(stderr, "Vstupny subor ma prilis dlhy nazov.\n");
            return -1;
        }
        else
            strcpy(subor, argv[1]);   // ulozi nazov suboru do pola

        if (argc == 2)   // ked N nie je zadane tak berie n=1
            narg = 1;

        if (argc == 3)   // ak je zadany 3 argument
        {
            pvstup = argv[2];   // pointer na druhy argument
            narg = (int) strtol(pvstup, &perr, 10);

            if (*perr != '\0' || narg <= 0)   // kontrola spravneho pretypovania
            {
                fprintf(stderr, "Nespravne zadanie argumentu N!\n");
                return -1;
            }
        }
    }
    else   // kontroluje pocet argumentov
    {
        fprintf(stderr, "Neplatny pocet argumentov!\n");
        return -1;
    }

    if ((pocet = load_clusters(subor, &clusters)) < 0)   // chyba pri nacitani objektu ze vstupniho souboru -> chybove hlaseni
        return -1;

    if (narg > pocet) 
    {
        fprintf(stderr, "Pocet nacitanych objektov je mensi ako pocet vyslednych clusterov!\n");
        clear_all(pocet, clusters);
        return -1;
    }

    while (pocet != narg)
    {
        find_neighbours(clusters, pocet, &c1, &c2);
        merge_clusters(&clusters[c1], &clusters[c2]);
        pocet = remove_cluster(clusters, pocet, c2);
    }
    print_clusters(clusters, pocet);   // vypis clusterov
    clear_all(pocet, clusters);   // uvolneni pamat
    return 0;
}
