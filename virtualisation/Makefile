# nom d'archive
ARCHIVE=TP5_Baloup_Badiane.zip

# librairies
LIBDIR = lib

LIBS = hardware

# fichiers binaires à produire
# les prérequis (.o) des binaires sont à définir plus bas dans le fichier
BINARIES = mmu

# commande de compilation
CC      = gcc

# options de compilation (.o et linkage)
CFLAGS  = -Wall -Werror -m32







# =========== NE PAS TOUCHER ===========
# commande 'make' sans argument
all: ${BINARIES}

LDLIBS  = -L${LIBDIR} $(addprefix -l,$(LIBS))

# Compilation séparée
%.o: %.c %.h
	${CC} ${CFLAGS} -c $<

clean:
	rm *.o ${BINARIES} || true

# cible utilisée pour le rendu sous forme d'archive
archive: clean
	zip -r ../${ARCHIVE} *

.PHONY: all clean realclean archive

${BINARIES}:
	${CC} ${CFLAGS} -o $@ $^ ${LDLIBS}
# ======================================




# ----- les exécutables à produire -----
mmu: mmu.o
