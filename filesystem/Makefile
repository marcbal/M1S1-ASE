# numéro de TP
TP=4
# nom du coéquipier 1
NOM1=Badiane
# nom du coéquipier 2
NOM2=Baloup

# librairies
LIBDIR = lib

LIBS = hardware



# =========== NE PAS TOUCHER ===========
CC      = gcc
CFLAGS  = -g -Wall -m32
# CFLAGS += -D_XOPEN_SOURCE=500
LDLIBS  = -L${LIBDIR} $(addprefix -l,$(LIBS))
# ======================================



# commande 'make' sans argument
# lister les exécutables à produire
all: filesystem

# lister en argument de 'rm' les noms de fichiers binaires exécutables
realclean: clean
	rm filesystem || true




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
filesystem: drive.o dump.o main.o
	${CC} ${CFLAGS} -o $@ $^ ${LDLIBS}

# --------------------------------------
