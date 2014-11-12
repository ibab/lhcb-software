# echo "Setting AlignOnline v1r0 in /group/online/dataflow/cmtuser/ALIGNMENTONLINE/ALIGNMENTONLINE_HEAD/AlignmentOnline"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /cvmfs/lhcb.cern.ch/lib/contrib/CMT/v1r20p20090520
endif
source ${CMTROOT}/mgr/setup.csh

set tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=AlignOnline -version=v1r0 -path=/group/online/dataflow/cmtuser/ALIGNMENTONLINE/ALIGNMENTONLINE_HEAD/AlignmentOnline  -no_cleanup $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

