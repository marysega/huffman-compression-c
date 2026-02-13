#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "huffman.h"

// Fonction pour lire la table de Huffman depuis le fichier compressé
void lire_table_huffman(FILE *f, TableHuffman *table) {
    uint8_t nb_symboles;
    fread(&nb_symboles, sizeof(uint8_t), 1, f);

    for (uint32_t i = 0; i < nb_symboles; i++) {
        uint8_t oct, longueur;
        fread(&oct, sizeof(uint8_t), 1, f);
        fread(&longueur, sizeof(uint8_t), 1, f);

        char *code = malloc((longueur + 1) * sizeof(char));
        fread(code, sizeof(char), longueur, f);
        code[longueur] = '\0'; // Terminer la chaîne

        table->code[oct] = code;
    }
}

// Fonction pour reconstruire l'arbre de Huffman à partir de la table
Noeud *recreer_arbre(TableHuffman *table) {
    Noeud *racine = creer_noeud(0, 0, NULL, NULL);
    if (!racine) {
    fprintf(stderr, "Erreur : échec de la création d'un noeud\n");
    exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < TAILLE_ALPHABET; i++) {
        if (table->code[i] != NULL) {
            Noeud *courant = racine;
            char *code = table->code[i]; 

             

            for (uint32_t j = 0; code[j] != '\0'; j++) {
                if (code[j] == '0') {
                    if (courant->g == NULL) {
                        courant->g = creer_noeud(0, 0, NULL, NULL);
                        
                    }
                    courant = courant->g;
                } else if (code[j] == '1') {
                    if (courant->d == NULL) {
                        courant->d = creer_noeud(0, 0, NULL, NULL);
                        
                    }
                    courant = courant->d;
                }
            }

            courant->oct = i; // Stocker l'octet à la feuille
           
        }
    }
    

    return racine;
}

// Fonction pour décompresser un fichier
void decompresser_fichier(const char *f_compresse, const char *f_dest) {
    
    
    FILE *f_in = fopen(f_compresse, "rb");
    FILE *f_out = fopen(f_dest, "wb");

    if (!f_in || !f_out) {
        perror("Erreur d'ouverture de fichier");
        exit(EXIT_FAILURE);
    }

    // Lire la table de Huffman
    
    TableHuffman table;
    for (uint32_t i = 0; i < TAILLE_ALPHABET; i++) {
        table.code[i] = NULL;
        
    }
   
    lire_table_huffman(f_in, &table);

    for (uint32_t i = 0; i < TAILLE_ALPHABET; i++) {
        if (table.code[i] != NULL) {
         printf("Symbole: %c, Code: %s\n", (char)i, table.code[i]); //pour avoir une visibilité
         }
    }
    // Reconstruire l'arbre de Huffman
    Noeud *racine = recreer_arbre(&table);
    if (racine == NULL) {
    fprintf(stderr, "Erreur : l'arbre de Huffman n'a pas été reconstruit.\n");
    exit(EXIT_FAILURE);
    }


    // Lire les données compressées et les décompresser
    Noeud *courant = racine;
    
    uint8_t oct, /*bits_restants = 0,*/ bit;

    while (fread(&oct, sizeof(uint8_t), 1, f_in) == 1) {
        
        for (int i = 7; i >= 0; i--) {
            bit = (oct >> i) & 1;
            //printf("Bit: %d, Position dans l'arbre: %p\n", bit, courant);
            if (bit == 0) {
                courant = courant->g;
            } else {
                courant = courant->d;
            }
            if (courant == NULL) {

                fprintf(stderr, ":( le noeuds actuel est NULL .Bit lu: %d, Octet courant: %02x\n", bit, oct);

                exit(EXIT_FAILURE);

            }

            if (courant->g == NULL && courant->d == NULL) {
                fwrite(&courant->oct, sizeof(uint8_t), 1, f_out);
                courant = racine;
            }
        }
    }

    // Libérer la mémoire
    for (uint32_t i = 0; i < TAILLE_ALPHABET; i++) {
        if (table.code[i] != NULL) {
            free(table.code[i]);
        }
    }
    liberer_arbre(racine);

    fclose(f_in);
    fclose(f_out);
}

void decompresser_bmp(const char *f_compresse, const char *f_dest) {
    FILE *f_in = fopen(f_compresse, "rb");
    FILE *f_out = fopen(f_dest, "wb");

    if (!f_in || !f_out) {
        perror("Erreur d'ouverture de fichier");
        exit(EXIT_FAILURE);
    }

    // Lire l'en-tête BMP (54 octets)
    uint8_t entete[54];
    if (fread(entete, sizeof(uint8_t), 54, f_in) != 54) {
        fprintf(stderr, "Erreur : lecture de l'en-tête BMP échouée\n");
        fclose(f_in);
        fclose(f_out);
        exit(EXIT_FAILURE);
    }

    // Écrire l'en-tête dans le fichier décompressé
    fwrite(entete, sizeof(uint8_t), 54, f_out);

    // Lire la table de Huffman
    TableHuffman table;
    for (uint32_t i = 0; i < TAILLE_ALPHABET; i++) {
        table.code[i] = NULL;
    }
    lire_table_huffman(f_in, &table);

    // Reconstruire l'arbre de Huffman
    Noeud *racine = recreer_arbre(&table);
    if (!racine) {
        fprintf(stderr, "Erreur : échec de la reconstruction de l'arbre de Huffman\n");
        fclose(f_in);
        fclose(f_out);
        exit(EXIT_FAILURE);
    }

    // Décompresser les données
    Noeud *courant = racine;
    uint8_t oct, bit;

    while (fread(&oct, sizeof(uint8_t), 1, f_in) == 1) {
        for (int i = 7; i >= 0; i--) {
            bit = (oct >> i) & 1;
            courant = (bit == 0) ? courant->g : courant->d;

            if (!courant) {
                fprintf(stderr, "Erreur : séquence de bits invalide\n");
                fclose(f_in);
                fclose(f_out);
                exit(EXIT_FAILURE);
            }

            if (!courant->g && !courant->d) {
                fwrite(&courant->oct, sizeof(uint8_t), 1, f_out);
                courant = racine;
            }
        }
    }

    // Libérer la mémoire
    for (uint32_t i = 0; i < TAILLE_ALPHABET; i++) {
        if (table.code[i]) {
            free(table.code[i]);
        }
    }
    liberer_arbre(racine);

    fclose(f_in);
    fclose(f_out);
}

// Fonction pour décompresser un fichier JPG
void decompresser_jpg(const char *f_compresse, const char *f_dest) {
    // La décompression d'un JPG est similaire à celle d'un fichier texte ou BMP
    decompresser_bmp(f_compresse, f_dest);
}


