rem $Id: RunAllTests.bat,v 1.2 2002-12-03 17:16:42 andreav Exp $
echo off
if "%DETCONDEXAMPLEROOT%." == "." (
  echo ERROR! Must 'setup' before running this script"
) else (
  cd %DETCONDEXAMPLEROOT%\options
  rem # YOU MUST SET DETCONDEXAMPLEROOT WITH DOUBLE BACKSLASHES HERE!
  rem # Under WIN32 you must modify CONDDB_implementation in cmt/requirements!
  mkdir ..\job\NEWlog.%CONDDB_implementation%
  del ..\job\NEWlog.%CONDDB_implementation%\log.*.%CMTCONFIG%
  run DetCondExample testXml.opts | sed "s|I:\\\\newmycmt11\\\\Ex\\\\DetCondExample\\\\v4r2\\\\|DETCONDEXAMPLEROOT/|" > ..\job\NEWlog.%CONDDB_implementation%\log.testXml.%CMTCONFIG%
..\cmt\cleanupDB.bat
  run DetCondExample fillCondDB.opts | sed "s|I:\\\\newmycmt11\\\\Ex\\\\DetCondExample\\\\v4r2\\\\|DETCONDEXAMPLEROOT/|" > ..\job\NEWlog.%CONDDB_implementation%\log.fillCondDB_1.%CMTCONFIG%
  run DetCondExample fillCondDB.opts | sed "s|I:\\\\newmycmt11\\\\Ex\\\\DetCondExample\\\\v4r2\\\\|DETCONDEXAMPLEROOT/|" > ..\job\NEWlog.%CONDDB_implementation%\log.fillCondDB_2.%CMTCONFIG%
  run DetCondExample testCondDB.opts | sed "s|I:\\\\newmycmt11\\\\Ex\\\\DetCondExample\\\\v4r2\\\\|DETCONDEXAMPLEROOT/|" > ..\job\NEWlog.%CONDDB_implementation%\log.testCondDB.%CMTCONFIG%
  run DetCondExample daqCondDB.opts | sed "s|I:\\\\newmycmt11\\\\Ex\\\\DetCondExample\\\\v4r2\\\\|DETCONDEXAMPLEROOT/|" > ..\job\NEWlog.%CONDDB_implementation%\log.daqCondDB.%CMTCONFIG% 
)


