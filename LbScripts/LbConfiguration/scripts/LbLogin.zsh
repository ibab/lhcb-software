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

# LHCb release area
SOFTWARE="/afs/cern.ch/lhcb/software"; export SOFTWARE
LHCBRELEASES="$SOFTWARE/releases";export LHCBRELEASES


# get .rootauthrc file if not yet there
if [ ! -f $HOME/.rootauthrc ] ; then
	cp /afs/cern.ch/lhcb/scripts/.rootauthrc $HOME/.
fi

echo " -------------------------------------------------------------------"


# Gaudi release area and cvs repository
GAUDISOFT="$SITEROOT/sw/Gaudi/releases"; export GAUDISOFT
GAUDIDEV="$SITEROOT/sw/Gaudi/dev"; export GAUDIDEV
Gaudi_release_area="$GAUDISOFT"; export Gaudi_release_area
LCG_release_area="/afs/cern.ch/sw/lcg/app/releases"; export LCG_release_area



# new emacs environment
EMACSDIR="$LHCBRELEASES/TOOLS/Tools/Emacs/pro"; export EMACSDIR

# LHCb styles configuration
LHCBSTYLE="$LHCBRELEASES/TOOLS/Tools/Styles/pro"; export LHCBSTYLE

# deal with different linux distributions
if [ -e /etc/redhat-release ] ; then
	distrib=`cat /etc/redhat-release | awk '{print $1}'`
	rhv=`cat /etc/redhat-release | awk '{print $5}'`
	if [ "$distrib" = "Scientific" ] ; then
		hw=`uname -i`
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
elif [ -e /etc/debian_version ] ; then
	deb=`cat /etc/debian_version`
	if [ "$deb" = "3.0" ] ; then
		rh="rh73"
	else
		rh="deb$deb"
	fi
	distrib="debian"
fi


# deal with OS type ===========================================
if [ "$OSTYPE" = "darwin" ] ; then
	rh=`sw_vers | grep ProductVersion | awk '{print $2}' | awk -F . '{print $1 $2}'`
	rh="osx$rh"
	comp=`gcc --version | grep gcc | awk '{print $3}' | tr -d "."`
	comp="gcc$comp"
elif [ "$OSTYPE" = "linux-gnu" -o "$OSTYPE" = "linux" ] ; then
# get the compiler from the arguments
	comp="$*"
	if [ "W$comp" = "W" ] ; then
		comp="gcc323"
	fi
#================== redhat distribustion ==========================
	if [ "$distrib" = "Red" ] ; then
		echo $comp
		if [ "$comp" = "gcc323" ] ; then
			COMPILER_PATH="/usr/local/gcc-alt-3.2.3"; export COMPILER_PATH
		elif [ "$comp" = "gcc32" ] ; then
			COMPILER_PATH="/usr/local/gcc-alt-3.2"; export COMPILER_PATH
		else
			echo "$comp compiler is unknown"
			goto end
		fi
		if ! [ -d ${COMPILER_PATH} ] ; then
			echo "$comp compiler is not available on this node"
			return
		fi
		PATH=${COMPILER_PATH}/bin:$PATH; export PATH
		LD_LIBRARY_PATH=${COMPILER_PATH}/lib; export LD_LIBRARY_PATH    
# ======= debian distribution
	elif [ "$distrib" = "debian" ] ;then
		if [ "$comp" = "gcc30" ] ; then
			compiler="/usr/bin/gcc-3.0"
		elif [ "$comp" = "gcc32" ] ; then
			compiler="/usr/bin/gcc-3.2"
		elif [ "$comp" = "gcc33" ] ; then
			compiler="/usr/bin/gcc-3.3"
		else
			echo "$comp compiler is unknown"
			return
		fi 
		if ! [ -x $compiler ] ; then
			echo "$comp compiler is not available on this node"
			return
		fi 
		alias gcc='$compiler'
	fi

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
  
