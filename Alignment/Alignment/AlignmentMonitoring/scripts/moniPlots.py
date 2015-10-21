#!/usr/bin/env python

##########################
###   Options parser   ###
if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description ="Macro to make plots from alignlog")
    parser.add_argument('-r','--run', help='run number, default is the last one')
    parser.add_argument('--alignlog', help='location of alignlog.txt, default is guessed by run number')
    parser.add_argument('--histoFiles', help='location of histograms files, expect two paths, old ad new align histos, default is guessed by run number', nargs=2)
    parser.add_argument('-o','--outFile',help='output file name')
    parser.add_argument('-t','--Tracker', help='Analise Tracker instead of Velo',action='store_true')
    args = parser.parse_args()

##########################

import os, sys, fnmatch, re
import ROOT as r
from GaudiPython import gbl
from AlignmentOutputParser.AlignOutput import *
from MultiPlot import MultiPlot

AlMon = gbl.Alignment.AlignmentMonitoring

r.gROOT.SetBatch(True)

activity = 'Velo'

if args.Tracker:
    activity = 'Tracker'
    
def findLastRun():
    rootDir = '/group/online/AligWork/{0}/'.format(activity)
    ll = sorted([(re.findall('run([0-9]+)', i)[0],  os.stat(os.path.join(rootDir, i)).st_mtime) for i in os.listdir(rootDir)if re.match('run([0-9]+)', i)], key= lambda x: x[-1])
    return ll[-1][0]

    
def findAlignlog(run):
    rootDir = '/group/online/AligWork/{0}/'.format(activity)
    inDir = os.path.join(rootDir, 'run{0}'.format(run))
    alignlog_path = os.path.join(inDir, 'alignlog.txt')
    return alignlog_path


def findHistos(run):
    histDir = '/hist/Savesets/2015/LHCbA/AligWrk_{0}'.format(activity)
    matches = []
    for root, dirs, files in os.walk(histDir):
        for file in files:
            match = re.findall(r'AligWrk_{activity}-{run}(.*?)-(.*?)-EOR.root'.format(activity = activity,**locals()), file)
            if match:
                matches.append(list(match[0])+[os.path.join(root, file)])

    matches = sorted(matches, key=lambda x: x[1], reverse = True)
   
    files_histo = {}
    if matches[0][0] == '01': # only one histogram file
        files_histo['old'] = matches[0][2]
    else:
        for match in matches:
            if match[0] == '01':
                files_histo['old'] = match[2]
                files_histo['new'] = matches[0][2]
                break
    if not len(files_histo): # temporary patch if first run has not 01, use only timestamp info, assumes each fill run only once
        files_histo['old'] = matches[-1][2]
        files_histo['new'] = matches[0][2]
    return files_histo


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
                histo_args['vlines_colors'] = {val : r.kBlue for val in  histo_args['vlines']}
                histo_args['vlines_colors'][0] = r.kGreen+2
                histo_args['vlines_styles'] = {val : 3 for val in  histo_args['vlines']}
                histo_args['vlines_styles'][0] = 5
                histo_args['vlines_width'] = {0 : 2}
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
            mps[-1].Add(histos['old'], 'old', color = r.kBlack, markerStyle=1)
            try:
                mps[-1].Add(histos['new'], 'new', color = r.kRed, lineStyle=7, markerStyle=1)
            except KeyError: pass
            if isinstance(histos['old'], r.TProfile):
                mps[-1].DrawOption = 'nostack'
            else:
                mps[-1].DrawOption = 'nostack hist'
            if wantMeanLines:
                mps[-1].AddLine(histos['old'].GetMean(), 'v', color = r.kBlack, style=1)
                mps[-1].AddLine(histos['new'].GetMean(), 'v', color = r.kRed, style=7)
        c1 = getDrawnCanvas(mps)
        c1.Print(outputFile_name)


def makeGraph(values, errors=None):
    import numpy as np
    x = np.array([float(i) for i in range(len(values))])
    ex = np.array([0 for i in range(len(x))])
    y = np.array([float(i) for i in values])
    if errors:
        ey = np.array([float(i) for i in errors])
    else:
        ey = np.array([0 for i in range(len(x))])
    return r.TGraphErrors(len(x),x, y, ex, ey)


def makeHisto(name, values, title = None, range = [None, None], nBins = 100):
    if range[0] == None: range[0] = min(values)
    if range[1] == None: range[1] = max(values)+1
    if title == None: title = name
    h = r.TH1D(name, title, nBins, *range)
    for i in values:
        h.Fill(i)
    return h


def getDofDeltaConvergence(aout, dof='Tx', alignable='Velo/VeloLeft'):
    '''
    For this to work one must have already done
    aout = AlignOutput(fileName)
    aout.Parse()

    return list values and list errors
    '''
    list_delta = [(j.Delta, j.DeltaErr) for j in sum([i.Alignables[alignable].AlignmentDOFs for i in aout.AlignIterations],[]) if j.Name == dof ]
    if 'T' in dof:
        delta = [(i[0]*1000, i[1]*1000) for i in list_delta] # Translations in um 
    elif 'R' in dof:
        delta = [(i[0]*1000000, i[1]*1000000) for i in list_delta] # Rotations in urad
    return [i[0] for i in delta], [i[1] for i in delta]


def getDofDeltas(aout, dof='Tx', regexes_alignables=['Velo/Velo(Left|Right)/Module..']):
    '''
    For this to work one must have already done
    aout = AlignOutput(fileName)
    aout.Parse()

    return list values of difference after-before for all the alignables which satisfy one of the regexes
    '''
    import re
    deltas = []
    for alignable in aout.AlignIterations[0].Alignables.keys():
        match = [re.match(regex+'$', alignable) for regex in regexes_alignables]
        if match != [None]*len(match): # there is at least a match
            deltas.append(sum([j.Delta for j in sum([i.Alignables[alignable].AlignmentDOFs for i in aout.AlignIterations],[]) if j.Name == dof ]))
    if 'T' in dof:
        deltas = [i*1000 for i in deltas] # Translations in um 
    elif 'R' in dof:
        deltas = [i*1000000 for i in deltas] # Rotations in urad
    return deltas


def getExpression(aout, expression):
    '''
    For this to work one must have already done
    aout = AlignOutput(fileName)
    aout.Parse()

    return list values

    expression in [Chi2, DeltaChi2, DeltaChi2nDofs, DeltaChi2overNDofs, LocalDeltaChi2, MaxModeChi2, NormalisedChi2Change, TrChi2nDof, VChi2nDof]
    '''
    return [getattr(i,expression) for i in aout.AlignIterations]
    


def getDrawnCanvas(drawables):#, title = 'Pollo'):
    
    if len(drawables) == 1:
        canvas_divsions = [1]
    elif len(drawables) == 2:
        canvas_divsions = [2]
    elif len(drawables) in [3, 4]:
        canvas_divsions = [2,2]
    elif len(drawables) in [5, 6]:
        canvas_divsions = [3,2]
    elif len(drawables) in [7, 8, 9]:
        canvas_divsions = [3,3]
        
    c = r.TCanvas()
    c.pads = []
    
    # if title:
    #     import random
    #     title_pad = r.TPad("title_pad"+str(random.random()), "The pad 10% of the height",0.,.9,1.,1.)
    #     title_pad.cd()
    #     titletext = r.TText(0.5,0.5, title) 
    #     titletext.SetNDC() 
    #     titletext.SetTextSize(0.8) 
    #     titletext.SetTextAlign(22) 
    #     titletext.Draw() 
    #     c.cd()
    #     title_pad.Draw()
    #     c.pads.append(title_pad)
    #     pad = r.TPad("pad"+str(random.random()), "The pad 90% of the height",0.,.01,1.,.9)
    #     #pad2.cd()
    #     #framePulls.Draw()
    # else:
    #     pad = r.TPad("pad", "The pad 100% of the height",0.,.01,1.,1.)

    # pad.Divide(*canvas_divsions)
    
    # for i in range(len(drawables)):
    #     pad.cd(i+1)
    #     drawables[i].Draw()

    # c.cd()
    # pad.Draw()
    # c.pads.append(pad)
    # c.Update()
    # c.Print('c1.eps')
    # return c
    
    c.Divide(*canvas_divsions)
    
    for i in range(len(drawables)):
        c.cd(i+1)
        drawables[i].Draw()
        
    c.Update()
    return c


Pages = [("Long track properties and PV position", [ ["Track/TrackMonitor/Velo/3", {'title' : 'track #chi^{2}/ndof'}],
                                                    ["Track/TrackMonitor/Velo/7", {'title' : 'track #eta'}],
                                                    ["Track/TrackMonitor/Velo/8", {'title' : 'track #phi'}],
                                                    "Track/TrackVertexMonitor/PV x position",
                                                    "Track/TrackVertexMonitor/PV y position",
                                                    "Track/TrackVertexMonitor/PV z position",
                                                    ]),
         ("VELO related quantities", [ ["Track/TrackMonitor/Velo/chi2PerDofVelo", {'title' : 'Velo segment #chi^{2}/ndof'}],
                                      "Track/TrackMonitor/Velo/VeloPhiresidualPull",
                                       "Track/TrackMonitor/Velo/VeloRresidualPull",
                                       ["Track/TrackVertexMonitor/PV chisquare per dof", {'title' : 'PV #chi^{2}/ndof'}],
                                       "Velo/VeloTrackMonitor/Pseudoefficiency_per_sensor_vs_sensorID",
                                      ]),
         ("VELO 2-halves alignment", [ ["Track/TrackVertexMonitor/PV left-right delta x", {'vlines' : [0, -.008, .008], 'rangeX' : [-.03, .03]}, True],
                                       ["Track/TrackVertexMonitor/PV left-right delta y", {'vlines' : [0, -.008, .008], 'rangeX' : [-.03, .03]}, True],
                                       ["Track/TrackVertexMonitor/PV left-right delta z", {'vlines' : [0, -.05, .05], 'rangeX' : [-.3, .3]}, True],
                                       ["Track/TrackVeloOverlapMonitor/overlapResidualPhi", {'vlines' : [0,-.03,.03], 'rangeX' : [-.1, .1]}, True],
                                       ["Track/TrackVeloOverlapMonitor/overlapResidualR", {'vlines' : [0,-.03,.03], 'rangeX' : [-.1, .1]}, True],
                                       ]),
         ]
    
   

if __name__ == '__main__':

    run = args.run if args.run else findLastRun()
    alignlog = args.alignlog if args.alignlog else findAlignlog(run)
    files_histo = {'old': args.histoFiles[0], 'new': args.histoFiles[1]} if args.histoFiles else findHistos(run)
    
    # read alignlog
    aout = AlignOutput(alignlog)
    aout.Parse()
    
    c = r.TCanvas('c', 'c')
    outputFile_name = args.outFile if args.outFile else 'MoniPlots.pdf'
    c.Print(outputFile_name+'[')

    # Plots compare before-after
    plotsCompare(Pages, files_histo, outputFile_name)

    mps = []
    
    for expression, title in [('TrChi2nDof', 'Track #chi^{2}/dof'), ('VChi2nDof', 'Vertex #chi^{2}/dof'), ('NormalisedChi2Change', '#Delta #chi^{2}/dof')]:
        gr = makeGraph(getExpression(aout, expression))
        mps.append(MultiPlot(expression, title = '{0};Iteration;{0}'.format(title), histos = {expression:gr},
                       kind='g', drawLegend = False))
        mps[-1].DrawOption = 'alp'

    # 2 halves
    c.cd()
    mp1 = MultiPlot('1', title = 'Convergence Halves;Iteration;Variation [#mum]',kind='g', hlines=[0,-2,2,-4,4],
                    hlines_colors = {-4: r.kGreen+3, 4: r.kGreen+3}, rangeY = [-10, 10])
    mp1.DrawOption = 'alp'
    mp1.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Tx', alignable='Velo/VeloLeft')), 'Tx')
    mp1.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Ty', alignable='Velo/VeloLeft')), 'Ty')
    mp1.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Tz', alignable='Velo/VeloLeft')), 'Tz')

    mp2 = MultiPlot('2', title = 'Convergence Halves;Iteration;Variation [#murad]',kind='g', hlines=[0,-3.5,3.5], rangeY = [-25, 25])
    mp2.DrawOption = 'alp'
    mp2.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Rx', alignable='Velo/VeloLeft')), 'Rx')
    mp2.Add(makeGraph(*getDofDeltaConvergence(aout, dof='Ry', alignable='Velo/VeloLeft')), 'Ry')

    mp3 = MultiPlot('3', title = 'Convergence Halves Rz;Iteration;Variation [#murad]', kind='g', drawLegend = False, hlines=[0,-15,15], rangeY = [-100, 100],
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
    



