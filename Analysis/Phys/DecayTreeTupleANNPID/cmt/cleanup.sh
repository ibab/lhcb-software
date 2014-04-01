if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/lhcb.cern.ch/lib/contrib/CMT/v1r20p20090520; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt cleanup -sh -pack=DecayTreeTupleANNPID -version=v1r0 -path=/var/clus/usera/jonesc/cmtuser/DaVinci_v34r0/Phys $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

