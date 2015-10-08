#!/bin/sh
# cern-lxdistcc-wrappers.sh: v1 KELEMEN Peter <lxdistcc-admins@cern.ch>
#
# 2012-10-17: Modified by Marco Clemencic
#
_self=${0##*/}

hostos() {
    arch=$(uname -i)
    case $(lsb_release -si) in
        ScientificCERNSLC)
            os=slc
            vers=$(lsb_release -sr | cut -d. -f1)
            ;;
        *)
            os=$(lsb_release -si | tr '[:upper:]' '[:lower:]')
            vers=$(lsb_release -sr)
    esac
    echo $arch-$os$vers
}

set_prefix() {
    _dirname=$1
    _version=$2

    _platform=${LCG_hostos:-$(hostos)}

    _external=${LCG_external_area:-/afs/cern.ch/sw/lcg/external}
    _releases=${LCG_release_area:-/afs/cern.ch/sw/lcg/releases}
    # take only the first entry in _releases
    _releases=${_releases%%:*}

    for _prefix in {${_releases},${_external}}/${_dirname}/${_version}/${_platform} ; do
	if [ -d "${_prefix}" ] ; then
	    break
	fi
    done
}

setup_gcc() {
        _version=$1

        set_prefix gcc ${_version}

        _bin="${_prefix}/bin"
        _lib="${_prefix}/lib64"
        LD_LIBRARY_PATH=$(echo $LD_LIBRARY_PATH | sed 's-[^:]*/gcc/[^:]*:\?--g')
        LD_LIBRARY_PATH="${_lib}${LD_LIBRARY_PATH:+:}${LD_LIBRARY_PATH}"
        PATH="${_bin}${PATH:+:}${PATH}"
        COMPILER_PATH="${_prefix}/lib/gcc/x86_64-unknown-linux-gnu/${_version}"

        GCC_TOOLCHAIN="${_prefix}"
        export LD_LIBRARY_PATH
        export PATH
        export COMPILER_PATH
        export GCC_TOOLCHAIN
}

setup_clang() {

        _clang_version=$1
        case ${_clang_version} in
          3.2) setup_gcc 4.6.3 ;;
          3.3|3.4) setup_gcc 4.8.1 ;;
          *) setup_gcc 4.9.3 ;;
        esac

        set_prefix llvm ${_clang_version}

        _bin="${_prefix}/bin"
        _lib="${_prefix}/lib64"
        LD_LIBRARY_PATH=$(echo $LD_LIBRARY_PATH | sed 's-[^:]*/llvm/[^:]*:\?--g')
        LD_LIBRARY_PATH="${_lib}${LD_LIBRARY_PATH:+:}${LD_LIBRARY_PATH}"
        PATH="${_bin}${PATH:+:}${PATH}"

        export LD_LIBRARY_PATH
        export PATH
        export COMPILER_PATH
}

case ${_self} in

        lcg-[cg]++-[0-9].[0-9].[0-9]|lcg-gcc-[0-9].[0-9].[0-9]|lcg-gfortran-[0-9].[0-9].[0-9])

                _version=${_self##*-}
                setup_gcc $_version
                _self=${_self%-*}
                _self=${_self#*-}
                ;;

        lcg-clang-*|lcg-clang++-*)

                _version=${_self##*-}
                setup_clang $_version
                _self=${_self%-*}
                _self=${_self#*-}
                _self="${_self} --gcc-toolchain=${GCC_TOOLCHAIN}"
                ;;

        *)
                echo "E: Unsupported compiler '${_self}', please contact <marco.clemencic@cern.ch>"
                exit 100
                ;;
esac
exec ${_bin}/${_self} "$@"

# End of file.
