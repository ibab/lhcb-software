##############################
# CMTenvironment for (t)csh
#
# F.Ranjard 990401
##############################
#
if ( ! -e ${HOME}/.rhosts ) then
   echo "Creating a ${HOME}/.rhosts to use CMT"
   echo " "
   echo "Joel.Closier@cern.ch"
   echo "+ ${USER}" > ${HOME}/.rhosts
endif
alias lbcmt 'source /afs/cern.ch/lhcb/scripts/CMT.csh'
alias newcmt 'source /afs/cern.ch/lhcb/scripts/newCMT.csh'
alias oldcmt 'source /afs/cern.ch/lhcb/scripts/oldCMT.csh'
alias cmt64 'source /afs/cern.ch/lhcb/scripts/cmt64.csh'

alias GangaEnv "source /afs/cern.ch/sw/ganga/install/etc/setup.csh"

#setenv SAVEPATH $PATH


echo --------------------
echo 'Executing CMT setting'
echo --------------------
if ( -r /afs/cern.ch/lhcb/scripts/CMT.csh ) then
  if ($#argv != 0 ) shift
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


