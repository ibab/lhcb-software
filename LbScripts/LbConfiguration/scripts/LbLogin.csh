
# get the location of this very script

# set echo
unalias cwdcmd

set a = `history -rh 1 | awk '{print $2}'`
echo $a
#set b = `basename $a`
set l = `readlink $a`
if ( $l == "" ) then
	set l = $a
	set scriptsdir = `dirname $a`
else
	set d = `dirname $a`
	if ( $d != $cwd && $d != "." ) then
		pushd $d # >& /dev/null
  	endif
  	pushd `dirname $l` # >& /dev/null
  	set scriptsdir = `pwd`
  	popd # >& /dev/null
  	if ( $d != $cwd && $d != "." ) then
    	popd # >& /dev/null
  	endif
endif

unset a
unset l
unset d

if ( $scriptsdir == ".") then
	set scriptsdir = `pwd`
endif
echo $scriptsdir


#################################################################
# Parsing command line arguments
# cloned from the getopt examples

set debug = 0
set mysiteroot = 0
set cmtconfig = 0
set userarea = 0
set cmtvers = 0

set temp=(`getopt -s tcsh -o d,m:,c:,u:,v: --long debug,mysiteroot:,cmtconfig:,userarea:,cmtvers: -- $argv:q`)
if ($? != 0) then 
	echo "Terminating..." >/dev/stderr
	exit 1
endif

eval set argv=\($temp:q\)

while (1)
	switch($1:q)
	case -d:
	case --debug:
		set debug = 1 ; shift
		breaksw ;		
	case -m:
    case --mysiteroot:
    	set mysiteroot = $2 ; shift ; shift 
      	breaksw;
    case -c:
   	case --cmtconfig:
		set cmtconfig = $2  ; shift ; shift
		breaksw
   	case -u:
	case --userarea:
		set userarea = $2 ;  shift; shift
		breaksw
	case -v:
	case --cmtvers:
		set cmtvers = $2 ;  shift; shift
		breaksw
    case --:
		shift
		break
	default:
		echo "Internal error!" ; exit 1
    endsw
end

echo "Remaining arguments:"
# foreach el ($argv:q) created problems for some tcsh-versions (at least
# 6.02). So we use another shift-loop here:
while ($#argv > 0)
	echo '--> '\`$1:q\'
    shift
end

unset temp

#################################################################

if ( ! -e ${HOME}/.rhosts ) then
	echo "Creating a ${HOME}/.rhosts to use CMT"
   	echo " "
   	echo "Joel.Closier@cern.ch"
   	echo "+ ${USER}" > ${HOME}/.rhosts
endif

# remove any .cmtrc file stored in the $HOME directory
if (-f ${HOME}/.cmtrc ) /bin/rm  ${HOME}/.cmtrc

# clear PATH and LD_LIBRARY_PATH
if ("$OSTYPE" == "linux" && $?SAVEPATH && "$PATH" != "$SAVEPATH") then
  setenv PATH $SAVEPATH
endif

unsetenv LD_LIBRARY_PATH
unsetenv COMPILER_PATH
unsetenv GCC_EXEC_PREFIX


###################################################################################
# site independent setup
# Gaudi:
setenv GAUDIKSERVER ":kserver:isscvs.cern.ch:/local/reps/Gaudi"
# LHCb
setenv LHCBKSERVER ":kserver:isscvs.cern.ch:/local/reps/lhcb"

###################################################################################
# MYSITEROOT massaging
if ( $mysiteroot != 0 ) then
	setenv MYSITEROOT $mysiteroot
	setenv SITEROOT $MYSITEROOT
	setenv CMTSITE LOCAL
else
	if ("$?MYSITEROOT") then
		setenv SITEROOT $MYSITEROOT
		set mysiteroot $MYSITEROOT
		setenv CMTSITE LOCAL		
    else
    	if (-d /afs/cern.ch) then
			setenv CMTSITE CERN
			setenv SITEROOT /afs/cern.ch
			setenv AFSROOT /afs    		
    	else
			echo " "\$MYSITEROOT" is not defined"
			echo " we suggest you install all software under" \$MYSITEROOT
			echo " then LHCb software will be installed under" \$MYSITEROOT"/lhcb"
			echo "      LCG software will be installed under" \$MYSITEROOT"/lcg/external"
			echo "      CMT and OpenScientist will be installed under" \$MYSITEROOT"/contrib"
			echo " as an example "
			echo " setenv "\$MYSITEROOT" /home/ranjard/Install"
			echo ""
			exit
    	endif
	endif
endif

###################################################################################
# setup contrib area

if ( "$CMTSITE" == "CERN") then 
	setenv CONTRIBDIR $SITEROOT/sw/contrib
else
	setenv CONTRIBDIR $MYSITEROOT/contrib 
endif


###################################################################################
# configure CMT

if ( $cmtvers == 0 ) then
	set cmtvers = 'v1r20p20070208'
	if ( `uname` == Darwin ) then
		set cmtvers = 'v1r20p20070524'
	endif
endif

setenv CMT_DIR $CONTRIBDIR

source $CMT_DIR/CMT/$cmtvers/mgr/setup.csh


###################################################################################
# shared area massaging
# On AFS the shared area is /afs/cern.ch/project/gd/apps/lhcb/lib

###################################################################################
echo "debug $debug"
echo "mysiteroot $mysiteroot"
echo "cmtconfig $cmtconfig"
echo "userarea $userarea"
echo "cmtvers $cmtvers"


echo "Exiting ..."
exit
echo "Exited"

#set echo on

# set default values for CMT and gcc versions 
set comp = `gcc --version | grep gcc | awk '{print $3}' | tr -d "."`
if ("$comp" >= "340") then
  set gcc = `gcc --version | grep gcc | awk '{print $3}' | awk -F. '{for(i=1; i<=2; i++){print $i}}'`
  set comp = `echo $gcc | tr -d " "`
endif


set compdef = gcc$comp


set debug = ''
unset newcomp

# get CMT and/or gcc versions from arguments if any
set nar = $#argv
while ($nar > 0) 
	set argn = $argv[${nar}]
	if ($argn == 'HEAD') then
		set cmtvers = 'HEAD'
	else
		if ($argn == 'debug') then
			set debug = $argn
		else
			set v = `echo $argn | grep 'v1r'`
			if ($v != '') then
				set cmtvers = $v
			else
				set newcomp = $argn
			endif
		endif
	endif
	set argv[${nar}] = ""
	@ nar = ${nar} - 1
end



echo " -------------------------------------------------------------------"


set origin = $HOME
set work = "cmtuser"

# User_release_area
if !(-d $origin/${work}) then
	mkdir -p $origin/${work}
	fs setacl $origin/${work} system:anyuser rl
	echo " --- ${work} directory has been created in your HOME directory"
	echo " --- with public access (readonly)"
	echo " --- use mkprivat to remove public access to the current directory"
	echo " --- use mkpublic to give public access to the current directory"
endif

if !("$?User_release_area") setenv User_release_area $origin/$work
if (-d ${User_release_area}/cmt) rm -fr ${User_release_area}/cmt

# to work with rootd the .rootauthrc file is required
if !(-f $HOME/.rootauthrc) then
	cp  /afs/cern.ch/lhcb/scripts/.rootauthrc $HOME/.
endif 

# Gaudi release area
setenv GAUDISOFT $SITEROOT/sw/Gaudi/releases
setenv Gaudi_release_area ${GAUDISOFT}

# LCG release area
setenv LCG_release_area /afs/cern.ch/sw/lcg/app/releases

# DIM release area
setenv DIM_release_area /afs/cern.ch/lhcb/online/control

# LHCb release area
setenv LHCBRELEASES $SITEROOT/lhcb/software/releases
setenv LHCBDEV $SITEROOT/lhcb/software/DEV
setenv LHCBDOC $LHCBRELEASES/DOC

# LHCb project path
setenv LHCBPROJECTPATH ${LHCBRELEASES}:${GAUDISOFT}:${LCG_release_area}


#===========================================================================
# set CMTPATH or CMTPROJECTPATH
# if CMTPROJECTPATH is set beforehand keep it ( test)
if ("$?CMTPROJECTPATH")then
	unsetenv CMTPATH
else
# today set CMTPROJECTPATH if the version of CMT is at least v1r20
	echo `cmt version` | grep v1r20 > /dev/null
	if ( $status == 1 ) then
		setenv CMTPATH $User_release_area
		unsetenv CMTPROJECTPATH
	else
		unsetenv CMTPATH
		setenv CMTPROJECTPATH ${User_release_area}:${LHCb_release_area}:${Gaudi_release_area}:${LCG_release_area}
	endif
endif


#=============================================================
# emacs configuration file
setenv EMACSDIR $LHCBRELEASES/TOOLS/Tools/Emacs/pro

#LHCb styles configuration
setenv LHCBSTYLE $LHCBRELEASES/TOOLS/Tools/Styles/pro

#==============================================================
# deal with different linux distributions
if (-e /etc/redhat-release) then
	set distrib = `cat /etc/redhat-release | awk '{print $1}'` 
	set rhv = `cat /etc/redhat-release | tr -d '[a-z][A-Z]()'`
	if ("$distrib" == "Scientific") then
		set nativehw = `uname -i`
		set hw = $nativehw
		if ($hw == "i386") set hw = "ia32"
		if ($hw == "x86_64") set hw = "amd64"
		set rhv = `echo ${rhv} | awk -F "." '{print $1}'`
		set rh = "slc"${rhv}"_"${hw}
	else
		set rhv = `echo ${rhv} | tr -d "."`
		set rh = "rh$rhv"
	endif 
endif

# deal with OS type ===========================================
if ( `uname -s` == "Darwin" ) then
  set rh = `sw_vers | grep ProductVersion | awk '{print $2}' | awk -F . '{print $1 $2}'`
  set rh = "osx$rh"
  set comp = `gcc --version | grep gcc | awk '{print $3}' | tr -d "."`
  set comp = "gcc$comp"
  if ( `uname -p` == 'powerpc' ) then
    set hw = 'ppc'
  else
    set hw = 'ia32'
  endif
  set rh = "${rh}_${hw}"
  setenv CMTOPT ${rh}_${comp}
#
else if ( "$OSTYPE" == "linux" ) then
# get the compiler from the arguments
  if ("$?newcomp" == "0") then
    set comp = $compdef
  else
    set comp = $newcomp
  endif

#======= compile on SLC4 pretending it is SLC3
  if ( $comp == "gcc323" && $rhv != "3") then
  if ( $nativehw == "x86_64" || $nativehw == "i386" ) then
    setenv COMPILER_PATH "/afs/cern.ch/lhcb/externallib/SLC3COMPAT/slc3_ia32_gcc323"
    setenv CMTOPT "slc3_ia32_${comp}"
    set rh = "slc3_ia32"
    if !(-d $COMPILER_PATH ) then
       echo "$comp compiler is not available on this node"
       goto end
    endif
    set path =(${COMPILER_PATH}/bin $path)
    setenv LD_LIBRARY_PATH "${COMPILER_PATH}/lib"
  endif
endif

#==========================
  set binary =  ${rh}_${comp}
  if ($hw == "ia64") set binary = `echo $binary | sed -e 's/ia64/amd64/'`
  setenv CMTOPT  ${binary}
endif
unset echo

setenv CMTDEB    "${CMTOPT}_dbg"
setenv CMTCONFIG "${CMTOPT}"
if ($debug == "debug") setenv CMTCONFIG "$CMTDEB"


#=================================================================================================

if ( ! $?LD_LIBRARY_PATH ) setenv LD_LIBRARY_PATH
if ( ! $?ROOTSYS ) setenv ROOTSYS

echo "******************************************************"
echo "*           WELCOME to the $comp on $rh system       *"
echo "******************************************************"
echo " --- "\$CMTROOT " is set to $CMTROOT "
echo " --- "\$CMTCONFIG " is set to $CMTCONFIG "
	if ($debug != 'debug') echo " --- to compile and link in debug mode : setenv CMTCONFIG "\$CMTDEB" ; gmake"
  	if ($?CMTPATH) then
    	echo " --- "\$CMTPATH " is set to ${origin}/${work}" 
  	else
    	echo " --- "\$User_release_area " is set to ${User_release_area}" 
    	echo " --- "\$CMTPROJECTPATH "is set to "\$User_release_area ":"\$LHCb_release_area":"\$Gaudi_release_area":"\$LCG_release_area
    	echo " --- projects will be searched in "\$CMTPROJECTPATH
  	endif 
  	echo " -------------------------------------------------------------------- "
end:
unset newcomp, rh, rhv, comp, cmtvers
unset debug, mysiteroot, cmtconfig, userarea, cmtvers
exit
