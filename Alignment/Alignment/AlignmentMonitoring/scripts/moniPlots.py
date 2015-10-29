#!/usr/bin/env python

##########################
###   Options parser   ###
if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description ="Macro to make plots to monitor automatic VELO alignment")
    parser.add_argument('-r','--run', help='run number, default is the last one')
    parser.add_argument('--alignlog', help='location of alignlog.txt, default is guessed by run number')
    parser.add_argument('--histoFiles', help='location of histograms files, expect two paths, old ad new align histos, default is guessed by run number', nargs=2)
    parser.add_argument('-o','--outFile',help='output file name')
    args = parser.parse_args()

##########################

import os, sys, fnmatch, re
import ROOT as r
from GaudiPython import gbl
from AlignmentOutputParser.AlignOutput import *
from AlignmentMonitoring.MultiPlot import MultiPlot
from AlignmentMonitoring.OnlineUtils import findLastRun, findAlignlog, findHistos
from AlignmentMonitoring.RootUtils import getExpression, getDofDeltas, getDofDeltaConvergence, makeGraph, makeHisto, getDrawnCanvas

AlMon = gbl.Alignment.AlignmentMonitoring

r.gROOT.SetBatch(True)

activity = 'Velo'


def plotsCompare(Pages, files_histo, outputFile_name, normalize = True):
    inFiles = {key: r.TFile(path) for key, path in files_histo.items()}
    for title, paths in Pages:
        mps = []
        for cont, _path in enumerate(paths):
            histo_args = {}
            wantMeanLines = False
            if type(_path) == str:
                path = _path
            else:
                try:
                    path = _path[0]
                    histo_args = _path[1]
                    wantMeanLines = _path[2]
                except IndexError:
                    pass
            if histo_args.has_key('vlines'):
                histo_args['vlines_colors'] = {val : r.kBlack for val in  histo_args['vlines']}
                histo_args['vlines_colors'][0] = r.kGreen+2
                histo_args['vlines_styles'] = {val : 3 for val in  histo_args['vlines']}
                histo_args['vlines_styles'][0] = 5
                histo_args['vlines_width'] = {val : 2 for val in  histo_args['vlines']}#{0 : 2}
            drawLegend = (cont==0)
            histos = {key: inFiles[key].Get(path) for key in inFiles}
            if not histo_args.has_key('title'):
                histo_args['title'] = histos['old'].GetTitle()
            isProfile = isinstance(histos['old'], r.TProfile)
            if normalize:
                for histo in histos.values():
                    if not isProfile:
                        histo.Scale(1./histo.GetEntries())           
            kind = 'p' if isProfile else 'h'
            mps.append(MultiPlot(path, kind = kind, drawLegend=drawLegend, **histo_args))
            mps[-1].Add(histos['old'], 'old', color = r.kRed, markerStyle=1)
            try:
                mps[-1].Add(histos['new'], 'new', color = r.kBlue, lineStyle=2, markerStyle=1)
            except KeyError: pass
            if isinstance(histos['old'], r.TProfile):
                mps[-1].DrawOption = 'nostack'
            else:
                mps[-1].DrawOption = 'nostack hist'
            if wantMeanLines:
                mps[-1].AddLine(histos['old'].GetMean(), 'v', color = r.kRed, style=1)
                mps[-1].AddLine(histos['new'].GetMean(), 'v', color = r.kBlue, style=2)
        c1 = getDrawnCanvas(mps)
        c1.Print(outputFile_name)



Pages = [("Long track properties and PV position", [ ["Track/TrackMonitor/Velo/3", {'title' : '(1) track #chi^{2}/ndof'}],
                                                    ["Track/TrackMonitor/Velo/7", {'title' : '(2) track #eta'}],
                                                    ["Track/TrackMonitor/Velo/8", {'title' : '(3) track #phi'}],
                                                    ["Track/TrackVertexMonitor/PV x position", {'title' : '(4) PV x position'}],
                                                    ["Track/TrackVertexMonitor/PV y position", {'title' : '(5) PV y position'}],
                                                    ["Track/TrackVertexMonitor/PV z position", {'title' : '(6) PV z position'}],
                                                    ]),
         ("VELO related quantities", [ ["Track/TrackMonitor/Velo/chi2PerDofVelo", {'title' : '(7) Velo segment #chi^{2}/ndof'}],
                                      ["Track/TrackMonitor/Velo/VeloPhiresidualPull", {'title' : '(8) Velo #phi residual pull'}],
                                       ["Track/TrackMonitor/Velo/VeloRresidualPull", {'title' : '(9) Velo R residual pull'}],
                                       ["Track/TrackVertexMonitor/PV chisquare per dof", {'title' : '(10) PV #chi^{2}/ndof'}],
                                       ["Velo/VeloTrackMonitor/Pseudoefficiency_per_sensor_vs_sensorID", {'title' : '(11) Pseudoefficiency vs sensorID'}],
                                      ]),
         ("VELO 2-halves alignment", [ ["Track/TrackVertexMonitor/PV left-right delta x", {'title' : '(12) PV left-right delta x', 'vlines' : [0, -.008, .008], 'rangeX' : [-.04, .04]}, True],
                                       ["Track/TrackVertexMonitor/PV left-right delta y", {'title' : '(13) PV left-right delta y', 'vlines' : [0, -.008, .008], 'rangeX' : [-.04, .04]}, True],
                                       ["Track/TrackVertexMonitor/PV left-right delta z", {'title' : '(14) PV left-right delta z', 'vlines' : [0, -.05, .05], 'rangeX' : [-.3, .3]}, True],
                                       ["Track/TrackVeloOverlapMonitor/overlapResidualPhi", {'title' : '(15) Overlap residuals #phi-sensors', 'vlines' : [0,-.03,.03], 'rangeX' : [-.1, .1]}, True],
                                       ["Track/TrackVeloOverlapMonitor/overlapResidualR", {'title' : '(16) overlap residual R-sensors', 'vlines' : [0,-.03,.03], 'rangeX' : [-.1, .1]}, True],
                                       ]),
         ]
    
   

if __name__ == '__main__':

    if not (args.alignlog and args.histoFiles):
        run = args.run if args.run else findLastRun(activity)
    alignlog = args.alignlog if args.alignlog else findAlignlog(activity, run)
    files_histo = {'old': args.histoFiles[0], 'new': args.histoFiles[1]} if args.histoFiles else findHistos(activity, run)
    
    # read alignlog
    aout = AlignOutput(alignlog)
    aout.Parse()
    
    c = r.TCanvas('c', 'c')
    outputFile_name = args.outFile if args.outFile else 'MoniPlots.pdf'
    c.Print(outputFile_name+'[')

    # Plots compare before-after
    plotsCompare(Pages, files_histo, outputFile_name)

    mps = []
    
    for expression, title in [('TrChi2nDof', '(17) Track #chi^{2}/dof'), ('VChi2nDof', '(18) Vertex #chi^{2}/dof'), ('NormalisedChi2Change', '(19) #Delta #chi^{2}/dof')]:
        gr = makeGraph(getExpression(aout, expression))
        mps.append(MultiPlot(expression, title = '{0};Iteration;{0}'.format(title), histos = {expression:gr},
                       kind='g', drawLegend = False))
        mps[-1].DrawOption = 'alp'

    # 2 halves
    c.cd()
    mp1 = MultiPlot('1', title = '(20) Convergence Halves;Iteration;Variation [#mum]',kind='g', hlines=[0,-2,2,-4,4],
                    hlines_colors = {-4: r.kGreen+3, 4: r.kGreen+3}, rangeY = [-10, 10])
    mp1.DrawOption = 'alp'
    mp1.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Tx', alignable='Velo/VeloLeft')), 'Tx')
    mp1.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Ty', alignable='Velo/VeloLeft')), 'Ty')
    mp1.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Tz', alignable='Velo/VeloLeft')), 'Tz')

    mp2 = MultiPlot('2', title = '(21) Convergence Halves;Iteration;Variation [#murad]',kind='g', hlines=[0,-3.5,3.5], rangeY = [-25, 25])
    mp2.DrawOption = 'alp'
    mp2.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Rx', alignable='Velo/VeloLeft')), 'Rx')
    mp2.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Ry', alignable='Velo/VeloLeft')), 'Ry')

    mp3 = MultiPlot('3', title = '(22) Convergence Halves Rz;Iteration;Variation [#murad]', kind='g', drawLegend = False, hlines=[0,-15,15], rangeY = [-100, 100],
                    histos = {'Rz1': makeGraph(*getDofDeltaConvergence(aout, dof='Rz', alignable='Velo/VeloLeft'))},
                      )
    mp3.DrawOption = 'alp'

    # c1 = getDrawnCanvas([mp1, mp2, mp3])
    mps += [mp1, mp2, mp3]
    c1 = getDrawnCanvas(mps)
    c1.Print(outputFile_name)
    

    # # Modules
    # c.cd()
    # mp1 = MultiPlot('1', title = 'Convergence Modules;Iteration;Variation [#mum]',kind='g', hlines=[0,-2,2], rangeY = [-10, 10])
    # mp1.DrawOption = 'alp'
    # mp1.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Tx', alignable='Velo/VeloLeft/Module00')), '00-Tx',  style =1)
    # mp1.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Ty', alignable='Velo/VeloLeft/Module00')), '00-Ty', style =-1)
    # mp1.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Tx', alignable='Velo/VeloLeft/Module12')), '12',  style =2)
    # mp1.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Ty', alignable='Velo/VeloLeft/Module12')), style =-2)
    # mp1.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Tx', alignable='Velo/VeloLeft/Module32')), '32',  style =3)
    # mp1.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Ty', alignable='Velo/VeloLeft/Module32')), style =-3)
    # mp1.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Tx', alignable='Velo/VeloLeft/Module38')), '38',  style =4)
    # mp1.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Ty', alignable='Velo/VeloLeft/Module38')), style =-4)

    # mp2 = MultiPlot('2', title = 'Convergence Modules Rz;Iteration;Variation [#murad]', kind='g', hlines=[0,-100,100], rangeY = [-300, 300])
    # mp2.DrawOption = 'alp'
    # mp2.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Rz', alignable='Velo/VeloLeft/Module00')), '00', style = 1)
    # mp2.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Rz', alignable='Velo/VeloLeft/Module12')), '12', style = 2)
    # mp2.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Rz', alignable='Velo/VeloLeft/Module32')), '32', style = 3)
    # mp2.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Rz', alignable='Velo/VeloLeft/Module38')), '38', style = 4)

    # mp3 = MultiPlot('3', title = 'Convergence Modules;Iteration;Variation [#mum]',kind='g', hlines=[0,-2,2], rangeY = [-10, 10])
    # mp3.DrawOption = 'alp'
    # mp3.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Tx', alignable='Velo/VeloRight/Module01')), '01-Tx',  style =1)
    # mp3.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Ty', alignable='Velo/VeloRight/Module01')), '01-Ty', style =-1)
    # mp3.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Tx', alignable='Velo/VeloRight/Module13')), '13',  style =2)
    # mp3.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Ty', alignable='Velo/VeloRight/Module13')), style =-2)
    # mp3.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Tx', alignable='Velo/VeloRight/Module33')), '33',  style =3)
    # mp3.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Ty', alignable='Velo/VeloRight/Module33')), style =-3)
    # mp3.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Tx', alignable='Velo/VeloRight/Module39')), '39',  style =4)
    # mp3.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Ty', alignable='Velo/VeloRight/Module39')), style =-4)

    # mp4 = MultiPlot('4', title = 'Convergence Modules Rz;Iteration;Variation [#murad]', kind='g', hlines=[0,-100,100], rangeY = [-300, 300])
    # mp4.DrawOption = 'alp'
    # mp4.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Rz', alignable='Velo/VeloRight/Module01')), '01', style = 1)
    # mp4.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Rz', alignable='Velo/VeloRight/Module13')), '13', style = 2)
    # mp4.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Rz', alignable='Velo/VeloRight/Module33')), '33', style = 3)
    # mp4.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Rz', alignable='Velo/VeloRight/Module39')), '39', style = 4)

    
    # # Modules histo
    # mp5 = MultiPlot('5', title = 'Variation All Modules;Variation [#mum];',kind='h', vlines=[-2,2])
    # mp5.Add(makeHisto('Tx5',getDofDeltas(aout,dof='Tx',regexes_alignables=['Velo/Velo(Left|Right)/Module..']),range=[-10,10], nBins=30),'Tx')
    # mp5.Add(makeHisto('Ty5',getDofDeltas(aout,dof='Ty',regexes_alignables=['Velo/Velo(Left|Right)/Module..']),range=[-10,10], nBins=30),'Ty')

    # mp6 = MultiPlot('6', title = 'Variation All Modules Rz;Variation [#murad];',kind='h', vlines=[-100,100], drawLegend=False)
    # mp6.Add(makeHisto('Rz6',getDofDeltas(aout,dof='Rz',regexes_alignables=['Velo/Velo(Left|Right)/Module..']),range=[-300,300], nBins=30))

    # c1 = getDrawnCanvas([mp1, mp3, mp5, mp2, mp4, mp6])
    # c1.Print(outputFile_name)
        
    c.Print(outputFile_name+']')
    



