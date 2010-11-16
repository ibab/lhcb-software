if test "${CMTROOT}" = ""; then
  CMTROOT=/sw/lib/contrib/CMT/v1r20p20070208; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt cleanup -sh -pack=tests -version= -path=/home/dsonnick/cmtuser/Online_v4r20/Online/Writer $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

