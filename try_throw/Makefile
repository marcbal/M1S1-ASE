# numéro de TP
TP=1
# nom du coéquipier 1
NOM1=Badiane
# nom du coéquipier 2
NOM2=Baloup




# =========== NE PAS TOUCHER ===========
CC      = gcc
CFLAGS  = -g -Wall -m32
# CFLAGS += -D_XOPEN_SOURCE=500
LDFLAGS  = -g -m32
# ======================================



# commande 'make' sans argument
# lister les exécutables à produire
all: try

# lister en argument de 'rm' les noms de fichiers binaires exécutables
realclean: clean
	rm try || true




# =========== NE PAS TOUCHER ===========
# Compilation séparée
%.o: %.c %.h
	${CC} ${CFLAGS} -c $<
clean:
	rm *.o || true
archive: realclean
	zip -r ../TP${TP}_${NOM1}_${NOM2}.zip *
# Liste des pseudo-cibles utilisés dans ce fichier,
# qui ne sont pas des noms de fichier à produire
.PHONY: all clean realclean archive
# ======================================




# ----- les exécutables à produire -----
try: try.o try_mul.o
	${CC} ${LDFLAGS} -o $@ $^

# --------------------------------------
