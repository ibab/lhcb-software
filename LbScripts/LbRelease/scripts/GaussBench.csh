#!/usr/bin/env tcsh



cd $TMPDIR
mkdir soft run
cd soft
rm -rf install_project.py
wget http://lhcbproject.web.cern.ch/lhcbproject/dist/install_project.py
setenv MYSITEROOT `pwd`
python install_project.py -p Gaudi -v v19r7 -b slc4_ia32_gcc34
python install_project.py -p Gaudi -v v19r7 -b slc4_amd64_gcc34
source scripts/ExtCMT.csh
setenv PYTHONPATH ${LHCBPYTHON}:${PYTHONPATH}
setenv CMTPROJECTPATH $LHCBPROJECTPATH
unsetenv CMTPATH
source $LHCBSCRIPTS/setup_external.csh --lcg 54 Python



set gaussver=v30r5
python install_project.py -p Gauss -v $gaussver -b slc4_ia32_gcc34 -m do_config
python install_project.py -p Gauss -v $gaussver -b slc4_amd64_gcc34 -m do_config
python install_project.py -p Gauss -v $gaussver -b slc4_ia32_gcc34_dbg -m do_config
python install_project.py -p Gauss -v $gaussver -b slc4_amd64_gcc34_dbg -m do_config

setenv PYTHONPATH
SetupProject Gauss $gaussver
cd $TMPDIR/run
rm -rf BenchMark.opts 
cat > BenchMark.opts << EOF
#include "\$GAUSSROOT/options/v200601.opts"

#include "\$DECFILESROOT/options/30000000.opts"  // Min Bias
ApplicationMgr.EvtMax     = 3 ;

//ApplicationMgr.OutStream = [] ;
//ApplicationMgr.HistogramPersistency = "NONE" ;
// PoolDbCacheSvc.Catalog = [] ;

EOF




# Gauss.exe BenchMark.opts


#Benchmark -n 1000 -t 1 Gauss.exe $TMPDIR/run/BenchMark.opts
#Benchmark -n 1000 -t 2 Gauss.exe $TMPDIR/run/BenchMark.opts
#Benchmark -n 1000 -t 3 Gauss.exe $TMPDIR/run/BenchMark.opts
#Benchmark -n 1000 -t 4 Gauss.exe $TMPDIR/run/BenchMark.opts
#Benchmark -n 1000 -t 5 Gauss.exe $TMPDIR/run/BenchMark.opts
#Benchmark -n 1000 -t 6 Gauss.exe $TMPDIR/run/BenchMark.opts
#Benchmark -n 1000 -t 7 Gauss.exe $TMPDIR/run/BenchMark.opts
Benchmark -n 500 Gauss.exe $TMPDIR/run/BenchMark.opts
#Benchmark -n 1000 -t 9 Gauss.exe $TMPDIR/run/BenchMark.opts


# rm -rf $TMPDIR/*
