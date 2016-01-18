#!/bin/csh
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
killall test.exe
rm /dev/shm/sem.TAN_*
rm /dev/shm/TAN_*
set EXEC=$ONLINEKERNELROOT/$CMTCONFIG/test.exe
set MINITERM='xterm -geometry 80x12 -e '
set MIDITERM='xterm -geometry 100x12 -e '
$MINITERM "export PROCESSNAME=TANALLOC_0;   $EXEC tan_nameserver -a" &
sleep 2
$MINITERM "export PROCESSNAME=TANINQUIRE_0; $EXEC tan_nameserver -i -tcp" &
$MIDITERM "export PROCESSNAME=ALLOCTST_0;   $EXEC rtl_tan_alloc_test -c -q" &
sleep 2
$MIDITERM "export PROCESSNAME=INQUIRETST_0; $EXEC rtl_tan_inquire_test -c -q" &
sleep 2
xterm -e "export PROCESSNAME=TANMON_0;      $EXEC tanmon -c" &
export PROCESSNAME=TANDBDUMP_0; $EXEC rtl_tandb_dump
