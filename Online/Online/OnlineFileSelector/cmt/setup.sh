# echo "Setting OnlineFileSelector v0r1 in /home/ichalkia/cmtuser/Online_v5r6/Online"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/lhcb.cern.ch/lib/contrib/CMT/v1r20p20090520; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=OnlineFileSelector -version=v0r1 -path=/home/ichalkia/cmtuser/Online_v5r6/Online  -no_cleanup $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

