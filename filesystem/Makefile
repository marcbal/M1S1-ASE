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
all: dmps fmrt dvol mkvol rmvol mknfs dfs valid_super

# lister en argument de 'rm' les noms de fichiers binaires exécutables
realclean: clean
	rm dmps fmrt dvol mkvol rmvol mknfs dfs valid_super || true




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

dmps: drive.o dump.o cmd_dmps.o
	${CC} ${CFLAGS} -o $@ $^ ${LDLIBS}

fmrt: drive.o cmd_fmrt.o
	${CC} ${CFLAGS} -o $@ $^ ${LDLIBS}

dvol: drive.o vol.o cmd_dvol.o
	${CC} ${CFLAGS} -o $@ $^ ${LDLIBS}

mkvol: drive.o vol.o cmd_mkvol.o
	${CC} ${CFLAGS} -o $@ $^ ${LDLIBS}

rmvol: drive.o vol.o cmd_rmvol.o
	${CC} ${CFLAGS} -o $@ $^ ${LDLIBS}

mknfs: drive.o vol.o super.o cmd_mknfs.o
	${CC} ${CFLAGS} -o $@ $^ ${LDLIBS}

dfs: drive.o vol.o super.o cmd_dfs.o
	${CC} ${CFLAGS} -o $@ $^ ${LDLIBS}

valid_super: drive.o vol.o super.o valid_super.o
	${CC} ${CFLAGS} -o $@ $^ ${LDLIBS}

# --------------------------------------
