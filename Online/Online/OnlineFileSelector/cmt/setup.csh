# echo "Setting OnlineFileSelector head in /afs/cern.ch/user/i/ichalkia/cmtuser/Online_v5r6/Online"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/sw/contrib/CMT/v1r20p20090520
endif
source ${CMTROOT}/mgr/setup.csh

set tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=OnlineFileSelector -version=head -path=/afs/cern.ch/user/i/ichalkia/cmtuser/Online_v5r6/Online  -no_cleanup $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

