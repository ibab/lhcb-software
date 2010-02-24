#!/bin/bash
#

narg=$#
nar=0
list_arg=""
feicim_dev=""

while [ $nar -lt $narg ] ; do
  case "$1" in 
    -h )
      echo "Usage : feicim [-h] [--dev] "
      echo "      -h : provide this help"
      echo "      --dev : to use the Feicim version which is DEV"
      exit 0
      ;;
    --dev )
      feicim_dev=" --dev"
      if [ $nar -lt $narg ] ; then
        nar=$(( ++nar ))
      else
        break
      fi
      shift
      ;;
    * )
      echo "unknown option : " $argv[${nar}]
      exit 0
  esac
done


if [ ! -n "$FEICIMSYSROOT" ] ; then
  echo 'The Feicm project setup takes time!'
#  . `which SetupProject.sh` $feicim_dev Feicim >&! /dev/null
  . `which SetupProject.sh` $feicim_dev Feicim
  if [ $SetupProjectStatus != 0 ] ; then
    echo Problem to set the Feicim environment
  else
    `which dirac-proxy-info` --checkvalid > /dev/null
    if [ "$?" != "0" ] ; then
       feicimGenerateProxy
       if [ "$?" != "0" ] ;then
         exit 1
       fi
    fi
  fi
fi

feicimGui

#exit 0
