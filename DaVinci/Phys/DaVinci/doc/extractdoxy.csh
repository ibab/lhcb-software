#!/bin/tcsh
 
source ../cmt/setup.csh

set davinciroots = ""
foreach hat ( Phys/ Tools/ L0/ Trg/ Hlt/ ) 
  set hatroots = `env | grep ROOT | grep DAVINCI/DAVINCI_${APPVERSION}/${hat} | grep -v LoKi | sort | uniq | awk -F= '{print $1}'`
  set davinciroots = `echo $davinciroots ; echo $hatroots`
end
set eventroots = `env | grep ROOT | grep Event | sort | uniq | awk -F= '{print $1}'`
# now get the gaudi and lhcb stuff from Brunel
set brunel = `ls -1d /afs/cern.ch/lhcb/software/releases/BRUNEL/* | grep BRUNEL_v | \tail -1`
set gaudiroots = ""
set lhcbbrunelroots = ""
set brunelroots = `grep ROOT $brunel/Rec/Brunel/v*/doc/DoxyFile.cfg | awk -F\( '{print $2}' | awk -F\) '{print $1}'`
touch tmpfile
foreach R ( $brunelroots )
  env | grep $R | grep LHCB | grep -F -e /Phys/ -e /Event/ -e /Kernel/  -e /Associators/ | awk -F= '{print $1}' >> tmpfile
end
set lhcbroots = `cat tmpfile`
rm tmpfile

foreach D (  $davinciroots $lhcbroots $gaudiroots )
  echo "         \044("$D") \"  
end
