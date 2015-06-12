# echo "Setting DataQualitySys v5r9 in /group/online/dataflow/cmtuser/DataQuality_v5r18"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/lhcb.cern.ch/lib/contrib/CMT/v1r20p20090520; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=DataQualitySys -version=v5r9 -path=/group/online/dataflow/cmtuser/DataQuality_v5r18  -no_cleanup $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

