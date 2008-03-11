#!/bin/bash
if test -f /tmp/CreateMEP.opts;
then
  rm -f /tmp/CreateMEP.opts;
fi

CREATETAE=false;
if test "${1}" = "-tae";
then
    CREATETAE=true;
    echo "We are creating TAE events.....";
fi

cat >/tmp/CreateMEP.opts <<END-OF-OPTS
#include "$MDFROOT/options/CreateMEP.opts"
ApplicationMgr.OutStream  = { "LHCb::MEPWriter/Writer_0" };
Writer_0.Compress       = 0;
Writer_0.ChecksumType   = 0;
Writer_0.PackingFactor  = 10;
Writer_0.Connection     = "file://mepData_0.dat";
Writer_0.MakeTAE        = ${CREATETAE};
END-OF-OPTS
#
cat /tmp/CreateMEP.opts
$ONLINEKERNELROOT/$CMTCONFIG/gentest.exe libGaudiKernel.so GaudiMain /tmp/CreateMEP.opts
rm /tmp/CreateMEP.opts
