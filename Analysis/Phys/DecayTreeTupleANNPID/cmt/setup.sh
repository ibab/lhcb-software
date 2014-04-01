# echo "Setting DecayTreeTupleANNPID v1r0 in /usera/jonesc/cmtuser/DaVinci_v34r0/Phys"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/lhcb.cern.ch/lib/contrib/CMT/v1r20p20090520; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=DecayTreeTupleANNPID -version=v1r0 -path=/usera/jonesc/cmtuser/DaVinci_v34r0/Phys  -no_cleanup $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

