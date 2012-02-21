# echo "Setting LifetimeAcceptance v1r0 in /afs/cern.ch/user/y/yamhis/Phys"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/cern.ch/sw/contrib/CMT/v1r20p20090520; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=LifetimeAcceptance -version=v1r0 -path=/afs/cern.ch/user/y/yamhis/Phys  -no_cleanup $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

