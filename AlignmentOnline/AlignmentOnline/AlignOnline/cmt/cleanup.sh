if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/lhcb.cern.ch/lib/contrib/CMT/v1r20p20090520; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt cleanup -sh -pack=AlignOnline -version=v1r0 -path=/group/online/dataflow/cmtuser/ALIGNMENTONLINE/ALIGNMENTONLINE_HEAD/AlignmentOnline $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

