##############################
# CMTenvironment for zsh
#
# F.Ranjard 990401
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
alias lbcmt='/afs/cern.ch/lhcb/scripts/CMT.zsh'
alias oldcmt='. /afs/cern.ch/lhcb/scripts/oldCMT.sh'

alias GangaEnv='. /afs/cern.ch/sw/ganga/install/etc/setup.sh'

echo --------------------
echo 'Executing CMT setting'
echo --------------------
if [ -r /afs/cern.ch/lhcb/scripts/CMT.zsh ]; then
  . /afs/cern.ch/lhcb/scripts/CMT.zsh
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
  # Post setup, site wide, does not depend on CMTCONFIG (X509 vars for example)
  export LHCB_POST_SCRIPT=${HPX_HEPGROUP_DIR}/post/site
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
     . /afs/cern.ch/lhcb/software/releases/LBSCRIPTS/$lbvers/InstallArea/scripts/LbLogin.sh --quiet 
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

