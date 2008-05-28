
# get the location of this very script

# set echo
unalias cwdcmd

set a = `history -rh 1 | awk '{print $2}'`
# echo $a
#set b = `/bin/basename $a`
set l = `/usr/bin/readlink $a`
if ( $l == "" ) then
	set l = $a
	set scriptsdir = `/usr/bin/dirname $a`
else
	set d = `/usr/bin/dirname $a`
	if ( $d != $cwd && $d != "." ) then
		pushd $d # >& /dev/null
  	endif
  	pushd `/usr/bin/dirname $l` # >& /dev/null
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
# echo $scriptsdir

#################################################################
# Parsing command line arguments
# cloned from the getopt examples

set debug = 0
set mysiteroot = 0
set cmtconfig = 0
set userarea = 0
set cmtvers = 0
set sharedarea = 0

set temp=(`getopt -s tcsh -o d,m:,c:,u:,v:,s --long debug,mysiteroot:,cmtconfig:,userarea:,cmtvers:,shared -- $argv:q`)
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
	case -s:
	case --shared:
		set sharedarea = 1 ;  shift
		breaksw
    case --:
		shift
		break
	default:
		echo "Internal error!" ; exit 1
    endsw
end

# echo "Remaining arguments:"
# foreach el ($argv:q) created problems for some tcsh-versions (at least
# 6.02). So we use another shift-loop here:
# while ($#argv > 0)
#	echo '--> '\`$1:q\'
#    shift
#end

unset temp

#################################################################

# clear PATH and LD_LIBRARY_PATH
if ("$OSTYPE" == "linux" ) then
	if ( $?SAVEPATH ) then
		if ( "$PATH" != "$SAVEPATH") then
  			setenv PATH $SAVEPATH
  		endif
  	endif
endif

unsetenv LD_LIBRARY_PATH
unsetenv COMPILER_PATH
unsetenv GCC_EXEC_PREFIX
unsetenv PYTHONPATH


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
			echo ' $MYSITEROOT is not defined'
			echo ' we suggest you install all software under $MYSITEROOT'
			echo ' then LHCb software will be installed under $MYSITEROOT/lhcb'
			echo '      LCG software will be installed under $MYSITEROOT/lcg/external'
			echo '      CMT and OpenScientist will be installed under $MYSITEROOT/contrib'
			echo ' as an example '
			echo ' setenv $MYSITEROOT /home/ranjard/Install'
			echo ''
			exit
    	endif
	endif
endif

###################################################################################
# User release area massaging

if ( $userarea != 0 ) then
	setenv User_release_area $userarea
else
	if !("$?User_release_area") then
		setenv User_release_area $HOME/cmtuser
	endif
endif

# User_release_area
if !(-d $User_release_area) then
	mkdir -p $User_release_area
	echo " --- a new ${User_release_area:t} directory has been created in your HOME directory"
	if ( "$CMTSITE" == "CERN" ) then
		fs setacl $User_release_area system:anyuser rl
		echo " --- with public access (readonly)"
		echo " --- use mkprivat to remove public access to the current directory"
		echo " --- use mkpublic to give public access to the current directory"
	endif
endif

if (-d ${User_release_area}/cmt) rm -fr ${User_release_area}/cmt

###################################################################################
# setup contrib area

if ("$CMTSITE" == "CERN") then 
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
# LHCb software locations

if ("$CMTSITE" == "LOCAL") then
	setenv LHCBHOME $MYSITEROOT
	setenv DIM_release_area $CONTRIBDIR
	setenv XMLRPC_release_area $CONTRIBDIR
	setenv LCG_release_area $MYSITEROOT/lcg/external
	setenv LHCBRELEASES $LHCBHOME/lhcb
	setenv GAUDISOFT $LHCBRELEASES
	setenv LHCBPROJECTPATH ${LHCb_release_area}:${LCG_release_area}
else
	setenv LHCBHOME $SITEROOT/lhcb
	setenv DIM_release_area $LHCBHOME/online/control
	setenv XMLRPC_release_area $LHCBHOME/project/web/online
	setenv LCG_release_area $SITEROOT/sw/lcg/app/releases
	setenv SOFTWARE $LHCBHOME/software
	setenv LHCBRELEASES $SOFTWARE/releases
	setenv GAUDISOFT $SITEROOT/sw/Gaudi/releases
	setenv LHCBPROJECTPATH ${LHCBRELEASES}:${GAUDISOFT}:${LCG_release_area}
	setenv LHCBDEV $SITEROOT/lhcb/software/DEV
	setenv LHCBDOC $LHCBRELEASES/DOC
	setenv EMACSDIR $LHCBRELEASES/TOOLS/Tools/Emacs/pro
	setenv LHCBSTYLE $LHCBRELEASES/TOOLS/Tools/Styles/pro
endif 

setenv OSC_release_area ${CONTRIBDIR}
setenv Gaudi_release_area ${GAUDISOFT}
setenv LHCb_release_area ${LHCBRELEASES}

###################################################################################
# shared area massaging
# On AFS the shared area is /afs/cern.ch/project/gd/apps/lhcb/lib

if ! ( $sharedarea ==  0 ) then
	if ($?VO_LHCB_SW_DIR) then
		export LHCBPROJECTPATH=${LHCBPROJECTPATH}:${VO_LHCB_SW_DIR}/lib/lhcb:${VO_LHCB_SW_DIR}/lib/lcg/external
	else
		echo 'You have requested a shared area but no VO_LHCB_SW_DIR is set.'
		echo 'Please define it.'
		exit
	endif
endif

###################################################################################
# set CMTPATH or CMTPROJECTPATH
# if CMTPROJECTPATH is set beforehand keep it
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
		setenv CMTPROJECTPATH ${User_release_area}:${LHCBPROJECTPATH}
	endif
endif

###################################################################################

# Guess the configuration
if ( "$OSTYPE" == "linux" || "$OSTYPE" == "linux-gnu" ) then
	set comp = `gcc --version | grep gcc | awk '{print $3}' | tr -d "."`
	if ("$comp" >= "340") then
  		set gcc = `gcc --version | grep gcc | awk '{print $3}' | awk -F. '{for(i=1; i<=2; i++){print $i}}'`
  		set comp = `echo $gcc | tr -d " "`
	endif
	set compdef = gcc$comp
	if (-e /etc/redhat-release) then
		set distrib = `cat /etc/redhat-release | awk '{print $1}'` 
		set rhv = `cat /etc/redhat-release | tr -d '[a-z][A-Z]()'`
		if ("$distrib" == "Scientific") then
			set rhv = `echo ${rhv} | awk -F "." '{print $1}'`
			set platform = "slc${rhv}"
		else
			set rhv = `echo ${rhv} | tr -d "."`
			set platform = "rh${rhv}" 
		endif
	endif

	set nativehw = `uname -i`
	set hw = $nativehw
	if ($hw == "i386") then 
		set hw = "ia32"
	else if ($hw == "x86_64") then 
		set hw = "amd64"
	else if ($hw == "ia64") then
		set hw ="amd64" 
	endif
	set binary = $hw
else if ( `uname -s` == "Darwin" ) then 
	set comp = `gcc --version | grep gcc | awk '{print $3}' | tr -d "."`
	set compdef = "gcc$comp"
	set rh = `sw_vers | grep ProductVersion | awk '{print $2}' | awk -F . '{print $1 $2}'`
	set platform = "osx$rh"
	if ( `uname -p` == 'powerpc' ) then
		set hw = 'ppc'
	else
		set hw = 'ia32'
	endif
	set binary = $hw
endif


# global override from the command line
if ( $cmtconfig != 0 ) then
	set conflist = `echo $cmtconfig | tr '_' ' '`
	if ( $#conflist > 2) then
		set platform = $conflist[1]
		set binary = $conflist[2]
		set compdef = $conflist[3]	
	endif 
	foreach c ( $conflist )
		if ( $c =~ gcc32* ) then
			set platform = slc3
			set binary = ia32
			set compdef = gcc323
			break
		endif 
		switch($c)
		case sl3:
			set platform = slc3
			set binary = ia32
			set compdef = gcc323
			break
			breaksw
		case sl4:
			set platform = slc4
			breaksw
		endsw
	end
endif
unset c, conflist 

# fixes compiler path at CERN for slc3 on a native slc4

if ("$CMTSITE" == "CERN") then 
	if ( "$compdef" == "gcc323" && "$rhv" != "3" ) then
    	setenv COMPILER_PATH "/afs/cern.ch/lhcb/externallib/SLC3COMPAT/slc3_ia32_gcc323"
    	if !(-d $COMPILER_PATH && -f /usr/bin/gcc32 ) then
       		echo "$compdef compiler is not available on this node"
       		goto end
    	endif
    	set path =(${COMPILER_PATH}/bin $path)
    	setenv LD_LIBRARY_PATH "${COMPILER_PATH}/lib"
	endif
endif

setenv CMTOPT ${platform}_${binary}_${compdef}

###################################################################################

setenv CMTDEB    "${CMTOPT}_dbg"
setenv CMTCONFIG "${CMTOPT}"
if ($debug == 1) then
	setenv CMTCONFIG "$CMTDEB"
endif


###################################################################################

if ( ! -e ${HOME}/.rhosts ) then
	echo "Creating a ${HOME}/.rhosts to use CMT"
   	echo " "
   	echo "Joel.Closier@cern.ch"
   	echo "+ ${USER}" > ${HOME}/.rhosts
endif

# remove any .cmtrc file stored in the $HOME directory
if (-f ${HOME}/.cmtrc ) /bin/rm  ${HOME}/.cmtrc
# to work with rootd the .rootauthrc file is required

if ("$CMTSITE" == "CERN") then 
	if !(-f $HOME/.rootauthrc) then
		cp  /afs/cern.ch/lhcb/scripts/.rootauthrc $HOME/.
	endif 
endif

if ( ! $?LD_LIBRARY_PATH ) then 
	setenv LD_LIBRARY_PATH
endif

if ( ! $?ROOTSYS ) then 
	setenv ROOTSYS
endif

echo "*****************************************************************"
echo "*           WELCOME to the $compdef on ${platform}_${binary} system       *"
echo "*           DEVELOPMENT SCRIPT                                  *"
echo "*****************************************************************"
echo " --- "\$CMTROOT " is set to $CMTROOT "
echo " --- "\$CMTCONFIG " is set to $CMTCONFIG "
	if ($debug != 1) echo " --- to compile and link in debug mode : setenv CMTCONFIG "\$CMTDEB" ; gmake"
  	if ($?CMTPATH) then
    	echo " --- "\$CMTPATH " is set to ${User_release_area}" 
  	else
    	echo " --- "\$User_release_area " is set to ${User_release_area}" 
    	echo " --- "\$CMTPROJECTPATH "is set to "\$User_release_area ":"\$LHCb_release_area":"\$Gaudi_release_area":"\$LCG_release_area
    	echo " --- projects will be searched in "\$CMTPROJECTPATH
  	endif 
  	echo " -------------------------------------------------------------------- "

###################################################################################
# setting up the LbScripts project

if (-f  $scriptsdir/SetupProject.csh) then
  	set mainscriptloc = $scriptsdir
else
	set mainscriptloc = $LHCBRELEASES/LBSCRIPTS/prod/InstallArea/scripts
endif
unset scriptsdir

#echo $mainscriptloc

setenv PATH ${PATH}:$mainscriptloc
source $mainscriptloc/SetupProject.csh LbScripts --runtime LCGCMT Python -v 2.5 > /dev/null
unset mainscriptloc

###################################################################################
# the end
end:
unset newcomp, rh, rhv, comp, cmtvers
unset debug, mysiteroot, cmtconfig, userarea, cmtvers, sharedarea
exit
