# $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/cmt/myCleanup.csh,v 1.1.1.1 2001-09-14 15:45:03 andreav Exp $
set tempfile=/tmp/setup$$
${CMTROOT}/mgr/cmt -quiet cleanup -csh -pack=DetCondExample -version=v3r03 -path=/afs/cern.ch/user/a/andreav/newmycmt $* | sed "s/setup/cleanup/g" >$tempfile; source $tempfile; /bin/rm -f $tempfile

