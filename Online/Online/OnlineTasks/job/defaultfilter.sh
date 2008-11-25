#!/bin/sh
# pick up the online setup...
cd job
source ./setupOnline.sh $*
# pick up 'our' setup... (which defines $MOOREROOT!)
source /group/hlt/prod-Moore_v5r4/Moore_v5r4/Hlt/Moore/job/SetupMoore_v5r4.sh
# and the options to be used
export USEROPTS=/group/hlt/prod-Moore_v5r4/Moore_v5r4/Hlt/Moore/options/DEFAULT${IS_TAE_RUN}.opts
# and run them!
echo exec -a ${UTGID} ${CLASS1_TASK} -opt=${USEROPTS}
exec -a ${UTGID} ${CLASS1_TASK} -opt=${USEROPTS}
      