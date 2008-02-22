#!/bin/bash
# $Id: cleanup_external.sh,v 1.1 2008-02-22 10:39:48 hmdegaud Exp $
# Author: Marco Clemencic

savedcmtroot=$CMTROOT

if [ -n "$LCG_release_area" ] ; then
    
    __my__packages=""

    if [ -n "$CMTPATH" ]; then
	__my_old_cmtpath__="$CMTPATH"
	__my__lcgcmt_path=`echo $CMTPATH | awk -F: '{for(i = 1; i<= NF; i++){print $i}}' | grep LCGCMT | head -1`
    fi

    # print help if no arguments
    __my_do_help=$(( $# == 0 ))
    # default position (CMT), 1 = prepend, 2 = append
    __my_do_ld_append=0

    # parse options
    while [ $# != 0 ] ; do
	case "$1" in
	    --lcg )
		__my__lcgcmt_path="$LCG_release_area/LCGCMT/LCGCMT_${2}"
		shift
		;;
	    --list )
		__my_do_list=1
		;;
	    --prepend )
		__my_do_ld_append=1
		;;
	    --append )
		__my_do_ld_append=2
		;;
	    -h | --help )
		__my_do_help=1
		;;
	    * )
		__my__packages="$__my__packages $1"
	esac
	shift
    done

    if [ $__my_do_help != 0 ] ; then
	echo "Usage: . cleanup_external.sh [options] package1 [package2 ...]"
	echo ""
	echo "Available options:"
	echo ""
	echo "  --lcg <version>  use the specified version of LCGCMT"
	echo "                   (default: the version in CMTPATH or"
	echo "                   the one used by latest Gaudi)"
	echo "  --list           show the available packages and exit"
        echo "  --append         put the new entries at the end of LD_LIBRARY_PATH"
        echo "  --prepend        put the new entries at the beginning of LD_LIBRARY_PATH"
	echo "  -h, --help       show this message and exit"
	echo ""
    else

    if [ ! -n "$__my__lcgcmt_path" ] ; then
	# get path to LCGCMT from latest Gaudi
	__my__lcgcmt_path=`echo | tcsh -f /afs/cern.ch/lhcb/scripts/ProjectEnv.csh Gaudi | sed 's/^.*searched for in //;s/ *$//' | awk -F: '{for(i = 1; i<= NF; i++){print $i}}' | grep LCGCMT | head -1`
    fi

    if [ -n "$__my_do_list" ] ; then
	echo "Available externals in $__my__lcgcmt_path:"
	\ls $__my__lcgcmt_path/LCG_Interfaces
	__my__packages=""
    fi

    export CMTPATH="$__my__lcgcmt_path"
    
    if [ -n "$__my__packages" ] ; then 

	if [ -n "$LD_LIBRARY_PATH" ] ; then
	    __my_old_ld_library_path="$LD_LIBRARY_PATH"
	fi

	__my__tempdir=`mktemp -d`

	touch $__my__tempdir/requirements
	for __my__external in $__my__packages ; do
	    echo "use $__my__external v* LCG_Interfaces" >> $__my__tempdir/requirements
	done
	
	( cd $__my__tempdir ; cmt config 2>&1 ) | grep "not found"
	if [ "$?" = "1" ]; then
	    if [ $__my_do_ld_append != 0 ]; then
		export LD_LIBRARY_PATH=""
	    fi
	    source $__my__tempdir/cleanup.sh
	    if [ -n "$__my_old_ld_library_path" ]; then
		case "$__my_do_ld_append" in
		    1 )
			export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${__my_old_ld_library_path}"
			;;
		    2 )
			export LD_LIBRARY_PATH="${__my_old_ld_library_path}:${LD_LIBRARY_PATH}"
			;;
		esac
	    fi
	else
	    echo "Problems when configuring: I do not change the environment"
	fi

	# remove temporary directory
	rm -rf $__my__tempdir
    fi
    
    fi # do help 

    # clean up environment
    unset __my__packages __my__lcgcmt_path __my__external __my__tempdir __my_do_list __my_do_help \
	__my_do_ld_append __my_old_ld_library_path
    export CMTPATH="$__my_old_cmtpath__"
    unset __my_old_cmtpath__

else
    echo "LCG_release_area not set: I cannot work like that."
fi

# set back CMT which has stupidly unset itself


export CMTROOT=$savedcmtroot
unset savedcmtroot

. $CMTROOT/mgr/setup.sh

.
