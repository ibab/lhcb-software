#!/bin/csh
killall test.exe
rm /dev/shm/bm_*
rm /dev/shm/sem.BM*
rm /dev/shm/sem.bm*
rm /dev/shm/sem.MEP*
setenv EXEC '${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe'
setenv MINITERM 'xterm -ls -132 -e'
setenv GENOPT
#   '' | more'
$MINITERM "$EXEC mbm_install -s=8096 -e=64 -u=64 -i=0" &
sleep 1
xterm -ls -132 -geometry 132x45 -e "$EXEC mbm_mon" &
#
# gvd ${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe &
#
$MINITERM "$EXEC mbm_cons -name=cons_s_0        $GENOPT" &
$MINITERM "$EXEC mbm_cons -name=cons_s_1        $GENOPT" &
#
$MINITERM "$EXEC mbm_cons -name=cons_o_0 -one   $GENOPT" &
$MINITERM "$EXEC mbm_cons -name=cons_o_1 -one   $GENOPT" &
#
# Does not work - problem in synchronization of asynchronous events
# $MINITERM "$EXEC mbm_cons -name=cons_a_0 -async $GENOPT" &
# $MINITERM "$EXEC mbm_cons -name=cons_a_1 -async $GENOPT" &
#
$MINITERM "$EXEC mbm_cons -name=cons_u_0 -user -s=1 $GENOPT" &
$MINITERM "$EXEC mbm_cons -name=cons_u_1 -user -s=1 $GENOPT" &
#
echo "$EXEC mbm_prod     -n=prod_0    -m=1000  -s=3500"
