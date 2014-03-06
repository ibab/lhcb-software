# echo "Setting CC2DDStudies v1r0 in /afs/cern.ch/user/l/landerli/cmtuser/DaVinci_v33r9/Phys/Phys/"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/sw/contrib/CMT/v1r20p20090520
endif
source ${CMTROOT}/mgr/setup.csh

set tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=CC2DDStudies -version=v1r0 -path=/afs/cern.ch/user/l/landerli/cmtuser/DaVinci_v33r9/Phys/Phys/  -no_cleanup $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

