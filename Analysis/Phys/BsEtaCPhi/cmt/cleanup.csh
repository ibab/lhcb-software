if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /cvmfs/lhcb.cern.ch/lib/contrib/CMT/v1r20p20090520
endif
source ${CMTROOT}/mgr/setup.csh
set tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt cleanup -csh -pack=BsEtaCPhi -version=v1r0 -path=/lhcb/users/akar/cmtuser/Urania_v3r0/Phys $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

