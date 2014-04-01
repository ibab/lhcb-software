# echo "Setting DecayTreeTupleANNPID v1r0 in /usera/jonesc/cmtuser/DaVinci_v34r0/Phys"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /cvmfs/lhcb.cern.ch/lib/contrib/CMT/v1r20p20090520
endif
source ${CMTROOT}/mgr/setup.csh

set tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=DecayTreeTupleANNPID -version=v1r0 -path=/usera/jonesc/cmtuser/DaVinci_v34r0/Phys  -no_cleanup $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

