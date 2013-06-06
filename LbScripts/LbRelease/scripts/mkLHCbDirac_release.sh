#!/bin/bash
#
# $HeadURL$
# __RCSID__ = $Id$
narg=$#
nar=0
distribute=False

if [ $narg -lt 2 ] ; then
  echo "Usage : $0 [option] version"
  echo "      : -D to release Dirac"
  echo "      : -M to release VMDirac"
  echo "      : -L to release LHCBDirac"
  echo "      : -V to release LHCbVMDirac"
  echo "      : -W to release LHCbWebDirac"
  echo "      : -t to create the tar ball"
  echo "      : version to be release"
  exit 0
fi

while [ $nar -lt $narg ] ; do
  case "$1" in
       -D )
          lbpackage=Dirac
          package=DIRAC
          shift
          version=$1
          shift
          ;;
       -M )
          lbpackage=VMDirac
          package=VMDIRAC
          shift
          version=$1
          shift
          ;;
       -L )
          lbpackage=LHCbDirac
          package=LHCbDIRAC
          shift
          version=$1
          shift
          ;;
       -V )
          lbpackage=LHCbVMDirac
          package=LHCbVMDIRAC
          shift
          version=$1
          shift
          ;;
       -W )
          lbpackage=LHCbWebDirac
          package=LHCbWebDIRAC
          shift
          version=$1
          shift
          ;;
       -t )
          distribute=True
          shift
          ;;
       * )
          shift
          ;;
  esac
  if [ $nar -lt $narg ] ; then
    nar=$(( ++nar ))
  else
    break
  fi
done

echo "We will prepare the release for $package $version"

. /cvmfs/lhcb.cern.ch/lib/LbLogin.sh
. SetupProject.sh LHCbDirac

if [ $package == "DIRAC" ] ; then
  lhcb-import-dirac-release -r $version
else if [ $package == "VMDIRAC" ] ; then
  lhcb-import-vmdirac-release -r $version
else
  dirac-create-svn-tag -p $package -v $version
fi

if [ $distribute == "True" ] ; then
  dirac-distribution -r $version -t server,client -l LHCb -D /afs/cern.ch/lhcb/distribution/LHCbDirac_project
fi

if [ $package != "LHCbWebDirac" ] ; then
  dirac-create-svn-cmt-tag $lbpackage $version
fi

exit 0