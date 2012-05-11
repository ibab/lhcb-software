##############################
# CMTenvironment for (z)sh
#
# F.Ranjard 041213
##############################
#
if ! [ -e ${HOME}/.nogrouplogin -o -e ${HOME}/.noLHCBLoginscript ] ; then

if [ -e ${HOME}/.oldLHCBLoginscript ] ; then
if [ ! -e ${HOME}/.rhosts ]; then
   echo "Creating a ${HOME}/.rhosts to use CMT"
   echo " "
   echo "Joel.Closier@cern.ch"
   echo "+ ${USER}" > ${HOME}/.rhosts
fi
alias lbcmt='. /afs/cern.ch/lhcb/scripts/CMT.sh'
alias newcmt='. /afs/cern.ch/lhcb/scripts/newCMT.sh'
alias oldcmt='. /afs/cern.ch/lhcb/scripts/oldCMT.sh'

alias GangaEnv='. /afs/cern.ch/sw/ganga/install/etc/setup-lhcb.sh'

echo --------------------
echo 'Executing CMT setting'
echo --------------------
if ! [ $# = 0 ]; then
  shift
fi

if [ -r /afs/cern.ch/lhcb/scripts/CMT.sh ]; then
  .  /afs/cern.ch/lhcb/scripts/CMT.sh
fi
#if [ `zlocate $USER | grep Hidden > /dev/null` ]; then
# This may not be X-connected, so start zwgc in ttymode
#      echo 'I am starting a zwgc client for you in dumb terminal mode'
#      zwgc -nofork -ttymode &
# Subscribe to the aleph zephyr class
#     (sleep 2; zctl add lhcb \* \*)
#fi

#alias netscape7='/afs/cern.ch/lhcb/project/software/netscape/netscape&'

else
  # Post setup, CMTCONFIG specific (compilers setup for example)
  export LOGIN_POST_SCRIPT=${GROUP_DIR}/post/login
  # Post setup, site wide, does not depend on CMTCONFIG (X509 vars for example)
  export LHCB_POST_SCRIPT=${GROUP_DIR}/post/site
  lbvers=prod
  if [ -e ${HOME}/.prevLHCBLoginscript ]; then
    lbvers=prev
  fi
  if [ -e ${HOME}/.devLHCBLoginscript ]; then
    lbvers=dev
  fi
  if [ -n "$LB_BANNER" ]; then
     cat ${LB_BANNER}
     rm -f ${LB_BANNER}
     unset LB_BANNER
  else
    use_cvmfs=0
    if [[ -e ${HOME}/.devLHCBuseCVMFS && -e /cvmfs/lhcb.cern.ch/lib/lhcb/LBSCRIPTS/prod/InstallArea/scripts/LbLogin.sh ]]; then
      use_cvmfs=1
    fi

    if [[ $ENVIRONMENT = "BATCH" && -e /cvmfs/lhcb.cern.ch/lib/lhcb/LBSCRIPTS/prod/InstallArea/scripts/LbLogin.sh  && ! -e ${HOME}/.devLHCBuseAFS ]]; then
      use_cvmfs=1
    fi
    if [ $use_cvmfs -eq 1 ]; then
      . /cvmfs/lhcb.cern.ch/lib/lhcb/LBSCRIPTS/prod/InstallArea/scripts/LbLogin.sh --quiet
    else
      . /afs/cern.ch/lhcb/software/releases/LBSCRIPTS/$lbvers/InstallArea/scripts/LbLogin.sh --quiet 
    fi
  fi
  export LBLOGIN_DONE=yes
  unset lbvers
fi

if [ ! -n "$LHCB_POST_DONE" ]; then
  if [ -n "$LHCB_POST_SCRIPT" ]; then
    if [ -r $LHCB_POST_SCRIPT.sh ]; then
      . $LHCB_POST_SCRIPT.sh
      export LHCB_POST_DONE=yes
    fi
  fi
fi

fi

. ${GROUP_DIR}/extras.sh


