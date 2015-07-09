if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/sw/contrib/CMT/v1r20p20090520
endif
source ${CMTROOT}/mgr/setup.csh
set tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt cleanup -csh -pack=AlignmentMonitoring -version=v0r1 -path=/afs/cern.ch/user/m/mamartin/cmtuser/Alignment_v10r4/Alignment $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

