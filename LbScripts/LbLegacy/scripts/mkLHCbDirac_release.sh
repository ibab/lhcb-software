#!/bin/bash
narg=$#
nar=0
distribute=False

if [ $narg != 2 ] ; then
  echo "Usage : $0 [option] version"
  echo "      : -D to relase Dirac"
  echo "      : -L to release LHCBDirac"
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
          if [ $nar -lt $narg ] ; then
            nar=$(( ++nar ))
          else
            break
          fi
          shift
          ;;
       -L )
          lbpackage=LHCbDirac
          package=LHCbDIRAC
          if [ $nar -lt $narg ] ; then
            nar=$(( ++nar ))
          else
            break
          fi
          shift
          ;;
       -W )
          lbpackage=LHCbWebDirac
          package=LHCbWebDIRAC
          if [ $nar -lt $narg ] ; then
            nar=$(( ++nar ))
          else
            break
          fi
          shift
          ;;
       -t )
          distribute=True
          shift
          ;;
       * )
          version=$1
          if [ $nar -lt $narg ] ; then
            nar=$(( ++nar ))
          else
            break
          fi
          shift
          ;;
   esac
done

echo "We will prepare the release for $package $version"

. /cvmfs/lhcb.cern.ch/lib/LbLogin.sh
. SetupProject.sh LHCbDirac

if [ $package == "DIRAC" ] ; then
  lhcb-import-dirac-release -r $version
else
  dirac-create-svn-tag -p $package -v $version
fi

if [ $distribute == "True" ] ; then
  dirac-distribution -r $version -t server,client -l LHCb -D /afs/cern.ch/lhcb/distribution/LHCbDirac_project
fi

if [ $package != "LHCbWebDirac" ] then
  dirac-create-svn-cmt-tag $lbpackage $version
fi

exit 0