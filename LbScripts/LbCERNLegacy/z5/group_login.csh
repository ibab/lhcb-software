##############################
# CMTenvironment for (t)csh
#
# F.Ranjard 990401
##############################
#
if ! ( -e ${HOME}/.nogrouplogin || -e ${HOME}/.noLHCBLoginscript ) then

if ( -e ${HOME}/.oldLHCBLoginscript ) then
if ( ! -e ${HOME}/.rhosts ) then
   echo "Creating a ${HOME}/.rhosts to use CMT"
   echo " "
   echo "Joel.Closier@cern.ch"
   echo "+ ${USER}" > ${HOME}/.rhosts
endif
alias lbcmt 'source /afs/cern.ch/lhcb/scripts/CMT.csh'
alias GangaEnv "source /afs/cern.ch/sw/ganga/install/etc/setup-lhcb.csh"


echo --------------------
echo 'Executing CMT setting'
echo --------------------
if ($#argv != 0) shift
if ( -r /afs/cern.ch/lhcb/scripts/CMT.csh ) then
  source /afs/cern.ch/lhcb/scripts/CMT.csh
endif
#zlocate $USER | grep Hidden > /dev/null
#if ( $status == 0 ) then
# This may not be X-connected, so start zwgc in ttymode
#      echo 'I am starting a zwgc client for you in dumb terminal mode'
#      zwgc -nofork -ttymode &
# Subscribe to the lhcb zephyr class
#     (sleep 2; zctl add lhcb \* \*)
#endif

#alias netscape7 '/afs/cern.ch/lhcb/project/software/netscape/netscape&'

else
  # Post setup, CMTCONFIG specific (compilers setup for example)
  setenv LOGIN_POST_SCRIPT ${HPX_HEPGROUP_DIR}/post/login
  # Post setup, site wide, does not depend on CMTCONFIG (X509 vars for example)
  setenv LHCB_POST_SCRIPT ${HPX_HEPGROUP_DIR}/post/site
  set lbvers=prod
  if ( -e ${HOME}/.prevLHCBLoginscript ) then
    set lbvers=prev
  endif
  if ( -e ${HOME}/.devLHCBLoginscript ) then
    set lbvers=dev
  endif
  if ( $?LB_BANNER ) then
     cat ${LB_BANNER}
     rm -f ${LB_BANNER}
     unsetenv LB_BANNER
  else
     source /afs/cern.ch/lhcb/software/releases/LBSCRIPTS/$lbvers/InstallArea/scripts/LbLogin.csh --quiet
  endif
  setenv LBLOGIN_DONE yes
  unset lbvers
endif

if ( ! $?LHCB_POST_DONE ) then
  if ( $?LHCB_POST_SCRIPT ) then
    if ( -r $LHCB_POST_SCRIPT.csh ) then
      source $LHCB_POST_SCRIPT.csh
      setenv LHCB_POST_DONE yes
    endif
  endif
endif

endif


