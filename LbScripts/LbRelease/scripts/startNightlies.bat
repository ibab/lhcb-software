
call SetupProject.bat LCGCMT lcgnightlies

call "c:\Program Files\Microsoft Visual Studio\DF98\bin\DFVARS.BAT"
call "%VS71COMNTOOLS%vsvars32.bat"

call runpy LbRelease.Nightlies.startNightlies %1 %2 %3 %4 %5 %6 %7 %8 %9