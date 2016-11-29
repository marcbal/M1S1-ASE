# nom d'archive
ARCHIVE=TP4_Baloup_Badiane.zip

# librairies
LIBDIR = lib

LIBS = hardware

# fichiers binaires à produire
# les prérequis (.o) des binaires sont à définir plus bas dans le fichier
BINARIES = dmps fmrt dvol mkvol rmvol mknfs dfs valid_super valid_inode \
		if_nfile if_pfile if_dfile

# commande de compilation
CC      = gcc

# options de compilation (.o et linkage)
CFLAGS  = -Wall -Wextra -Werror -m32







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
dmps:        drive.o dump.o             cmd_dmps.o
fmrt:        drive.o                    cmd_fmrt.o
dvol:        drive.o vol.o              cmd_dvol.o
mkvol:       drive.o vol.o              cmd_mkvol.o
rmvol:       drive.o vol.o              cmd_rmvol.o
mknfs:       drive.o vol.o fs.o         cmd_mknfs.o
dfs:         drive.o vol.o fs.o         cmd_dfs.o
valid_super: drive.o vol.o fs.o         valid_super.o
valid_inode: drive.o vol.o fs.o         valid_inode.o
if_pfile:    drive.o vol.o fs.o ifile.o if_pfile.o
if_nfile:    drive.o vol.o fs.o ifile.o if_nfile.o
if_dfile:    drive.o vol.o fs.o ifile.o if_dfile.o
