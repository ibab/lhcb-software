# echo "Setting OnlineFileSelector head in /afs/cern.ch/user/i/ichalkia/cmtuser/Online_v5r6/Online"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/cern.ch/sw/contrib/CMT/v1r20p20090520; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=OnlineFileSelector -version=head -path=/afs/cern.ch/user/i/ichalkia/cmtuser/Online_v5r6/Online  -no_cleanup $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

