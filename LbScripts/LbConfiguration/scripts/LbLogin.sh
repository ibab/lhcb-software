
# set -x

# get the location of this very script


a=`history 1 | awk '{print $3}'`
#b=`basename $a`
l=`readlink $a`
if [ "$l" = "" ]; then
	l=$a
	scriptsdir=`dirname $a`
else
	pushd `dirname $a` &> /dev/null
	pushd `dirname $l` &> /dev/null
	scriptsdir=`pwd`
	popd &> /dev/null
	popd &> /dev/null
fi

if [ "$scriptsdir" = "." ]; then
  scriptsdir=`pwd`
fi

echo $scriptsdir

#################################################################
# parsing command line arguments

debug=0
mysiteroot=0
cmtconfig=0
userarea=0
cmtvers=0

TEMP=`getopt -o  d,m:,c:,u:,v: --long debug,mysiteroot:,cmtconfig:,userarea:,cmtvers: -- "$@"`

if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"

while true ; do
	case "$1" in
	-d|--debug) debug=1 ; shift ;;
	-m|--mysiteroot) mysiteroot=$2 ; shift 2 ;;
	-c|--cmtconfig) cmtconfig=$2 ; shift 2 ;;
	-u|--userarea)  userarea=$2 ; shift 2 ;; 
	-v|--cmtvers)  cmtvers=$2 ; shift 2 ;; 
	--) shift ; break ;;
	*) echo "Internal error!" ; exit 1 ;;
	esac
done
echo "Remaining arguments:"
for arg do echo '--> '"\`$arg'" ; done

#################################################################

if [ ! -e ${HOME}/.rhosts ]; then
	echo "Creating a ${HOME}/.rhosts to use CMT"
	echo " "
	echo "Joel.Closier@cern.ch"
	echo "+ ${USER}" > ${HOME}/.rhosts
fi

# remove .cmtrc file if it exists
if [ -f ${HOME}/.cmtrc ] ; then
	/bin/rm  ${HOME}/.cmtrc
fi

# clear PATH and LD_LIBRARY_PATH
if [ "x$SAVEPATH" != "x" ]; then
	PATH="$SAVEPATH"; export PATH
fi
unset LD_LIBRARY_PATH
unset COMPILER_PATH
unset GCC_EXEC_PREFIX

###################################################################################
# CVS setup
# Gaudi:
export GAUDIKSERVER=":kserver:isscvs.cern.ch:/local/reps/Gaudi"
# LHCb
export LHCBKSERVER=":kserver:isscvs.cern.ch:/local/reps/lhcb"

###################################################################################
# MYSITEROOT massaging

if [ ! "$mysiteroot" = "0" ] ; then
	export MYSITEROOT="$mysiteroot"
	export SITEROOT="$MYSITEROOT"
	export CMTSITE="LOCAL"
else
	if [ ! -z $MYSITEROOT  ] ; then
		export SITEROOT="$MYSITEROOT"
		mysiteroot="$MYSITEROOT"
		export CMTSITE="LOCAL"		
	else
		if [ -d /afs/cern.ch  ] ; then
			export CMTSITE="CERN"
			export AFSROOT="/afs"
			export SITEROOT="/afs/cern.ch"
		else
			echo " "\$MYSITEROOT" is not defined"
			echo " we suggest you install all software under" \$MYSITEROOT
			echo " then LHCb software will be installed under" \$MYSITEROOT"/lhcb"
			echo "      LCG software will be installed under" \$MYSITEROOT"/lcg/external"
			echo "      CMT and OpenScientist will be installed under" \$MYSITEROOT"/contrib"
			echo " as an example "
			echo " MYSITEROOT=/home/ranjard/Install"
			echo ""
			return
		fi
	fi
fi

###################################################################################
# User release area massaging

if [ ! "$userarea" = "0" ] ; then
	export User_release_area=$userarea
else
	if [ -z $User_release_area ] ; then
		export User_release_area=$HOME/cmtuser
	fi
fi

# User_release_area
if [ ! -d $User_release_area ] ; then
	mkdir -p $User_release_area
	echo " --- a new ${User_release_area:t} directory has been created in your HOME directory"
	if [ "$CMTSITE" = "CERN" ] ; then
		fs setacl $User_release_area system:anyuser rl
		echo " --- with public access (readonly)"
		echo " --- use mkprivat to remove public access to the current directory"
		echo " --- use mkpublic to give public access to the current directory"
	fi
fi

if [ -d ${User_release_area}/cmt ] ; then
	rm -fr ${User_release_area}/cmt
fi

###################################################################################
# setup contrib area

if [ "$CMTSITE" = "CERN" ] ; then 
	export CONTRIBDIR=$SITEROOT/sw/contrib
else
	export CONTRIBDIR=$MYSITEROOT/contrib 
fi


###################################################################################
# configure CMT

if [ "$cmtvers" = "0" ] ; then
	cmtvers="v1r20p20070208"
	if [ `uname` = "Darwin" ] ; then
		cmtvers="v1r20p20070524"
	fi
fi

export CMT_DIR=$CONTRIBDIR

. $CMT_DIR/CMT/$cmtvers/mgr/setup.sh


###################################################################################
# shared area massaging
# On AFS the shared area is /afs/cern.ch/project/gd/apps/lhcb/lib

###################################################################################
echo "debug $debug"
echo "mysiteroot $mysiteroot"
echo "cmtconfig $cmtconfig"
echo "userarea $userarea"
echo "cmtvers $cmtvers"

echo "Returning ..."
return 
echo "Returned"

#set -x

# set default values for CMT and gcc versions
comp=`gcc --version | grep gcc | awk '{print $3}' | tr -d "."`

if [ "$comp" -ge "340" ] ; then
	gcc=`gcc --version | grep gcc | awk '{print $3}' | awk -F. '{for(i=1; i<=2; i++){print $i}}'`
	comp=`echo $gcc | tr -d " "`
fi

compdef=gcc$comp
newcomp=""
debug=""

# get CMT and/or gcc versions from arguments if any  

for args 
do
	if [ "$args" = "HEAD" ]; then
		cmtvers="HEAD"
	else
		if [ "$args" = "debug" ] ; then
			debug=$args
		else
			v=`echo $args | grep 'v1r'`
			if [ "$v" != "" ] ; then
				cmtvers=$v
			else
				newcomp=$args
			fi
		fi
	fi
done



# get .rootauthrc file if not yet there
if [ ! -e $HOME/.rootauthrc ] ; then
	cp /afs/cern.ch/lhcb/scripts/.rootauthrc $HOME/.
fi

# Gaudi release area
GAUDISOFT="$SITEROOT/sw/Gaudi/releases"; export GAUDISOFT
GAUDIDEV="$SITEROOT/sw/Gaudi/dev"; export GAUDIDEV
Gaudi_release_area="$GAUDISOFT"; export Gaudi_release_area

# LCG release area
LCG_release_area="/afs/cern.ch/sw/lcg/app/releases"; export LCG_release_area

# DIM release area
DIM_release_area="/afs/cern.ch/lhcb/online/control"; export DIM_release_area

# LHCb release area
LHCBRELEASES="$SITEROOT/lhcb/software/releases"; export LHCBRELEASES
LHCBDEV="$SITEROOT/lhcb/software/DEV"; export LHCBDEV
LHCBDOC="$LHCBRELEASES/DOC"; export LHCBDOC

# LHCb project path
LHCBPROJECTPATH="$LHCBRELEASES:$GAUDISOFT:$LCG_release_area"; export LHCBPROJECTPATH

#================================================================================================
# set CMTPATH or CMTPROJECTPATH
# if CMTPROJECTPATH is set beforehand keep it ( test)
if [ "${CMTPROJECTPATH}" != "" ]; then
	unset CMTPATH
else
	cvers=`echo $cmtvers | grep v1r20`
	if  [ "W${cvers}" = "W" ]; then
		CMTPATH="${User_release_area}"; export CMTPATH  
		unset CMTPROJECTPATH
	else
		unset CMTPATH
		CMTPROJECTPATH="${User_release_area}:${LHCb_release_area}:${Gaudi_release_area}:${LCG_release_area}"; export CMTPROJECTPATH
	fi
fi

#==============================================================================================
# new emacs environment
EMACSDIR="$LHCBRELEASES/TOOLS/Tools/Emacs/pro"; export EMACSDIR

# LHCb styles configuration
LHCBSTYLE="$LHCBRELEASES/TOOLS/Tools/Styles/pro"; export LHCBSTYLE



# deal with different linux distributions ======================
if [ -e /etc/redhat-release ] ; then
	distrib=`cat /etc/redhat-release | awk '{print $1}'`
	rhv=`cat /etc/redhat-release | tr -d '[a-z][A-Z]'`
	if [ "$distrib" = "Scientific" ] ; then
		nativehw=`uname -i`
		hw="$nativehw"
		if [ "$hw" = "i386" ] ; then
			hw="ia32"
		else
			if [ "$hw" = "x86_64" ] ; then
				hw="amd64"
			fi
		fi
		rhv=`echo ${rhv} | awk -F "." '{print $1}'`
		rh="slc"${rhv}"_"${hw}
	else
		rhv=`echo ${rhv} | tr -d "."` 
		rh="rh$rhv"
	fi
fi

# deal with OS type ===========================================
if [ `uname -s` = "Darwin" ] ; then
	rh=`sw_vers | grep ProductVersion | awk '{print $2}' | awk -F . '{print $1 $2}'`
	rh="osx$rh"
	comp=`gcc --version | grep gcc | awk '{print $3}' | tr -d "."`
	comp="gcc$comp"
	if [ `uname -p` = 'powerpc' ]; then
		hw='ppc'
	else
		hw='ia32'
	fi
	rh="${rh}_${hw}"
	CMTOPT=${rh}_${comp} ; export CMTOPT
elif [ "$OSTYPE" = "linux-gnu" -o "$OSTYPE" = "linux" ] ; then
	# get the compiler from the arguments
	if [ "$newcomp" = "" ] ; then
		comp=$compdef
	else
		comp=$newcomp
	fi

  	#========if gcc323 is requested on SLC4 compile on SLC4 pretending it is SLC3
	if [ "$comp" = "gcc323" -a "$rhv" != "3" ] ; then
		if [ "$nativehw" = "x86_64" -o "$nativehw" = "i386" ] ; then
    		COMPILER_PATH="/afs/cern.ch/lhcb/externallib/SLC3COMPAT/slc3_ia32_gcc323" ; export COMPILER_PATH
    		CMTOPT="slc3_ia32_${comp}" ; export CMTOPT
    		rh="slc3_ia32"
    		if  [ ! -d ${COMPILER_PATH} ] ; then
      			echo "$comp compiler is not available on this node"
      			return 
    		fi
    		PATH=${COMPILER_PATH}/bin:$PATH; export PATH
    		LD_LIBRARY_PATH="${COMPILER_PATH}/lib" ; export LD_LIBRARY_PATH
		fi
	fi
  #============================================
	binary=${rh}_${comp}
	if [ "$hw" = "ia64" ] ; then
		binary=`echo $binary | sed -e 's/ia64/amd64/'`
	fi
	CMTOPT=$binary ; export CMTOPT
fi

export CMTCONFIG="${CMTOPT}"
export CMTDEB="${CMTCONFIG}_dbg"
if [ "$debug" = "debug" ] ; then
	export CMTCONFIG="${CMTDEB}"
fi
set -


if [ !  ${LD_LIBRARY_PATH} ];then
	export LD_LIBRARY_PATH=""
fi
if [ ! ${ROOTSYS} ]; then
	export ROOTSYS=""
fi

echo "******************************************************"
echo "*           WELCOME to the $comp on $rh system       *"
echo "******************************************************"
echo " --- "\$CMTROOT " is set to $CMTROOT "
echo " --- "\$CMTCONFIG " is set to $CMTCONFIG "
if [ "$debug" != "debug" ] ; then
	echo " --- to compile and link in debug mode : export CMTCONFIG="\$CMTDEB "; gmake"
fi
if [ "$CMTPATH" != "" ]; then
	echo " --- "\$CMTPATH " is set to ${origin}/${work}"
else
	echo " --- "\$User_release_area " is set to ${origin}/${work}"
	echo " --- "\$CMTPROJECTPATH " is set to "\$User_release_area":"\$LHCb_release_area":"\$Gaudi_release_area":"\$LCG_release_area
	echo " --- projects will be searched in "\$CMTPROJECTPATH
fi 
	echo " -------------------------------------------------------------------- "

#==========================================================================

unset newcomp rh rhv comp cmtvers
unset debug mysiteroot cmtconfig userarea cmtvers
#set -
