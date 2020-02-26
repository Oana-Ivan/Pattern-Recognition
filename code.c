#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    unsigned char B, G, R;
} Pixel;



// incarcare imagine BMP in memoria interna in forma liniarizata
//void liniarizare (const char *cale_imagine, Pixel **L, unsigned char **H, unsigned char *val_padding )
//{
//    FILE *f = fopen(cale_imagine, "rb");
//    if (f == NULL) {
//        printf("Imaginea nu a fost gasita.\n");
//        return;
//    }
//    (*H) = (unsigned char*)malloc(54 * sizeof(unsigned char));
//    if ((*H) == NULL) {
//        printf("Nu exista suficienta memorie pentru a memora header-ul imaginii.\n");
//        return;
//    }
//    int i, j, k = 0;
//    for (i = 0; i < 54; i++) fread(&((*H)[i]), sizeof(unsigned char), 1, f);
//
//    int latime, inaltime;
//    fseek(f, 18, SEEK_SET);
//    fread(&latime, sizeof(int), 1, f);
//    fread(&inaltime, sizeof(int), 1, f);
//
//    unsigned char padding;
//    if ((3*latime)%4 == 0) padding = 0;
//    else {
//        padding = 4 - (3 * latime)%4;
//        fseek (f, -1, SEEK_END);
//        fread (&(*val_padding), sizeof(unsigned char), 1, f);
//    }
//    (*L) = (Pixel *)malloc(latime * inaltime * sizeof(Pixel));
//    if ((*L) == NULL) {
//        printf("Nu exista suficienta memorie pentru a memora imaginea in forma liniarizata.\n");
//        return;
//    }
//    fseek(f, -(3 * latime + padding), SEEK_END);
//
//    for (i = 0; i < inaltime; i++) {
//        for (j = 0; j < latime; j++) {
//            fread(&(*L)[k].B,sizeof(unsigned char), 1, f);
//            fread(&(*L)[k].G,sizeof(unsigned char), 1, f);
//            fread(&(*L)[k].R,sizeof(unsigned char), 1, f);
//            k++;
//        }
//        fseek(f, -(2*3*latime+padding), SEEK_CUR);
//    }
//    fclose(f);
//}


// salvarea unei imagini BMP in memoria externa
//void salvare_memorie_externa(const char *cale_imagine, Pixel *L, unsigned char *H, unsigned char val_padding)
//{
//    FILE *f = fopen(cale_imagine,"wb+");
//    if (f == NULL) {
//        printf("Imaginea nu s-a putut deschide.\n");
//        return;
//    }
//    int i;
//    for (i = 0; i < 54; i++) fwrite(&H[i], sizeof(unsigned char), 1, f);
//
//    int latime, inaltime;
//    fseek (f, 18, SEEK_SET);
//    fread (&latime, sizeof(int), 1, f);
//    fread (&inaltime, sizeof(int), 1, f);
//
//    unsigned int padding;
//    if ((3 * latime)%4 == 0) padding = 0;
//    else padding = 4-(3 * latime)%4;
//
//    fseek (f, 54, SEEK_SET);
//    int k = 1;
//    while (k <= inaltime) {
//        for (i = latime * inaltime - k * latime; i < latime * inaltime + (1 - k) * latime; i++)
//            {
//                fwrite (&L[i].B, sizeof(unsigned char), 1, f);
//                fwrite (&L[i].G, sizeof(unsigned char), 1, f);
//                fwrite (&L[i].R, sizeof(unsigned char), 1, f);
//            }
//        unsigned int aux;
//        aux = padding;
//        while (aux != 0) {
//            fwrite (&val_padding, sizeof(unsigned char), 1, f);
//            aux--;
//        }
//        k++;
//    }
//    fclose(f);
//}
//

typedef struct {
    unsigned int coordX, coordY;
    double corelatie;
    Pixel culoare;
} Fereastra;

void salvare_in_matrice_greyscale(const char * nume_imagine, unsigned char *** Matrice,
                                   int *latime_img, int *inaltime_img) {
    FILE *img = fopen(nume_imagine,"rb");
    if (img == NULL) {
        printf ("Nu s-a gasit imaginea %s.\n", nume_imagine);
        return;
    }
    fseek(img, 18, SEEK_SET);
    int padding_img, i, j;
    fread(&(*latime_img), sizeof(int), 1, img);
    fread(&(*inaltime_img), sizeof(int), 1, img);
    if ((*latime_img)%4 == 0) padding_img = 0;
    else padding_img = 4 - (3 * (*latime_img)) % 4;

    (*Matrice) = (unsigned char **)malloc((*inaltime_img) * sizeof(unsigned char *));
    for (i = 0; i < (*inaltime_img); i++)
        (*Matrice)[i] = (unsigned char *)malloc((*latime_img) * sizeof(unsigned char));

    unsigned char r, g, b, aux;
    fseek(img, -(3*(*latime_img) + padding_img), SEEK_END);
    for (i = 0; i < (*inaltime_img); i++) {
        for (j = 0; j < (*latime_img); j++) {
            fread(&b, sizeof(unsigned char), 1, img);
            fread(&g, sizeof(unsigned char), 1, img);
            fread(&r, sizeof(unsigned char), 1, img);
            aux = 0.299*r + 0.587*g + 0.114*b;
            (*Matrice)[i][j] = aux;
        }
        fseek(img,-(2*3*(*latime_img) + padding_img), SEEK_CUR);
    }
    fclose(img);
}

void deviatie_standard(int x, int y, int latime, int inaltime, unsigned char **Matrice,
                        double n, double *sigma, double *medie) {
    (*medie) = 0;
    int i, j;
    for ( i = x; i < x + inaltime; i++)
        for (j = y; j < y + latime; j++) {
                (*medie) += (double)Matrice[i][j];
        }
    (*medie) = (*medie)/n;

    double suma = 0;
    for (i = x; i < x + inaltime; i++)
        for (j = y; j < y + latime; j++)
            suma += ((double)Matrice[i][j] - (*medie)) * ((double)Matrice[i][j] - (*medie));
    (*sigma) = sqrt((1 / (n-1)) * suma);
}
double calcul_corelatie(int latime, int inaltime, int x, int y, unsigned char **Imagine, unsigned char **Sablon,
                        double medieF, double medieS, double sigmaF, double sigmaS, double n) {
    double corr, sumaCorr;
    int i, j;
    sumaCorr = 0;
    for (i = 0; i < inaltime; i++)
        for (j = 0; j < latime; j++) {
       sumaCorr += (double)((double)Imagine[x+i][y+j] - medieF) * ((double)Sablon[i][j] - medieS);
       }
    sumaCorr = sumaCorr/(sigmaF * sigmaS);
    corr = (1/n) * sumaCorr;
    return corr;
}
// operatie de template matching dintre o imagine si un sablon
void template_matching (const char *nume_imagine, const char *nume_sablon, double ps,
                        Fereastra **fi, int *nrFerestre, Pixel culoare) {
    unsigned char **Imagine, **Sablon;
    int latime_img, inaltime_img, latime_sablon, inaltime_sablon;
    salvare_in_matrice_greyscale(nume_imagine, &Imagine, &latime_img, &inaltime_img);
    salvare_in_matrice_greyscale(nume_sablon, &Sablon, &latime_sablon, &inaltime_sablon);

    double n, sigmaS, sigmaF, medieS, medieF, corr;
    n = (double)latime_sablon * (double)inaltime_sablon;
    deviatie_standard(0, 0, latime_sablon, inaltime_sablon, Sablon, n, &sigmaS, &medieS);

    (*nrFerestre) = 0; //nr de ferestre cu corelatia mai mare decat ps
    int dimensiune_fi = inaltime_img * latime_img * sizeof(Fereastra);
    (*fi) = (Fereastra *)malloc(dimensiune_fi); //i se aloca nr maxim de cazuri posibile urmand sa se dezaloce

    int x, y;
    for (x = 0; x < inaltime_img - inaltime_sablon; x++)
        for(y = 0; y < latime_img - latime_sablon; y++) {
            deviatie_standard(x, y, latime_sablon, inaltime_sablon, Imagine, n, &sigmaF, &medieF);
            corr = calcul_corelatie(latime_sablon, inaltime_sablon, x, y, Imagine, Sablon, medieF, medieS, sigmaF, sigmaS, n);
            if(corr >= ps) {
                (*nrFerestre)++;
                (*fi)[(*nrFerestre) - 1].coordX = x;
                (*fi)[(*nrFerestre) - 1].coordY = y;
                (*fi)[(*nrFerestre) - 1].corelatie = corr;
                (*fi)[(*nrFerestre) - 1].culoare.R = culoare.R;
                (*fi)[(*nrFerestre) - 1].culoare.G = culoare.G;
                (*fi)[(*nrFerestre) - 1].culoare.B = culoare.B;
            }
            else {
                dimensiune_fi = dimensiune_fi - sizeof(Fereastra);
                (*fi) = realloc((*fi), dimensiune_fi);
            }
        }
    for (x = 0; x < inaltime_img; x++) free(Imagine[x]);
    free(Imagine);

    for (x = 0; x < inaltime_sablon; x++) free(Sablon[x]);
    free(Sablon);
}

void salvare_in_matrice(const char * nume_imagine, Pixel *** Matrice, int *latime_img,
                        int *inaltime_img, unsigned char *val_padding) {
    FILE *img = fopen(nume_imagine,"rb");
    if (img == NULL) {
        printf("Nu s-a gasit imaginea.\n");
        return;
    }

    fseek(img, 18, SEEK_SET);
    int padding_img, i, j;
    fread(&(*latime_img), sizeof(int), 1, img);
    fread(&(*inaltime_img), sizeof(int), 1, img);
    if ((*latime_img)%4 == 0) padding_img = 0;
    else {
        padding_img = 4 - (3 * (*latime_img))%4;
        fseek(img, -1, SEEK_END);
        fread(&(*val_padding), sizeof(unsigned char), 1, img);
    }

    (*Matrice) = (Pixel **)malloc((*inaltime_img) * sizeof(Pixel *));
    for (i = 0; i < (*inaltime_img); i++)
        (*Matrice)[i] = (Pixel *)malloc((*latime_img) * sizeof(Pixel));

    fseek(img, -(3 * (*latime_img) + padding_img), SEEK_END);
    for (i = 0; i < (*inaltime_img); i++) {
        for (j = 0; j < (*latime_img); j++) {
            fread(&(*Matrice)[i][j].B, sizeof(unsigned char), 1, img);
            fread(&(*Matrice)[i][j].G, sizeof(unsigned char), 1, img);
            fread(&(*Matrice)[i][j].R, sizeof(unsigned char), 1, img);
        }
        fseek(img, -(2 * 3 * (*latime_img) + padding_img), SEEK_CUR);
    }
    fclose(img);
}

void salvare_in_fisier(const char * nume_imagine, Pixel *** Matrice, unsigned char val_padding) {
    FILE *img = fopen(nume_imagine, "rb+");
    if (img == NULL) {
        //printf("Nu s-a gasit imaginea %s.\n",nume_imagine);
        return;
    }

    fseek(img,18,SEEK_SET);
    int latime_img, inaltime_img, padding_img, i, j;
    fread(&latime_img, sizeof(int), 1, img);
    fread(&inaltime_img, sizeof(int), 1, img);
    if (latime_img % 4 == 0) padding_img = 0;
    else padding_img = 4 - (3*latime_img)%4;

    fseek(img, 54, SEEK_SET);
    for (i = inaltime_img - 1; i >= 0; i--) {
            for (j = 0; j < latime_img; j++) {
                    fwrite(&(*Matrice)[i][j].B, sizeof(unsigned char), 1, img);
                    fwrite(&(*Matrice)[i][j].G, sizeof(unsigned char), 1, img);
                    fwrite(&(*Matrice)[i][j].R, sizeof(unsigned char), 1, img);
                }
            if (padding_img != 0) fwrite(&val_padding, sizeof(unsigned char), padding_img, img);
        }

    for (i = 0; i < inaltime_img; i++) free((*Matrice)[i]);
    free(*Matrice);
    fclose(img);
}
void desenare_fereastra(const char *nume_imagine, Fereastra f, Pixel C, int inaltime_fereastra, int latime_fereastra) {
    Pixel **Imagine;
    int latime_imagine, inaltime_imagine;
    unsigned char val_padding;
    salvare_in_matrice(nume_imagine, &Imagine, &latime_imagine, &inaltime_imagine, &val_padding);

    int i;
    for (i = f.coordY; i < f.coordY + latime_fereastra; i++) {
        Imagine[f.coordX][i].R = C.R;
        Imagine[f.coordX][i].G = C.G;
        Imagine[f.coordX][i].B = C.B;

        Imagine[f.coordX + inaltime_fereastra - 1][i].R = C.R;
        Imagine[f.coordX + inaltime_fereastra - 1][i].G = C.G;
        Imagine[f.coordX + inaltime_fereastra - 1][i].B = C.B;
    }

    for (i = f.coordX; i < f.coordX + inaltime_fereastra; i++) {
        Imagine[i][f.coordY].R = C.R;
        Imagine[i][f.coordY].G = C.G;
        Imagine[i][f.coordY].B = C.B;

        Imagine[i][f.coordY + latime_fereastra - 1].R = C.R;
        Imagine[i][f.coordY + latime_fereastra - 1].G = C.G;
        Imagine[i][f.coordY + latime_fereastra - 1].B = C.B;
    }

    salvare_in_fisier(nume_imagine,&Imagine,val_padding);

}
int cmpCorelatii(const void *a, const void *b)
{
    double corelatie1 = ((Fereastra *)a)->corelatie;
    double corelatie2 = ((Fereastra *)b)->corelatie;
    if(corelatie1 > corelatie2) return -1;
    if(corelatie1 < corelatie2) return 1;
    return 0;
}
// sortarea tabloului Detectii in ordinea descrescatoare a corelatiilor detectiilor
void sortare_detectii_descrecator(Fereastra ** Detectii, int nr_detectii) {
    qsort((*Detectii), nr_detectii, sizeof(Fereastra), cmpCorelatii);
}
void reunire_detectii(Fereastra **Detectii, int *nrDetectii) {
    char *nume_fisier_date;
    nume_fisier_date = (char *)malloc(256*sizeof(char));
    printf("Introduceti numele fisierului care contine numele imaginii, numele sabloanelor, numarul si codul culorilor acestora: ");
    scanf("%s",nume_fisier_date);
    unsigned int dimensiune_nume_fisier = strlen(nume_fisier_date);
    nume_fisier_date = (char *)realloc(nume_fisier_date, (dimensiune_nume_fisier + 1) * sizeof(char));

    FILE *date = fopen(nume_fisier_date, "r");
    if (date == NULL) {
        printf("Nu s-a gasit fisierul %s.\n", nume_fisier_date);
        return;
    }
    (*Detectii) = (Fereastra *) malloc(sizeof(Fereastra));
    (*nrDetectii) = 0;

    char *nume_imagine;
    nume_imagine = (char *)malloc(256 * sizeof(char));
    fscanf(date, "%s", nume_imagine);
    unsigned int dimensiune_nume_imagine = strlen(nume_imagine);
    nume_imagine = (char *)realloc(nume_imagine, (dimensiune_nume_imagine + 1) * sizeof(char));

    int nr_sabloane;
    fscanf(date, "%d", &nr_sabloane);

    char *nume_sablon;
    unsigned int dimensiune_nume_sablon;
    int i, j, poz, nrFerestre;
    Fereastra *fi;
    Pixel culoare;
    while (nr_sabloane != 0) {
        nume_sablon = (char *)malloc(256 * sizeof(char));
        fscanf(date, "%s", nume_sablon);
        dimensiune_nume_sablon = strlen(nume_sablon);
        nume_sablon = (char *)realloc(nume_sablon, (dimensiune_nume_sablon + 1) * sizeof(char));

        fseek(date, 1, SEEK_CUR);
        int r, g, b;
        fscanf(date, "%d", &r); culoare.R = (char)r;
        fseek(date, 1, SEEK_CUR);
        fscanf(date, "%d", &g); culoare.G = (char)g;
        fseek(date, 1, SEEK_CUR);
        fscanf(date, "%d", &b); culoare.B = (char)b;
        //printf("Culoare pt %s: %u, %u, %u\n",nume_sablon,culoare.R, culoare.G, culoare.B);

        template_matching(nume_imagine, nume_sablon, 0.5, &fi, &nrFerestre, culoare);
        poz = (*nrDetectii);
        (*nrDetectii) = (*nrDetectii) + nrFerestre;
        (*Detectii) = (Fereastra *)realloc((*Detectii), (*nrDetectii) * sizeof(Fereastra));

        for((i = poz) && (j = 0); (i < (*nrDetectii)) && (j < nrFerestre); (i++) && (j++)) {
            (*Detectii)[i].coordX = fi[j].coordX;
            (*Detectii)[i].coordY = fi[j].coordY;
            (*Detectii)[i].corelatie = fi[j].corelatie;
            (*Detectii)[i].culoare.R = fi[j].culoare.R;
            (*Detectii)[i].culoare.G = fi[j].culoare.G;
            (*Detectii)[i].culoare.B = fi[j].culoare.B;
        }
        free(fi);
        nr_sabloane--;
    }
    fclose(date);
}
int min (int a, int b) {
    if (a < b) return a;
    return b;
}
int max (int a, int b) {
    if(a > b) return a;
    return b;
}
double suprapunere_detectii (Fereastra D1, Fereastra D2, int latime, int inaltime) {
    struct Punct {
        int x, y;
    };
    struct Punct D1ss, D1id; //D1ss punctul din coltul superior din stanga al dreptunghiului D1 si
                             //D1id punctul din coltul inferior din din dreapta al dreptunghiului D1
    struct Punct D2ss, D2id; //D2xs punctul din coltul superior din stanga al dreptunghiului D2 si
                             //D2id punctul din coltul inferior din din dreapta al dreptunghiului D2
    D1ss.x = D1.coordX;
    D1ss.y = D1.coordY;
    D1id.x = D1.coordX + inaltime;
    D1id.y = D1.coordY + latime;

    D2ss.x = D2.coordX;
    D2ss.y = D2.coordY;
    D2id.x = D2.coordX + inaltime;
    D2id.y = D2.coordY + latime;

    double arie_D1 = (double) abs(D1ss.x - D1id.x) * abs(D1ss.y - D1id.y);
    double arie_D2 = (double) abs(D2ss.x - D2id.x) * abs(D2ss.y - D2id.y);
    double arie_D1_intersectat_cu_D2 = ((double)min(D1id.x, D2id.x) - (double)max(D1ss.x, D2ss.x)) *
                                       ((double)min(D1id.y, D2id.y) - (double)max(D1ss.y, D2ss.y) );
    double arie_D1_reunit_cu_D2 = (double)arie_D1 + (double)arie_D2 - (double)arie_D1_intersectat_cu_D2;
    double suprapunere = (1.0/(double)arie_D1_intersectat_cu_D2) * (double)arie_D1_reunit_cu_D2;
    return suprapunere;
}
// Rezolvare cerinta 10
void eliminare_non_maxime (Fereastra **Detectii, int *nrDetectii, int latime, int inaltime) {
    int i, j;
    for (i = 0;i < (*nrDetectii) - 1; i++)
        for (j = i + 1; j < (*nrDetectii); j++)
            if ((*Detectii)[j].corelatie != 100 && suprapunere_detectii((*Detectii)[i], (*Detectii)[j], latime, inaltime) > 0.2)
                (*Detectii)[j].corelatie = 100;

}
int main() {
    //Rulare operatia de template matching pentru toate sabloanele si reunirea detectiilor gasite
    Fereastra *Detectii;
    int nrDetectii;
    reunire_detectii(&Detectii, &nrDetectii);

    //sortare detectii folosind qsort
    sortare_detectii_descrecator(&Detectii, nrDetectii);

    //rulare functie non maxime
    //dimensiuni chenare
    int latime = 11, inaltime = 15;
    eliminare_non_maxime(&Detectii, &nrDetectii, latime, inaltime);

    //desenare ferestre
    char *nume_fisier_date;
    nume_fisier_date = (char *)malloc(256 * sizeof(char));
    printf("Reintroduceti numele fisierului care contine numele imaginii, numele sabloanelor, numarul si codul culorilor acestora: ");
    scanf("%s",nume_fisier_date);

    unsigned int dimensiune_nume_fisier = strlen(nume_fisier_date);
    nume_fisier_date = (char *)realloc(nume_fisier_date, (dimensiune_nume_fisier + 1) * sizeof(char));

    FILE *date = fopen(nume_fisier_date, "r");
    if(date == NULL) {
        printf("Nu s-a gasit fisierul %s.\n", nume_fisier_date);
        return 0;
    }
    char *nume_imagine;
    nume_imagine = (char *)malloc(256 * sizeof(char));
    fscanf(date, "%s", nume_imagine);
    fclose(date);
    unsigned int dimensiune_nume_imagine = strlen(nume_imagine);
    nume_imagine = (char *)realloc(nume_imagine, (dimensiune_nume_imagine + 1) * sizeof(char));

    int i;
    for (i = 0; i < nrDetectii; i++)
        if (Detectii[i].corelatie != 100)
            desenare_fereastra(nume_imagine, Detectii[i], Detectii[i].culoare, inaltime, latime);

    free(Detectii);
    return 0;
}
