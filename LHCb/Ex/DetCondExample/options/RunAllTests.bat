rem $Id: RunAllTests.bat,v 1.1 2002-12-02 11:37:05 andreav Exp $
echo off
if "%DETCONDEXAMPLEROOT%." == "." (
  echo ERROR! Must 'setup' before running this script"
) else (
  cd %DETCONDEXAMPLEROOT%\options
  ..\sql\dropCondDB.bat
  # YOU MUST SET YOUR VALUE OF DETCONDEXAMPLEROOT WITH DOUBLE BACKSLASHES HERE!
  run DetCondExample testXml.opts | sed "s|I:\\\\newmycmt11\\\\Ex\\\\DetCondExample\\\\v4r1\\\\|DETCONDEXAMPLEROOT/|" > log.testXml.%CMTCONFIG%
..\cmt\cleanupDB.bat
  run DetCondExample fillCondDB.opts | sed "s|I:\\\\newmycmt11\\\\Ex\\\\DetCondExample\\\\v4r1\\\\|DETCONDEXAMPLEROOT/|" > log.fillCondDB_1.%CMTCONFIG%
  run DetCondExample fillCondDB.opts | sed "s|I:\\\\newmycmt11\\\\Ex\\\\DetCondExample\\\\v4r1\\\\|DETCONDEXAMPLEROOT/|" > log.fillCondDB_2.%CMTCONFIG%
  run DetCondExample testCondDB.opts | sed "s|I:\\\\newmycmt11\\\\Ex\\\\DetCondExample\\\\v4r1\\\\|DETCONDEXAMPLEROOT/|" > log.testCondDB.%CMTCONFIG%
  run DetCondExample daqCondDB.opts | sed "s|I:\\\\newmycmt11\\\\Ex\\\\DetCondExample\\\\v4r1\\\\|DETCONDEXAMPLEROOT/|" > log.daqCondDB.%CMTCONFIG% 
)


