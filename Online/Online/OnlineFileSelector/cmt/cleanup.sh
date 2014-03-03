if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/lhcb.cern.ch/lib/contrib/CMT/v1r20p20090520; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt cleanup -sh -pack=OnlineFileSelector -version=v0r1 -path=/home/ichalkia/cmtuser/Online_v5r6/Online $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

