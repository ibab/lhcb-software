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

setup_gcc() {
        _version=$1
        _platform=${LCG_hostos:-$(hostos)}
        _external=${LCG_external_area:-/afs/cern.ch/sw/lcg/external}

        _prefix="${_external}/gcc/${_version}"
        _bin="${_prefix}/${_platform}/bin"
        _lib="${_prefix}/${_platform}/lib64"
        LD_LIBRARY_PATH=$(echo $LD_LIBRARY_PATH | sed 's-[^:]*/gcc/[^:]*:\?--g')
        LD_LIBRARY_PATH="${_lib}${LD_LIBRARY_PATH:+:}${LD_LIBRARY_PATH}"
        PATH="${_bin}${PATH:+:}${PATH}"
        COMPILER_PATH="${_prefix}/${_platform}/lib/gcc/x86_64-unknown-linux-gnu/${_version}"

        export LD_LIBRARY_PATH
        export PATH
        export COMPILER_PATH
}

setup_clang() {

        _version=$1
        case ${_version} in
          3.3) setup_gcc 4.8.1 ;;
          *)   setup_gcc 4.6.3 ;;
        esac

        _platform=${LCG_hostos:-$(hostos)}
        _external=${LCG_external_area:-/afs/cern.ch/sw/lcg/external}

        _prefix="${_external}/llvm/${_version}"
        _bin="${_prefix}/${_platform}/bin"
        _lib="${_prefix}/${_platform}/lib64"
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
                ;;

        lcg-clang-*|lcg-clang++-*)

                _version=${_self##*-}
                setup_clang $_version
                ;;

        *)
                echo "E: Unsupported compiler '${_self}', please contact <marco.clemencic@cern.ch>"
                exit 100
                ;;
esac

exec ${_bin}/${_self} "$@"

# End of file.
