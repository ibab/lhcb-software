# echo "Setting Writer v1r40 in /home/vijay/cmtuser/Online_v4r64/Online"

if test "${CMTROOT}" = ""; then
  CMTROOT=/sw/lib/contrib/CMT/v1r20p20090520; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=Writer -version=v1r40 -path=/home/vijay/cmtuser/Online_v4r64/Online  -no_cleanup $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

