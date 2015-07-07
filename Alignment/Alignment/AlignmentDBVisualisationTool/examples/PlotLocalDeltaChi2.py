import os, sys
from AlignmentOutputParser.AlPlotter import *
from AlignmentOutputParser.AlignOutput import *

fileName = '/afs/cern.ch/work/m/mamartin/Alignment2015/outputs/run154182_startfromITsurvey2/alignlog.txt'
aout = AlignOutput(fileName)
aout.Parse()
alIters = aout.AlignIterations
apl = AlPlotter(alIters)
apl.PlotLocalDeltaChi2()
