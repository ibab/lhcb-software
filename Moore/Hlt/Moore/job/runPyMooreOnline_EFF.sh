#!/bin/sh
# pick up the online setup...
source ./setupOnline.sh $*
# pick up 'our' setup... (which defines $MOOREROOT!)
source /project/bfys/graven/MOORE/Moore_v7r1/Hlt/Moore/job/SetupMoore_v7r1.sh

TAE='False'
[[ 'x'$IS_TAE_RUN != 'x' ]] && TAE='True'
USEROPTS=command="import Moore.runOnline; runOnline.start(TAE=${TAE})"

echo exec -a ${UTGID} ${CLASS1_TASK} -opt=${USEROPTS}
exec -a ${UTGID} ${CLASS1_TASK} -opt=${USEROPTS}


