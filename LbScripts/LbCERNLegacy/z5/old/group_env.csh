##############################
# LHC-B environment for (t)csh
#
# 7.12.95 A.Tsaregorodtsev
##############################
#
#echo --------------------
#echo 'Executing LHC-B (z5) group_env.csh'
#echo "PATH= " $PATH
#echo --------------------
if ( -e ${HOME}/.oldLHCBLoginscript ) then

if ( -r /afs/cern.ch/lhcb/scripts/lhcbsetup.csh ) then
#echo "call lhcbsetup"
  source /afs/cern.ch/lhcb/scripts/lhcbsetup.csh
endif
#echo "exit group_env.csh"

endif
