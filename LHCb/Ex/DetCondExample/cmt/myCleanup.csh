# $Id: myCleanup.csh,v 1.4 2001-10-29 16:06:27 andreav Exp $
set tempfile=`${CMTROOT}/mgr/cmt build temporary_name -quiet`
if ${status} != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt -quiet cleanup -csh -pack=DetCondExample -version=v1 -path=${HOME}/newmycmt/Ex $* | sed "s/setup/cleanup/g" >${tempfile}; source ${tempfile}; /bin/rm -f ${tempfile}

