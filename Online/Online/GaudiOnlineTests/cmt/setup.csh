# echo "Setting tests Close_to_Hell in /afs/cern.ch/work/f/frankb/cmtuser/ONLINE/ONLINE_HEAD/Online/GaudiOnline"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/sw/contrib/CMT/v1r20p20090520
endif
source ${CMTROOT}/mgr/setup.csh

set tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=tests -version=Close_to_Hell -path=/afs/cern.ch/work/f/frankb/cmtuser/ONLINE/ONLINE_HEAD/Online/GaudiOnline  -no_cleanup $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

