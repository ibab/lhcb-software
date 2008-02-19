#!/usr/bin/env tcsh



cd $TMPDIR
mkdir soft run
#source $LHCBSCRIPTS/setup_external.csh --lcg 54 Python
cd soft
wget http://lhcbproject.web.cern.ch/lhcbproject/dist/install_project.py
setenv MYSITEROOT `pwd`
set gaussver=v30r5
python install_project.py -p Gauss -v $gaussver -b sl4_ia32_gcc34
python install_project.py -p Gauss -v $gaussver -b sl4_amd64_gcc34

source scripts/ExtCMT.csh
setenv CMTPROJECTPATH $LHCBPROJECTPATH
unsetenv CMTPATH
SetupProject Gauss $gaussver
cd $TMPDIR/run
cat > BenchMark.opts << EOF
#include "\$GAUSSROOT/options/v200601.opts"

#include "\$DECFILESROOT/options/30000000.opts"  // Min Bias
ApplicationMgr.EvtMax     = 3 ;

//ApplicationMgr.OutStream = [] ;
//ApplicationMgr.HistogramPersistency = "NONE" ;
// PoolDbCacheSvc.Catalog = [] ;

EOF

Gauss.exe BenchMark.opts

unsetenv LD_LIBRARY_PATH
unsetenv PYTHONPATH

# rm -rf $TMPDIR/*
