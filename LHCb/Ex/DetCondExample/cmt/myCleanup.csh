# $Id: myCleanup.csh,v 1.2 2001-09-14 16:53:13 andreav Exp $
set tempfile=/tmp/setup$$
${CMTROOT}/mgr/cmt -quiet cleanup -csh -pack=DetCondExample -version=v1 -path=/afs/cern.ch/user/a/andreav/newmycmt $* | sed "s/setup/cleanup/g" >$tempfile; source $tempfile; /bin/rm -f $tempfile

