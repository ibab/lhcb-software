# $Id: myCleanup.csh,v 1.3 2001-11-23 17:10:32 andreav Exp $
set tempfile=`${CMTROOT}/mgr/cmt build temporary_name -quiet`
if ${status} != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt -quiet cleanup -csh -pack=DetCond -version=v2 -path=${HOME}/newmycmt/Det $* | sed "s/setup/cleanup/g" >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

