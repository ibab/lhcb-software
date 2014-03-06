# echo "Setting CC2DDStudies v1r0 in /afs/cern.ch/user/l/landerli/cmtuser/DaVinci_v33r9/Phys/Phys/"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/cern.ch/sw/contrib/CMT/v1r20p20090520; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=CC2DDStudies -version=v1r0 -path=/afs/cern.ch/user/l/landerli/cmtuser/DaVinci_v33r9/Phys/Phys/  -no_cleanup $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

