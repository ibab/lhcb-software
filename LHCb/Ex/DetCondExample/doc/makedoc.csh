#! /bin/csh -f
# $Id: makedoc.csh,v 1.2 2001-10-29 12:43:17 andreav Exp $

#------------------------------
# Make Doxygen documentation
#------------------------------
if ( ! (${?DETCONDEXAMPLE_DOXYGEN} && ${?DETCONDEXAMPLE_DOXYURL}) ) then
  echo Environment variables not set
  echo Source ${DETCONDEXAMPLEROOT}/cmt/setup.csh and try again
  exit
endif

cd ${DETCONDEXAMPLEROOT}/doc

if ( ${?GAUDI_DOXYURL} && ${?GAUDI_DOXYGEN} ) then
else
  setenv GAUDI_DOXY_UNDEFINED 1
  setenv GAUDI_DOXYURL http://cern.ch/gaudi/Doxygen/v8/doc/html
  setenv GAUDI_DOXYGEN /afs/cern.ch/sw/Gaudi/releases/Gaudi/v8/doc/
endif
echo Using dOxygen documentation for the Gaudi framework taken from URL
echo ${GAUDI_DOXYURL}
echo Using dOxygen tag for the Gaudi framework taken from 
echo ${GAUDI_DOXYGEN}  

/bin/rm -f Doxyfooter.html
/bin/rm -f Doxyheader.html
if ( ${?MAILSERV_GEM} ) then
  sed s/USERNAME/${USER}/g DoxyfooterSchema.html | \
  sed s/USERMAIL/${MAILSERV_GEM}/g > Doxyfooter.html
else
  sed s/USERNAME/${USER}/g DoxyfooterSchema.html | \
  sed /USERMAIL/d > Doxyfooter.html
endif
cat DoxyheaderSchema.html \
  | sed "s|CONDDB_DOXYURL|${CONDDB_DOXYURL}|g" \
  | sed "s|DETCOND_DOXYURL|${DETCOND_DOXYURL}|g" \
  | sed "s|GAUDI_DOXYURL|${GAUDI_DOXYURL}|g" \
  > Doxyheader.html

/afs/cern.ch/user/a/andreav/public/doxygen-1.2.11.1/bin/doxygen DoxyFile.cfg
#${GAUDISOFT}/../extsoft/doxygen-1.2.5/bin/doxygen DoxyFile.cfg
cp gaudiSF.gif ${DETCONDEXAMPLE_DOXYGEN}

/bin/rm -f Doxyfooter.html
/bin/rm -f Doxyheader.html

${DETCONDEXAMPLE_DOXYGEN}/installdox \
  -l Gaudi.tag@${GAUDI_DOXYURL} \
  -l ConditionsDB.tag@${CONDDB_DOXYURL} \
  -l DetCond.tag@${DETCOND_DOXYURL}

cp README ${DETCONDEXAMPLE_DOXYGEN}
cp simpleDOC.txt ${DETCONDEXAMPLE_DOXYGEN}
cp -dr ${DETCONDEXAMPLEROOT}/Check \
       ${DETCONDEXAMPLE_DOXYGEN}/DetCondExampleCheck

if ( ${?GAUDI_DOXY_UNDEFINED} ) then
  unsetenv GAUDI_DOXY_UNDEFINED
  unsetenv GAUDI_DOXYURL
  unsetenv GAUDI_DOXYGEN
endif

echo You can now browse the documentation under 
echo ${DETCONDEXAMPLE_DOXYURL}/index.html

