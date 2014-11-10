# echo "Setting AlignOnline v1r0 in /afs/cern.ch/user/m/mamartin/AlignmentOnline"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/sw/contrib/CMT/v1r20p20090520
endif
source ${CMTROOT}/mgr/setup.csh

set tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=AlignOnline -version=v1r0 -path=/afs/cern.ch/user/m/mamartin/AlignmentOnline  -no_cleanup $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

