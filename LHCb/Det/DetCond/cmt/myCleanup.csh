# $Id: myCleanup.csh,v 1.1 2001-10-29 12:41:51 andreav Exp $
set tempfile=`${CMTROOT}/mgr/cmt build temporary_name`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt -quiet cleanup -csh -pack=DetCond -version=v1 -path=${HOME}/newmycmt/Det $* | sed "s/setup/cleanup/g" >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

