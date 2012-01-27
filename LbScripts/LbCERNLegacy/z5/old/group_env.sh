##############################
# LHC-B environment for (z)sh
#
# 7.12.95 A.Tsaregorodtsev
##############################
#
#echo --------------------
#echo 'Executing LHC-B (z5) group profile'
#echo --------------------

if [ -e ${HOME}/.oldLHCBLoginscript ]; then

[ -r /afs/cern.ch/lhcb/scripts/lhcbsetup.sh ] && \
. /afs/cern.ch/lhcb/scripts/lhcbsetup.sh
#

fi
