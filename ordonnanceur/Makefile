# numéro de TP
TP=3
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
LDFLAGS  = -g -m32 -L${LIBDIR} $(addprefix -l,$(LIBS))
# ======================================



# commande 'make' sans argument
# lister les exécutables à produire
all: context

# lister en argument de 'rm' les noms de fichiers binaires exécutables
realclean: clean
	rm context || true




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
context: semaphore.o context.o main.o
	${CC} ${LDFLAGS} -o $@ $^

# --------------------------------------
