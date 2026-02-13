#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

void affichage() {
    printf("Ecrit sous cette forme :\n");
    printf("  Compression : ./huffman c <fichier_srsc> <fichier.compresser>\n");
    printf("  Décompression : ./huffman d <fichier.compresser> <fichier_sortie>\n");
}

int main(int argc, char *argv[]) {
    // Vérification des arguments
    if (argc != 4) {
        affichage();
        return EXIT_FAILURE;
    }

    char m = argv[1][0];  // 'c' pour compresser, 'd' pour décompresser
    const char *f_src = argv[2];
    const char *f_dst = argv[3];

    // Mode compression
    if (m == 'c') {
        if (strstr(f_src, ".bmp") != NULL) {
            printf("Compression d'un fichier BMP !!\n");
            compresser_bmp(f_src, f_dst);
        } 
        else if (strstr(f_src, ".jpg") != NULL || strstr(f_src, ".jpeg") != NULL) {
            printf("Compression d'un fichier JPG. !!\n");
            compresser_jpg(f_src, f_dst);
        } 
        else {
            printf("Compression d'un fichier txt !! \n");

            // Analyser le fichier pour obtenir les fréquences
            uint32_t freq[TAILLE_ALPHABET] = {0};
            lire_f(f_src, freq);

            // Construire l'arbre de Huffman
            Noeud *racine = creer_arbre(freq);

            if (!racine) {
                fprintf(stderr, "Erreur dans création de l'arbre Huffman ???\n");
                return EXIT_FAILURE;
            }

            // Générer la table de Huffman
            TableHuffman table;
            memset(&table, 0, sizeof(TableHuffman));
            char chemin[256];  // Assez grand au cas ou
            generer_codes(racine, chemin, 0, &table);

            // Écrire le fichier compressé
            ecrire_fcompresse(f_src, f_dst, &table);

            // Nettoyage mémoire
            liberer_arbre(racine);
            liberer_table_huffman(&table);
        }

        printf("Congrats ! Compression terminée !!! Fichier créée dans : %s\n", f_dst);
    
    } 
    // Mode décompression
    else if (m == 'd') {
        printf("Décompression du fichier :)\n");

        // Vérification que le fichier source est bien un fichier compressé
        if (strstr(f_src, ".compresser") == NULL) {
            fprintf(stderr, "Fait attention prend bien le fichier compresser :(");
        }
        
        //Décompression selon le type de fichier
        if (strstr(f_src, ".bmp") != NULL) {
            decompresser_bmp(f_src, f_dst);
        } 
        else if (strstr(f_src, ".jpg") != NULL || strstr(f_src, ".jpeg") != NULL) {
            decompresser_jpg(f_src, f_dst);
        } 
        else {
            decompresser_fichier(f_src, f_dst);
        }

        printf("Congratulation ;) tu peux retrouver ton ficher decompressée dans %s\n", f_dst);
    } 
    // Mode inconnu
    else {
        fprintf(stderr, "Faute de frappe ? : Mode '%c'?? \n", m);
        affichage();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

