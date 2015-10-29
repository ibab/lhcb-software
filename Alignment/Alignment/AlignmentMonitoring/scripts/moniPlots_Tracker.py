#!/bin/python

from GaudiPython import gbl
import ROOT as r
import os
from AlignmentMonitoring import OnlineUtils, RootUtils
import numpy as np

AlMon = gbl.Alignment.AlignmentMonitoring
def ConfigureMonApp(outputName, pages):
    r.gStyle.SetOptTitle(0)
    monApp = AlMon.MonitoringApplication(outputName)
    for name, hists in pages.iteritems():
        hists_sa = RootUtils.wrapStringArray(hists)
        if len(hists_sa)<10:
            almon = AlMon.MonitoringPage(name, hists_sa)
            for hist in hists:
                configs = Configs[name][hist]
                if(  'title' in configs): almon.setTitle(hist,configs['title'])
                if(   'mean' in configs): almon.setMLine(hist,configs['mean'])
                if( 'vlines' in configs): almon.setVLine(hist,RootUtils.wrapNumericArray(configs['vlines'],'double') )
                if( 'hlines' in configs): almon.setHLine(hist,RootUtils.wrapNumericArray(configs['hlines'],'double') )
                if( 'yrange' in configs): almon.setYRange(hist,configs['yrange'][0], configs['yrange'][1])
                if( 'xrange' in configs): almon.setXRange(hist,configs['xrange'][0], configs['xrange'][1])
                if( 'reference' in configs): almon.setReferenceLine(hist,configs['reference'][0], configs['reference'][1])
                if( 'lines' in configs):
                    tLines = r.std.vector('TLine*')()
                    for x1,y1,x2,y2 in configs['lines']:
                      tLines.push_back( r.TLine(x1,y1,x2,y2) )
                    almon.setLines(hist,tLines)
            monApp.addPage( almon )
        else: print 'Too many histograms per page requested!'
    return monApp

Pages = {
        "D0sAlign":
				      [ 
				      #"Track/AlignD02KPiMonitor/massNegativeY",
				      #"Track/AlignD02KPiMonitor/massPositiveY",
				      "Track/AlignD02KPiMonitor/mass",
				      "Track/AlignD02KPiMonitor/massVersusMom",
				      "Track/AlignD02KPiMonitor/massVersusPhiMatt",
				      "Track/AlignD02KPiMonitor/massVersusPt",
				      "Track/AlignD02KPiMonitor/massVersusEta",
                      ],
        "IT/TT overlap residuals":
				      [
                      "Track/ITTrackMonitor/ASide/Overlap residual",
				      "Track/ITTrackMonitor/CSide/Overlap residual",
				      "Track/ITTrackMonitor/Top/Overlap residual",
				      "Track/ITTrackMonitor/Bottom/Overlap residual",
                      "Track/TTTrackMonitor/TTaX/Overlap residual",
				      "Track/TTTrackMonitor/TTaU/Overlap residual",
				      "Track/TTTrackMonitor/TTbV/Overlap residual",
				      "Track/TTTrackMonitor/TTbX/Overlap residual"
                      ],
        "long track chisquare and Velo(TT)-T match curvature":
				     ["Track/TrackMonitor/Long/chi2ProbVsMom",
  				      "Track/TrackMonitor/Long/chi2ProbVsEta",
				      "Track/TrackMonitor/Long/chi2ProbVsPhi",
				      "Track/TrackFitMatchMonitor/curvatureRatioTToLongVsQoP",
				      "Track/TrackFitMatchMonitor/curvatureRatioVeloTTToLongVsQoP"],
                      
}
mD0 = 1.86484
Configs = {
        "D0sAlign":
				      { 
				      #"Track/AlignD02KPiMonitor/massNegativeY":{'vlines':[1.84,1.88],'mean':True},
				      #"Track/AlignD02KPiMonitor/massPositiveY":{'vlines':[1.84,1.88],'mean':True},
				      "Track/AlignD02KPiMonitor/mass":{'vlines':[mD0-1e-3,mD0+1e-3],'mean':'X', 'reference':['V',mD0]},
				      "Track/AlignD02KPiMonitor/massVersusMom":{'yrange':[mD0-1.5e-3,mD0+1.5e-3],'reference':['H',mD0], 'mean':'Y', 'lines':[(10,mD0-1e-3,200,mD0-1e-3),(10,mD0+1e-3,200,mD0+1e-3)]},
				      "Track/AlignD02KPiMonitor/massVersusPhiMatt":{'yrange':[mD0-1.5e-3,mD0+1.5e-3],'reference':['H',mD0], 'mean':'Y', 'hlines':[mD0-1e-3,mD0+1e-3]},
				      "Track/AlignD02KPiMonitor/massVersusPt":{'yrange':[mD0-1.5e-3,mD0+1.5e-3],'reference':['H',mD0], 'mean':'Y', 'lines':[(0,mD0-1e-3,15,mD0-1e-3),(0,mD0+1e-3,15,mD0+1e-3)]},
				      "Track/AlignD02KPiMonitor/massVersusEta":{'yrange':[mD0-1.5e-3,mD0+1.5e-3],'reference':['H',mD0], 'mean':'Y', 'lines':[(2,mD0-1e-3,5,mD0-1e-3),(2,mD0+1e-3,5,mD0+1e-3)]},
                      },
        "IT/TT overlap residuals":
				      {
                      "Track/ITTrackMonitor/ASide/Overlap residual":{'vlines':[-0.05,0.05],'mean':'X','reference':['V',0]},
				      "Track/ITTrackMonitor/CSide/Overlap residual":{'vlines':[-0.05,0.05],'mean':'X','reference':['V',0]},
				      "Track/ITTrackMonitor/Top/Overlap residual":{'vlines':[-0.05,0.05],'mean':'X','reference':['V',0]},
				      "Track/ITTrackMonitor/Bottom/Overlap residual":{'vlines':[-0.05,0.05],'mean':'X','reference':['V',0]},
                      "Track/TTTrackMonitor/TTaX/Overlap residual":{'vlines':[-0.05,0.05],'mean':'X','reference':['V',0]},
				      "Track/TTTrackMonitor/TTaU/Overlap residual":{'vlines':[-0.05,0.05],'mean':'X','reference':['V',0]},
				      "Track/TTTrackMonitor/TTbV/Overlap residual":{'vlines':[-0.05,0.05],'mean':'X','reference':['V',0]},
				      "Track/TTTrackMonitor/TTbX/Overlap residual":{'vlines':[-0.05,0.05],'mean':'X','reference':['V',0]}
                      },
        "long track chisquare and Velo(TT)-T match curvature":
				     {"Track/TrackMonitor/Long/chi2ProbVsMom":{},
				      "Track/TrackMonitor/Long/chi2ProbVsEta":{},
				      "Track/TrackMonitor/Long/chi2ProbVsPhi":{},
				      "Track/TrackFitMatchMonitor/curvatureRatioTToLongVsQoP":{'yrange':[0.95,1.05],'reference':['H',1], 'mean':'Y', 'lines':[(-0.15,0.975,0.15,0.975),(-0.15,1.025,0.15,1.025)]},
				      "Track/TrackFitMatchMonitor/curvatureRatioVeloTTToLongVsQoP":{'yrange':[0.95,1.05],'reference':['H',1], 'mean':'Y', 'lines':[(-0.15,0.96,0.15,0.96),(-0.15,1.04,0.15,1.04)]}
                    },
                      
}


from AlignmentOutputParser.AlPlotter import *
from AlignmentOutputParser.AlignOutput import *
from matplotlib.backends.backend_pdf import PdfPages
from AlignmentMonitoring.MultiPlot import MultiPlot
r.gROOT.SetBatch(True)
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

    activity = 'Velo'
    if args.Tracker:
        activity = 'Tracker' 

    run = args.run if args.run else OnlineUtils.findLastRun(activity)

    #-->read histograms from the newest .root files
    files_histo = {'old': args.histoFiles[0], 'new': args.histoFiles[1]} if args.histoFiles else OnlineUtils.findHistos(activity,run)
    print files_histo

    outputFile_name = args.outFile if args.outFile else 'MoniPlots.pdf'
    
    c = r.TCanvas('c', 'c')
    c.Print(outputFile_name+'[')
    #-->plot the histograms
    monApp = ConfigureMonApp(outputFile_name, Pages)
    for label, f in files_histo.iteritems(): monApp.addFile(label+':'+f)
    monApp.draw(c, False)

    #-->read alignlog.txt file
    c = r.gPad.GetCanvas()
    r.gPad.SetGrid(0,0)
    alignlog = args.alignlog if args.alignlog else OnlineUtils.findAlignlog(activity,run)
    print alignlog
    aout = AlignOutput(alignlog)
    aout.Parse()
    mps = []
    
    for expression, title in [('TrChi2nDof', 'Track #chi^{2}/dof'), ('NormalisedChi2Change', '#Delta #chi^{2}/dof')]:
        gr = RootUtils.makeGraph(RootUtils.getExpression(aout, expression))
        mps.append(MultiPlot(expression, title = '{0};Iteration;{0}'.format(title), histos = {expression:gr},
                       kind='g', drawLegend = False))
        mps[-1].DrawOption = 'alp'

    #--> to be finished


    c1 = RootUtils.getDrawnCanvas(mps)
    c1.Print(outputFile_name)
        
    c.Print(outputFile_name+']')
    
    ##########################
