#!/bin/sh
# Adapted from cern-lxdistcc-wrappers.sh by Marco Clemencic
#
_self=${0##*/}

_app=${_self#*-}
_app=${_app%-*}

setup_icc() {
    _version=${_self##*-}
    _soft_dir=/afs/cern.ch/sw/IntelSoftware/linux/x86_64

    . /afs/cern.ch/sw/IntelSoftware/linux/setup.sh > /dev/null
    case ${_version} in
	11)
	    . ${_soft_dir}/Compiler/11.1/072/bin/intel64/iccvars_intel64.sh
	    ;;
	13)
	    . ${_soft_dir}/xe2013/composer_xe_2013/pkg_bin/compilervars_arch.sh intel64
	    ;;
    esac
}

case ${_self} in
    
    lcg-i*-11 | lcg-i*-13)
		
        setup_icc
        ;;
    
    *)
	echo "E: Unsupported compiler '${_self}', please contact <marco.clemencic@cern.ch>"
	exit 100
	;;
esac

exec ${_app} "$@"

# End of file.
