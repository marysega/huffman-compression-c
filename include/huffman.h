#define TAILLE_ALPHABET 256  // 256 valeurs possibles pour un uint8_t
#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <stdint.h> 
#include <stdio.h>   
#include <stdlib.h> 



// Structure d'un noeud de Huffman
typedef struct Noeud 
{
    uint8_t oct;      // Valeur du caractere (0-255)
    uint32_t freq; // Frequence du caractere
    struct Noeud *g ;
    struct Noeud *d;
} Noeud;

typedef struct Maillon {
    Noeud *noeud;              
    struct Maillon *suivant;   
} Maillon;


typedef struct {
    Maillon *tete;  //premier maillon de la liste
    uint32_t taille; 
} File;

typedef struct {
    char *code[TAILLE_ALPHABET];  // Pointeur vers le code de chaque caractère
} TableHuffman;



void lire_f(const char *nom_fichier, uint32_t freq[TAILLE_ALPHABET]);
Noeud *creer_noeud(uint8_t oct, uint32_t frequence, Noeud *gauche, Noeud *droit);
void ajt_file(File *f, Noeud *n);
Noeud *extraire_tete(File *f) ;
Noeud *creer_arbre(uint32_t freq[TAILLE_ALPHABET]) ;
void generer_codes(Noeud *racine, char chemin[], uint32_t longueur, TableHuffman *table);
void ecrire_fcompresse(const char *f_src, const char *f_dst, TableHuffman *table);
void liberer_arbre(Noeud *racine);
void liberer_file(File *f) ;
void compresser_bmp(const char *f_src, const char *f_dest) ;
void compresser_jpg(const char *f_src, const char *f_dest) ;
void liberer_table_huffman(TableHuffman *table);


//Fonction de decompresse.c


void decompresser(const char *fichier_compresse, const char *f_dst);
void decompresser_jpg(const char *fichier_compresse, const char *f_dst) ;
void decompresser_fichier2(const char *fichier_compresse, const char *f_dst, Noeud *racine);
void decompresser_fichier(const char *f_compresse, const char *f_dest);
Noeud *recreer_arbre(TableHuffman *table);
void lire_table_huffman(FILE *f_in, TableHuffman *table) ;
void decompresser_bmp(const char *f_compresse, const char *f_dest);
void initialiser_table_huffman(TableHuffman *table);
void afficher_table_huffman(const TableHuffman *table);
//void decompresser_fichier_stream(FILE *f_in, FILE *f_out);

 #endif
