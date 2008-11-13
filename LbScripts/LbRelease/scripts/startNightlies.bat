
call SetupProject.bat LCGCMT lcgnightlies

python %~d0\%~p0\startNightlies.py ${*:q} %1 %2 %3 %4 %5 %6 %7 %8 %9

