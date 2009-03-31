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



set brunelver=v32r5
python install_project.py -p Brunel -v $brunelver -b slc4_ia32_gcc34 
python install_project.py -p Brunel -v $brunelver -b slc4_amd64_gcc34
python install_project.py -p Brunel -v $brunelver -b slc4_ia32_gcc34_dbg
python install_project.py -p Brunel -v $brunelver -b slc4_amd64_gcc34_dbg

setenv PYTHONPATH
SetupProject Brunel $brunelver
cd $TMPDIR/run


rm -rf BenchMark.opts 

cp $BRUNELROOT/options/v200601.opts BenchMark.opts



Benchmark -n 500 -D /castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001889/DIGI/0000 Brunel.exe $TMPDIR/run/BenchMark.opts


# rm -rf $TMPDIR/*
