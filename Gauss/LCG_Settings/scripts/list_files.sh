#!/bin/sh
# ============================================================================= #
#                                                                               #
#    list_files.sh [options] [dirs]                                             #
#                                                                               #
#    Unix script to list files in the specified directories                     #
#                                                                               #
#    Options:                                                                   #
#                                                                               #
#      -e|--executable       Executable files only                              #
#      -t|--type <type>      Specify the file type (e.g. so) (default = all)    #
#                            Multiple such types may be specified               #
#                                                                               #
# ============================================================================= #

#+
# Main Program
#-

# set -x

# --- Default parameters

dirList=
typeList=
fileList=
execOnly=

# --- Input parameters

while [ $# -ne 0 ]; do
    case $1 in
        -s=*)        dirList="$dirList `echo $1 | sed 's/-s=//'`";;
        -t | --type) if [ -z "`echo $typeList | grep $2`" ]; then 
                         typeList="$typeList $2" 
                     fi; shift;;
		-e | --executable) execOnly=true;;
        *)           dirList="$dirList $1";;
    esac
    shift
done

# --- Loop over directory list

for dir in $dirList; do
    if [ -d $dir ]; then
        files=`ls $dir`

# --- Loop over files in directory

        for file in $files; do
		    fileMatch=$file
		
# --- Check for executable files only if appropriate

			if [ ! -z $execOnly ]; then
				fileMatch=`find ${dir} -name ${file} -type f -perm +u+x,g+x,a+x`
			fi
			if [ ! -z ${fileMatch} ]; then
				
# --- Check for match with file types (extensions) if appropriate

				if [ ! -z "${typeList}" ]; then
					fileMatch=
	        		for type in $typeList; do
                		fileMatch=`echo $file | grep [.]${type}`
                		if [ ! -z $fileMatch ]; then
							break;
						fi
					done
				fi

# --- Append matching file to the output file list if appropriate

				if [ ! -z "${fileMatch}" ]; then
					fileList="${fileList} $dir/$file"
				fi
            fi
        done
    fi
done

# --- Report matching file list

echo $fileList
