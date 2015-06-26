# echo "Setting Production v1r0 in /afs/cern.ch/user/l/landerli/cmtuser/DaVinci_v36r7p2/PIDCalib/"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/sw/contrib/CMT/v1r20p20090520
endif
source ${CMTROOT}/mgr/setup.csh

set tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=Production -version=v1r0 -path=/afs/cern.ch/user/l/landerli/cmtuser/DaVinci_v36r7p2/PIDCalib/  -no_cleanup $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

