#!/bin/sh
# pick up the online setup...
source ./setupOnline.sh $*
# pick up 'our' setup... (which defines $MOOREROOT!)
source /group/hlt/MOORE/Moore_v6r1/Hlt/Moore/job/SetupMoore_v6r1.sh
# and the options to be used
export USEROPTS=/group/hlt/MOORE/Moore_v6r1/Hlt/Moore/options/DEFAULT${IS_TAE_RUN}.opts
[[ 'x'$PARTNAME == 'xFEST' ]] && USEROPTS=/group/hlt/MOORE/Moore_v6r1/Hlt/Moore/options/DEFAULT_FEST.opts
[[ 'x'$PARTNAME == 'xLHCb' ]] && USEROPTS=/group/hlt/MOORE/Moore_v6r1/Hlt/Moore/options/DEFAULT_LHCb.opts
export CORAL_SQLITE_TEMP_STORE_MEMORY=yes
# and run them!
echo exec -a ${UTGID} ${CLASS1_TASK} -opt=${USEROPTS}
exec -a ${UTGID} ${CLASS1_TASK} -opt=${USEROPTS}
      