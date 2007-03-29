if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/sw/contrib/CMT/v1r20p20070208
endif
source ${CMTROOT}/mgr/setup.csh
set tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt cleanup -csh -pack=Presenter -version=v0r1 -path=/home/psomogyi/cmtuser/Online_v3r1/Online $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

