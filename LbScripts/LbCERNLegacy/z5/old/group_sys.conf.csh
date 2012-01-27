##############################
# LHC-B environment for (t)csh
#
# 7.12.95 A.Tsaregorodtsev
##############################
#
#echo --------------------
#echo 'Executing LHC-B (z5) group_sys.conf'
#echo "PATH= " $PATH
#echo --------------------

if ( -e ${HOME}/.oldLHCBLoginscript ) then


if ( -r /afs/cern.ch/lhcb/scripts/lhcbsetup.csh ) then
  source /afs/cern.ch/lhcb/scripts/lhcbsetup.csh
endif
#echo "exit group_sys.conf"

endif
