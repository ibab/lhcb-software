# $Id: myCleanup.csh,v 1.5 2001-11-23 17:59:22 andreav Exp $
set tempfile=`${CMTROOT}/mgr/cmt build temporary_name -quiet`
if ${status} != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt -quiet cleanup -csh -pack=DetCondExample -version=v2 -path=${HOME}/newmycmt/Ex $* | sed "s/setup/cleanup/g" >${tempfile}; source ${tempfile}; /bin/rm -f ${tempfile}

