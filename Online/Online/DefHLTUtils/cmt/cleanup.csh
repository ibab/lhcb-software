if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /sw/lib/contrib/CMT/v1r20p20090520
endif
source ${CMTROOT}/mgr/setup.csh
set tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt cleanup -csh -pack=DefHLTUtils -version=v1r1 -path=/home/beat/cmtuser/myOnline/Online $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

