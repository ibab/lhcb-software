# echo "Setting Writer v1r46 in /home/frankm/cmtuser/Online_v4r68/Online"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/lhcb.cern.ch/lib/contrib/CMT/v1r20p20090520; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=Writer -version=v1r46 -path=/home/frankm/cmtuser/Online_v4r68/Online  -no_cleanup $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

