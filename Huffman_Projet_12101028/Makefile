# Variables
CC = gcc
CFLAGS = -Wall
SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin

# Fichiers sources
SRC_FILES = $(SRC_DIR)/main.c $(SRC_DIR)/compresse.c $(SRC_DIR)/decompresse.c
# Nom de l'exécutable
EXEC = $(BIN_DIR)/huffman

# Règle par défaut
all: $(EXEC)

# Règle pour créer l'exécutable
$(EXEC): $(SRC_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $(SRC_FILES) -o $(EXEC)

# Règle pour nettoyer les fichiers générés
clean:
	rm -rf $(BIN_DIR)

# Règles pour compresser des fichiers spécifiques
c-chaton: $(EXEC)
	./$(EXEC) c test_files/chaton.jpg test_files/chaton.compresse

c-caillou: $(EXEC)
	./$(EXEC) c test_files/Caillou.bmp test_files/Caillou.compresse

c-vingtmille: $(EXEC)
	./$(EXEC) c test_files/vingtmille.txt test_files/vingtmille.compresse

# Règles pour décompresser des fichiers spécifiques
d-chaton: $(EXEC)
	./$(EXEC) d test_files/chaton.compresse test_files/chaton_decompresse.jpg

d-caillou: $(EXEC)
	./$(EXEC) d test_files/Caillou.compresse test_files/Caillou_decompresse.bmp

d-vingtmille: $(EXEC)
	./$(EXEC) d test_files/vingtmille.compresse test_files/vingtmille_decompresse.txt

.PHONY: all clean c-chaton c-caillou c-vingtmille d-chaton d-caillou d-vingtmille

