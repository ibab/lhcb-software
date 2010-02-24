#!/bin/csh -f
#
set narg = $#argv
set nar = 1
set list_arg = ""
set feicim_dev = ""

#if ($narg != 0) then
while ($nar <= $narg)
  if ("$argv[${nar}]" == "-h") then
     echo "Usage : feicim [-h] [--dev] "
     echo "      -h : provide this help"
     echo "      --dev : to use the Feicim version which is DEV"
     exit 0
  else if ("$argv[${nar}]" == "--dev") then
     set feicim_dev = " --dev"
     if ($nar < $narg) then
       @ nar = ${nar} + 1
     else
       break
     endif
  else
     echo "unknown option : " $argv[${nar}]
     exit 0
  endif
end


if ! $?FEICIMSYSROOT then
  echo 'The Feicm project setup takes time!'
  source `which SetupProject.csh` $feicim_dev Feicim >&! /dev/null

  if $SetupProjectStatus != 0 then
    echo Problem to set the Feicim environment
   else
    `which dirac-proxy-info` --checkvalid > /dev/null
    if $status != 0 then
       feicimGenerateProxy
       if $status != 0 then
         exit 1
       endif
    endif
  endif
 endif

feicimGui

#exit 0
