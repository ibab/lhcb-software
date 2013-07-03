# echo "Setting Writer v1r47 in /afs/cern.ch/work/f/frankb/cmtuser/ONLINE/ONLINE_HEAD/Online"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/cern.ch/sw/contrib/CMT/v1r20p20090520; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=Writer -version=v1r47 -path=/afs/cern.ch/work/f/frankb/cmtuser/ONLINE/ONLINE_HEAD/Online  -no_cleanup $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

