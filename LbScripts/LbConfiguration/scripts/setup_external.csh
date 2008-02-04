#!/bin/tcsh
# $Id: setup_external.csh,v 1.1.1.1 2008-02-04 14:37:08 hmdegaud Exp $
# Author: Marco Clemencic

if ( $?LCG_release_area ) then

    set __my__packages = ()

    if ( $?CMTPATH ) then
	set __my_old_cmtpath__ = "$CMTPATH"
	set __my__lcgcmt_path = `echo $CMTPATH | awk -F: '{for(i = 1; i<= NF; i++){print $i}}' | grep LCGCMT | head -1`
	if ( "$__my__lcgcmt_path" == "" ) unset __my__lcgcmt_path
    endif

    # print help if no arguments
    @ __my_do_help = ( $# == 0 )
    # default position (CMT), 1 = prepend, 2 = append
    set __my_do_ld_append = 0

    # parse options
    while ( $# != 0 )
	switch ( "$1" )
	    case "--lcg" :
		set __my__lcgcmt_path = "$LCG_release_area/LCGCMT/LCGCMT_${2}"
		shift
		breaksw
	    case "--list" :
		set __my_do_list = 1
		breaksw
	    case "--prepend" :
		set __my_do_ld_append = 1
		breaksw
	    case "--append" :
		set __my_do_ld_append = 2
		breaksw
	    case "-h" :
		set __my_do_help = 1
		breaksw
	    case "--help" :
		set __my_do_help = 1
		breaksw
	    default :
		set __my__packages = ( $__my__packages $1 )
	endsw
	shift
    end

    if ( $__my_do_help ) then
	echo "Usage: source setup_external.csh [options] package1 [package2 ...]"
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
	echo "warning: --append and --prepend may pollute your LD_LIBRARY_PATH because"
        echo "         they are not doing the replacement done usually by CMT."
	goto cleanup
    endif

    if ( ! $?__my__lcgcmt_path ) then
	# get path to LCGCMT from latest Gaudi
	set __my__lcgcmt_path = `echo | tcsh -f /afs/cern.ch/lhcb/scripts/ProjectEnv.csh Gaudi | sed 's/^.*searched for in //;s/ *$//' | awk -F: '{for(i = 1; i<= NF; i++){print $i}}' | grep LCGCMT | head -1`
    endif
    
    if ( $?__my_do_list ) then
	echo "Available externals in ${__my__lcgcmt_path}:"
	\ls $__my__lcgcmt_path/LCG_Interfaces
	set __my__packages = ()
    endif

    setenv CMTPATH "$__my__lcgcmt_path"

    # prepare the fake package, configure it and source setup.csh
    if ( $#__my__packages ) then

	if ( $?LD_LIBRARY_PATH ) then
	    set __my_old_ld_library_path = "$LD_LIBRARY_PATH"
	endif

	set __my__tempdir = `mktemp -d`

	touch $__my__tempdir/requirements
	foreach __my__external ( $__my__packages )
	    echo "use $__my__external v* LCG_Interfaces" >> $__my__tempdir/requirements
	end

	( cd $__my__tempdir ; cmt config ) |& grep "not found"
	if ( $? == 1 ) then
	    if ( $__my_do_ld_append ) then
		setenv LD_LIBRARY_PATH ""
	    endif
	    source $__my__tempdir/setup.csh
	    if ( $?__my_old_ld_library_path ) then
		switch ( $__my_do_ld_append )
		    case 1 :
			setenv LD_LIBRARY_PATH "${LD_LIBRARY_PATH}:${__my_old_ld_library_path}"
			breaksw
		    case 2 :
			setenv LD_LIBRARY_PATH "${__my_old_ld_library_path}:${LD_LIBRARY_PATH}"
			breaksw
		endsw
	    endif
	else
	    echo "Problems when configuring: I do not change the environment"
	endif

	# remove temporary directory
	rm -rf $__my__tempdir
    endif

    # clean up environment
    cleanup:
    unset __my__packages __my__lcgcmt_path __my__external __my__tempdir __my_do_list __my_do_help \
	__my_do_ld_append __my_old_ld_library_path
    if ( $?__my_old_cmtpath__ ) then
	setenv CMTPATH "$__my_old_cmtpath__"
	unset __my_old_cmtpath__
    else
	unsetenv CMTPATH
    endif

else
    echo "LCG_release_area not set: I cannot work like that."
endif
