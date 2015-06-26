if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/cern.ch/sw/contrib/CMT/v1r20p20090520; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt cleanup -sh -pack=Production -version=v1r0 -path=/afs/cern.ch/user/l/landerli/cmtuser/DaVinci_v36r7p2/PIDCalib/ $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

