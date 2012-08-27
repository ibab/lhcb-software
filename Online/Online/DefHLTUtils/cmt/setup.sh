# echo "Setting DefHLTUtils v1r1 in /home/beat/cmtuser/myOnline/Online"

if test "${CMTROOT}" = ""; then
  CMTROOT=/sw/lib/contrib/CMT/v1r20p20090520; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=DefHLTUtils -version=v1r1 -path=/home/beat/cmtuser/myOnline/Online  -no_cleanup $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

