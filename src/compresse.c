#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "huffman.h"






// Fonction pour analyser un fichier et remplir un tableau de freq
void lire_f(const char *nom_fichier, uint32_t freq[TAILLE_ALPHABET]) 
{
    FILE *f = fopen(nom_fichier, "rb");  // Ouverture en mode binaire
    if (f == NULL) {
        printf("Erreur d'ouverture du fichier");
        exit(EXIT_FAILURE);
    }


    
    for (uint32_t i = 0; i < TAILLE_ALPHABET; i++) 
    {
        freq[i] = 0;
    }

    // Lecture du fichier et comptage des frequences
    uint8_t oct;
    while (fread(&oct, sizeof(uint8_t), 1, f) == 1) 
    {
        freq[oct]++;
    }

    fclose(f);
}



Noeud *creer_noeud(uint8_t oct, uint32_t frequence, Noeud *gauche, Noeud *droit) 
{
    Noeud *n = malloc(sizeof(Noeud));
    if (n == NULL) {
        exit(EXIT_FAILURE);
    }
    n->oct = oct;
    n->freq = frequence;
    n->g = gauche;
    n->d = droit;
    return n;
}



void ajt_file(File *f, Noeud *n) 
{
    // Creer un nouveau maillon pour le noeud
    Maillon *nouveau_maillon = malloc(sizeof(Maillon));
    nouveau_maillon->noeud = n;
    nouveau_maillon->suivant = NULL;

    // Si La file est vide
    if (f->tete == NULL) {
        f->tete = nouveau_maillon;
    }
    // Si le noeud doit etre mis en tête de file
    else if (n->freq < f->tete->noeud->freq) {
        nouveau_maillon->suivant = f->tete;
        f->tete = nouveau_maillon;
    }
    // Si le noeud doit etre mis au milieu ou a la fin de la file
    else {
        Maillon *actuel = f->tete;
        while (actuel->suivant != NULL && actuel->suivant->noeud->freq <= n->freq) {
            actuel = actuel->suivant;
        }
        nouveau_maillon->suivant = actuel->suivant;
        actuel->suivant = nouveau_maillon;
    }

    f->taille++;
}

Noeud *extraire_tete(File *f) 
{
    if (f->tete == NULL) return NULL; // File vide

    // Extraire le noeud en tete de file
    Maillon *maillon_extrait = f->tete;
    Noeud *noeud_extrait = maillon_extrait->noeud;

    
    f->tete = maillon_extrait->suivant;

    
    free(maillon_extrait);

    f->taille--;
    return noeud_extrait;
}

Noeud *creer_arbre(uint32_t freq[TAILLE_ALPHABET]) {
    File f = { .tete = NULL, .taille = 0 };

    // Mettre chaque octet non nul dans la file
    for (uint32_t i = 0; i < TAILLE_ALPHABET; i++) {
        if (freq[i] > 0) {
            ajt_file(&f, creer_noeud(i, freq[i], NULL, NULL));
            
        }
    }

    // Construire l'arbre de Huffman
    while (f.taille > 1) 
    {
        Noeud *gauche = extraire_tete(&f);
        Noeud *droit = extraire_tete(&f);
        
        Noeud *parent = creer_noeud(0, gauche->freq + droit->freq, gauche, droit);
        ajt_file(&f, parent);
        
    
    }

    Noeud *racine = extraire_tete(&f);  // Extraire la racine de l'arbre
    

    
    liberer_file(&f);

    return racine;  // Retourner la racine de l'arbre
}

void liberer_file(File *f) {
    Maillon *courant = f->tete;
    while (courant != NULL) {
        Maillon *temp = courant;
        courant = courant->suivant;
        free(temp);
    }
    f->tete = NULL;
    f->taille = 0;
}




void generer_codes(Noeud *racine, char chemin[], uint32_t longueur, TableHuffman *table) {
    if (!racine) return;

    // Si c'est une feuille (lettre avec une fréquence > 0)
    if (!racine->g && !racine->d) {
        chemin[longueur] = '\0';  // Fin de chaîne
        table->code[racine->oct] = strdup(chemin);  // Allouer et copier le code
        return;
    }

    // Descente à gauche avec '0'
    chemin[longueur] = '0';
    generer_codes(racine->g, chemin, longueur + 1, table);

    // Descente à droite avec '1'
    chemin[longueur] = '1';
    generer_codes(racine->d, chemin, longueur + 1, table);
}


void ecrire_fcompresse(const char *f_src, const char *f_dst, TableHuffman *table) {
    FILE *f_in = fopen(f_src, "rb");
    FILE *f_out = fopen(f_dst, "wb");

    if (!f_in || !f_out) {
        perror("Erreur d'ouverture de fichier");
        exit(EXIT_FAILURE);
    }

    // Écrire la table de Huffman
    uint8_t nb_symboles = 0;
    for (uint32_t i = 0; i < 256; i++) {
        if (table->code[i] != NULL) nb_symboles++;
    }

    fwrite(&nb_symboles, sizeof(uint8_t), 1, f_out);

    for (uint32_t i = 0; i < 256; i++) {
        if (table->code[i] != NULL) {
            uint8_t longueur = strlen(table->code[i]);
            fwrite(&i, sizeof(uint8_t), 1, f_out);  // Stocker l'octet
            fwrite(&longueur, sizeof(uint8_t), 1, f_out);  // Stocker la longueur du code
            fwrite(table->code[i], sizeof(char), longueur, f_out);  // Stocker le code Huffman
        }
    }

    // Écrire les données compressées en bits
    uint8_t oct, placebits = 0, bits_remplis = 0;

    while (fread(&oct, sizeof(uint8_t), 1, f_in) == 1) {
        char *code = table->code[oct];

        for (uint32_t i = 0; code[i] != '\0'; i++) {
            placebits = (placebits << 1) | (code[i] - '0');
            bits_remplis++;

            if (bits_remplis == 8) {
                fwrite(&placebits, sizeof(uint8_t), 1, f_out);
                placebits = 0;
                bits_remplis = 0;
            }//printf(" codage : '%c' (0x%02X) -> %s\n", oct, oct, code);
        }
    }

    // Écrire les bits restants (si < 8)
    if (bits_remplis > 0) {
        placebits <<= (8 - bits_remplis);
        fwrite(&placebits, sizeof(uint8_t), 1, f_out);
    }

    fclose(f_in);
    fclose(f_out);
}


void compresser_bmp(const char *f_src, const char *f_dst) {
    FILE *f_in = fopen(f_src, "rb");
    FILE *f_out = fopen(f_dst, "wb");

    if (!f_in || !f_out) {
        perror("Erreur d'ouverture de fichier");
        exit(EXIT_FAILURE);
    }

    // Lire l'en-tête BMP (54 octets)
    uint8_t entete[54];
    fread(entete, sizeof(uint8_t), 54, f_in);

    // Écrire l'en-tête dans le fichier compressé
    fwrite(entete, sizeof(uint8_t), 54, f_out);

    // Analyser et compresser le reste du fichier
    uint32_t freq[TAILLE_ALPHABET] = {0};
    lire_f(f_src, freq);

    Noeud *racine = creer_arbre(freq);

    TableHuffman table;
    char chemin[32];
    generer_codes(racine, chemin, 0, &table);

    ecrire_fcompresse(f_src, f_dst, &table);

    liberer_arbre(racine);
    liberer_table_huffman(&table);


    fclose(f_in);
    fclose(f_out);
}

void compresser_jpg(const char *f_src, const char *f_dst) {
    FILE *f_in = fopen(f_src, "rb");
    FILE *f_out = fopen(f_dst, "wb");

    if (!f_in || !f_out) {
        perror("Erreur d'ouverture de fichier");
        exit(EXIT_FAILURE);
    }

    // Analyser le fichier pour obtenir les fréquences
    uint32_t freq[TAILLE_ALPHABET] = {0};
    lire_f(f_src, freq);

    // Construire l'arbre de Huffman
    Noeud *racine = creer_arbre(freq);

    // Générer les codes de Huffman
    TableHuffman table;
    char chemin[32];
    generer_codes(racine, chemin, 0, &table);

    // Écrire le fichier compressé
    ecrire_fcompresse(f_src, f_dst, &table);

    // Libérer l'arbre de Huffman
    liberer_arbre(racine);
    liberer_table_huffman(&table);

    fclose(f_in);
    fclose(f_out);
}




void liberer_table_huffman(TableHuffman *table) {
    for (uint32_t i = 0; i < TAILLE_ALPHABET; i++) {
        if (table->code[i] != NULL) {
            free(table->code[i]);  // Libérer chaque code
            table->code[i] = NULL;
        }
    }
}




void liberer_arbre(Noeud *racine) {
    if (racine) {
        liberer_arbre(racine->g);
        liberer_arbre(racine->d);
        free(racine);
    }
}


void afficher_arbre(Noeud *racine, int niveau) {
    if (racine == NULL) return;

    // Décale l'affichage en fonction du niveau de profondeur
    for (int i = 0; i < niveau; i++) printf("  ");
    
    // Affiche les informations du nœud
    printf("Noeud: %c (Fréquence: %u)\n", racine->oct, racine->freq);

    // Affiche les enfants gauche et droit
    afficher_arbre(racine->g, niveau + 1);
    afficher_arbre(racine->d, niveau + 1);
}




