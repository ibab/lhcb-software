# echo "Setting DecayTreeTupleHerschel v1r0 in /afs/cern.ch/user/j/johndan/cmtuser/Analysis_v14r0/Phys/"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/sw/contrib/CMT/v1r20p20090520
endif
source ${CMTROOT}/mgr/setup.csh

set tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=DecayTreeTupleHerschel -version=v1r0 -path=/afs/cern.ch/user/j/johndan/cmtuser/Analysis_v14r0/Phys/  -no_cleanup $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

