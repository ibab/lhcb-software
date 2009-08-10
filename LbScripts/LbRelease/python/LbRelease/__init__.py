## Write the generic Makefile for CMT projects.
#  @param filename: the name of the destination file
#  @param overwrite: flag to decide if an already present file has to be kept or not (default is False)
def createProjectMakefile(dest, overwrite = False):
    import os
    if overwrite or not os.path.exists(dest):
        f = open(dest, "w")
        f.write("include ${LBRELEASEROOT}/data/Makefile\n")
        f.close()
