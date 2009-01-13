
call SetupProject.bat LCGCMT lcgnightlies

python -m LbRelease.Nightlies.startNightlies ${*:q} %1 %2 %3 %4 %5 %6 %7 %8 %9

