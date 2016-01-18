#!/bin/bash
#==========================================================================
#  LHCb Online software suite
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see OnlineSys/LICENSE.
#
# Author     : M.Frank
#
#==========================================================================
killall gentest.exe
pkill gentest.exe
rm /dev/shm/bm_*
rm /dev/shm/sem.BM*
rm /dev/shm/sem.bm*
rm /dev/shm/sem.MEP*
EXEC="`which gentest.exe` libOnlineKernel.so"
MINITERM='xterm -ls -132 -e'
#GENOPT=-fifo;$MINITERM "$EXEC mbm_install_server -s=8096 -e=64 -u=24 -i=0 -t=20 -f" &
GENOPT=-tcp;$MINITERM "$EXEC mbm_install_server -s=8096 -e=64 -u=32 -i=0 -t=8 -f -x" &
/bin/sleep 4
xterm -ls -132 -geometry 165x45 -e "$EXEC mbm_mon" &
#
# gvd ${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe &
#
#$MINITERM "$EXEC mbm_cons -name=cons_s_0        $GENOPT" &
#$MINITERM "$EXEC mbm_cons -name=cons_s_1        $GENOPT" &
#$MINITERM "$EXEC mbm_cons -name=cons_s_2        $GENOPT" &
#$MINITERM "$EXEC mbm_cons -name=cons_s_3        $GENOPT" &
#$MINITERM "$EXEC mbm_cons -name=cons_s_4        $GENOPT" &
#
$MINITERM "$EXEC mbm_cons -name=cons_o_0   -one -inc=0  $GENOPT" &
$MINITERM "$EXEC mbm_cons -name=cons_o_1   -one -inc=0  $GENOPT" &
$MINITERM "$EXEC mbm_cons -name=cons_o_2   -one -inc=0  $GENOPT" &
$MINITERM "$EXEC mbm_cons -name=cons_o_3   -one -inc=0  $GENOPT" &
$MINITERM "$EXEC mbm_cons -name=cons_o_4   -one -inc=0  $GENOPT" &
$MINITERM "$EXEC mbm_cons -name=cons_oo_0  -one -inc=2000  $GENOPT" &
$MINITERM "$EXEC mbm_cons -name=cons_oo_1  -one -inc=2000  $GENOPT" &
$MINITERM "$EXEC mbm_cons -name=cons_oo_2  -one -inc=2000  $GENOPT" &
$MINITERM "$EXEC mbm_cons -name=cons_oo_3  -one -inc=2000  $GENOPT" &
$MINITERM "$EXEC mbm_cons -name=cons_oo_4  -one -inc=2000  $GENOPT" &
#$MINITERM "$EXEC mbm_cons -name=cons_ooo_0 -one -inc=3000  $GENOPT" &
#$MINITERM "$EXEC mbm_cons -name=cons_ooo_1 -one -inc=3000  $GENOPT" &
#$MINITERM "$EXEC mbm_cons -name=cons_ooo_2 -one -inc=3000  $GENOPT" &
#$MINITERM "$EXEC mbm_cons -name=cons_ooo_3 -one -inc=3000  $GENOPT" &
#$MINITERM "$EXEC mbm_cons -name=cons_ooo_4 -one -inc=3000  $GENOPT" &
#
# Does not work - problem in synchronization of asynchronous events
# $MINITERM "$EXEC mbm_cons -name=cons_a_0 -async $GENOPT" &
# $MINITERM "$EXEC mbm_cons -name=cons_a_1 -async $GENOPT" &
#
$MINITERM "$EXEC mbm_cons -name=cons_u_0 -user -s=1 $GENOPT" &
$MINITERM "$EXEC mbm_cons -name=cons_u_1 -user -s=5 $GENOPT" &
#
/bin/sleep 4
$MINITERM "$EXEC mbm_prod     -n=prod_0    -m=100000  -s=3500 $GENOPT" &
$MINITERM "$EXEC mbm_prod     -n=prod_1    -m=100000  -s=3500 $GENOPT" &
$MINITERM "$EXEC mbm_prod     -n=prod_2    -m=100000  -s=3500 $GENOPT" &
$MINITERM "$EXEC mbm_prod     -n=prod_3    -m=100000  -s=3500 $GENOPT" &
$MINITERM "$EXEC mbm_prod     -n=prod_4    -m=100000  -s=3500 $GENOPT" &
$MINITERM "$EXEC mbm_prod     -n=prod_5    -m=100000  -s=3500 $GENOPT" &
$MINITERM "$EXEC mbm_prod     -n=prod_6    -m=100000  -s=3500 $GENOPT" &
#$MINITERM "$EXEC mbm_prod     -n=prod_7    -m=100000  -s=3500 $GENOPT" &
#$MINITERM "$EXEC mbm_prod     -n=prod_8    -m=100000  -s=3500 $GENOPT" &
#$MINITERM "$EXEC mbm_prod     -n=prod_9    -m=100000  -s=3500 $GENOPT" &
#$MINITERM "$EXEC mbm_prod     -n=prod_A    -m=100000  -s=3500 $GENOPT" &
echo "$EXEC mbm_prod     -n=prod_0    -m=1000  -s=3500"
