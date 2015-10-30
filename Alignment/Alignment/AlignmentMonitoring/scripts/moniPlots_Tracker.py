#!/usr/bin/env python

from GaudiPython import gbl
import ROOT as r
import os
from AlignmentMonitoring import OnlineUtils, RootUtils
import numpy as np

AlMon = gbl.Alignment.AlignmentMonitoring
def ConfigureMonApp(outputName, pages):
    r.gStyle.SetOptTitle(1)
    monApp = AlMon.MonitoringApplication(outputName)
    #for name, hists in pages.iteritems():
    for key in sorted(pages.iterkeys()):
        name = pages[key]['name']
        hists = pages[key]['hists']
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

Pages = {1: { 'name':"long track chisquare and Velo(TT)-T match curvature",
              'hists': [
                    "Track/TrackMonitor/Long/chi2ProbVsMom",
                    "Track/TrackMonitor/Long/chi2ProbVsEta",
                    "Track/TrackMonitor/Long/chi2ProbVsPhi",
                    "Track/TrackMonitor/Long/chi2ProbMatchVsMom",
                    "Track/TrackMonitor/Long/chi2ProbDownstreamVsMom",
                    "Track/TrackMonitor/Long/chi2ProbVeloVsMom",
                    "Track/TrackFitMatchMonitor/curvatureRatioTToLongVsQoP",
                    "Track/TrackFitMatchMonitor/curvatureRatioVeloTTToLongVsQoP"]},
        2: { 'name': "D0 Candidates",
			 'hists':[
				      #"Track/AlignD02KPiMonitor/massNegativeY",
				      #"Track/AlignD02KPiMonitor/massPositiveY",
				      "Track/AlignD02KPiMonitor/mass",
				      "Track/AlignD02KPiMonitor/massVersusMom",
				      "Track/AlignD02KPiMonitor/massVersusPhiMatt",
				      "Track/AlignD02KPiMonitor/massVersusPt",
				      "Track/AlignD02KPiMonitor/massVersusEta",
                      ]},
        3: { 'name':"IT/TT overlap residuals",
			 'hists':[
                      "Track/ITTrackMonitor/ASide/Overlap residual",
				      "Track/ITTrackMonitor/CSide/Overlap residual",
				      "Track/ITTrackMonitor/Top/Overlap residual",
				      "Track/ITTrackMonitor/Bottom/Overlap residual",
                      "Track/TTTrackMonitor/TTaX/Overlap residual",
				      "Track/TTTrackMonitor/TTaU/Overlap residual",
				      "Track/TTTrackMonitor/TTbV/Overlap residual",
				      "Track/TTTrackMonitor/TTbX/Overlap residual"
                      ]}

}
mD0 = 1.86484
Configs = {
        "D0 Candidates":
				      {
				      #"Track/AlignD02KPiMonitor/massNegativeY":{'vlines':[1.84,1.88],'mean':True},
				      #"Track/AlignD02KPiMonitor/massPositiveY":{'vlines':[1.84,1.88],'mean':True},
				      "Track/AlignD02KPiMonitor/mass":{'vlines':[mD0-1e-3,mD0+1e-3],'mean':'X', 'xrange':[1.855,1.875], 'title': '(9) Mass [GeV]'},#'reference':['V',mD0],'xrange':[1.855,1.875]},
				      "Track/AlignD02KPiMonitor/massVersusMom":{'yrange':[mD0-1.5e-3,mD0+1.5e-3],#'reference':['H',mD0],'mean':'Y',
                      'lines':[(10,mD0-1e-3,200,mD0-1e-3),(10,mD0+1e-3,200,mD0+1e-3)],'xrange':[10,200], 'title': '(10) Mass vs. Momentum [GeV]'},
				      "Track/AlignD02KPiMonitor/massVersusPhiMatt":{'yrange':[mD0-1.5e-3,mD0+1.5e-3],#'reference':['H',mD0],'mean':'Y',
                      'hlines':[mD0-1e-3,mD0+1e-3], 'title': '(11) Mass [GeV] vs. #phi_{Matt}'},
				      "Track/AlignD02KPiMonitor/massVersusPt":{'yrange':[mD0-1.5e-3,mD0+1.5e-3],#'reference':['H',mD0], 'mean':'Y',
                      'lines':[(0,mD0-1e-3,15,mD0-1e-3),(0,mD0+1e-3,15,mD0+1e-3)],'xrange':[0,15], 'title': '(12) Mass vs. p_{T} [GeV]'},
				      "Track/AlignD02KPiMonitor/massVersusEta":{'yrange':[mD0-1.5e-3,mD0+1.5e-3],#'reference':['H',mD0],'mean':'Y', 
                      'lines':[(2,mD0-1e-3,5,mD0-1e-3),(2,mD0+1e-3,5,mD0+1e-3)],'xrange':[2,5], 'title': '(13) Mass [GeV] vs. #eta'},
                      },
        "IT/TT overlap residuals":
				      {
                      "Track/ITTrackMonitor/ASide/Overlap residual":{'vlines':[-0.05,0.05],'mean':'X','reference':['V',0],'xrange':[-0.2,0.2], 'title': '(14) IT ASide Boxes'},
				      "Track/ITTrackMonitor/CSide/Overlap residual":{'vlines':[-0.05,0.05],'mean':'X','reference':['V',0],'xrange':[-0.2,0.2], 'title': '(15) IT CSide Boxes'},
				      "Track/ITTrackMonitor/Top/Overlap residual":{'vlines':[-0.05,0.05],'mean':'X','reference':['V',0],'xrange':[-0.2,0.2], 'title': '(16) IT Top Boxes'},
				      "Track/ITTrackMonitor/Bottom/Overlap residual":{'vlines':[-0.05,0.05],'mean':'X','reference':['V',0],'xrange':[-0.2,0.2], 'title': '(17) IT Bottom Boxes'},
                      "Track/TTTrackMonitor/TTaX/Overlap residual":{'vlines':[-0.05,0.05],'mean':'X','reference':['V',0],'xrange':[-0.2,0.2], 'title': '(18) TTaX'},
				      "Track/TTTrackMonitor/TTaU/Overlap residual":{'vlines':[-0.05,0.05],'mean':'X','reference':['V',0],'xrange':[-0.2,0.2], 'title': '(19) TTaU'},
				      "Track/TTTrackMonitor/TTbV/Overlap residual":{'vlines':[-0.05,0.05],'mean':'X','reference':['V',0],'xrange':[-0.2,0.2], 'title': '(20) TTbV'},
				      "Track/TTTrackMonitor/TTbX/Overlap residual":{'vlines':[-0.05,0.05],'mean':'X','reference':['V',0],'xrange':[-0.2,0.2], 'title': '(21) TTbX'}
                      },
        "long track chisquare and Velo(TT)-T match curvature":
				     {"Track/TrackMonitor/Long/chi2ProbVsMom":{'title': '(1) prob(#chi^{2}) vs. Momentum [GeV]'},
				      "Track/TrackMonitor/Long/chi2ProbVsEta":{'title': '(2) prob(#chi^{2}) vs. #eta'},
				      "Track/TrackMonitor/Long/chi2ProbVsPhi":{'title': '(3) prob(#chi^{2}) vs. #phi'},
 				      "Track/TrackMonitor/Long/chi2ProbMatchVsMom":{'title': '(4) prob(#chi^{2}_{match}) vs. Momentum [GeV]'},
 				      "Track/TrackMonitor/Long/chi2ProbDownstreamVsMom":{'title': '(5) prob(#chi^{2}_{downstream}) vs. Momentum [GeV]'},
 				      "Track/TrackMonitor/Long/chi2ProbVeloVsMom":{'title': '(6) prob(#chi^{2}_{Velo}) vs. Momentum [GeV]'},
				      "Track/TrackFitMatchMonitor/curvatureRatioTToLongVsQoP":{'yrange':[0.95,1.05],#'reference':['H',1], 'mean':'Y',
                       'lines':[(-0.15,0.975,0.15,0.975),(-0.15,1.025,0.15,1.025)],'xrange':[-0.15,0.15],'title': '(7) curvature ratio T to Long versus q/p'},
				      "Track/TrackFitMatchMonitor/curvatureRatioVeloTTToLongVsQoP":{'yrange':[0.95,1.05],
                      #'reference':['H',1], 'mean':'Y',
                      'lines':[(-0.15,0.96,0.15,0.96),(-0.15,1.04,0.15,1.04)],'xrange':[-0.15,0.15],'title': '(8) curvature ratio VeloTT to Long versus q/p'}
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
    parser = argparse.ArgumentParser(description ="Macro to make plots to monitor automatic Tracker alignment")
    parser.add_argument('-r','--run', help='run number, default is the last one')
    parser.add_argument('--alignlog', help='location of alignlog.txt, default is guessed by run number')
    parser.add_argument('--histoFiles', help='location of histograms files, expect two paths, old ad new align histos, default is guessed by run number', nargs=2)
    parser.add_argument('-o','--outFile',help='output file name')
    args = parser.parse_args()

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

    for expression, title in [('TrChi2nDof', '(22) Track #chi^{2}/dof'), ('NormalisedChi2Change', '(23) #Delta #chi^{2}/dof')]:
        gr = RootUtils.makeGraph(RootUtils.getExpression(aout, expression))
        mps.append(MultiPlot(expression, title = '{0};Iteration;{0}'.format(title), histos = {expression:gr},
                       kind='g', drawLegend = False))
        mps[-1].DrawOption = 'alp'

    # 2 halves
    c.cd()
    mp1 = MultiPlot('1', title = '(24) Convergence TT Tz;Iteration;Variation [#mum]',kind='g', hlines=[0,-100,100],
                    hlines_colors = {-4: r.kGreen+3, 4: r.kGreen+3}, rangeY = [-250, 250])
    mp1.DrawOption = 'alp'
    mp1.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='TT/TTa/TTaXLayer')), 'TTaXLayer.Tz')
    mp1.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='TT/TTa/TTaULayer')), 'TTaULayer.Tz')
    mp1.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='TT/TTb/TTbVLayer')), 'TTbVLayer.Tz')
    mp1.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='TT/TTb/TTbXLayer')), 'TTbXLayer.Tz')

    mp2 = MultiPlot('2', title = '(25) Convergence IT1 Tz;Iteration;Variation [#mum]',kind='g', hlines=[0,-150,150], rangeY = [-250, 250])
    mp2.DrawOption = 'alp'
    mp2.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='IT/Station1/ASideBox')), 'IT1.ASideBox.Tz')
    mp2.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='IT/Station1/CSideBox')), 'IT1.CSideBox.Tz')
    mp2.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='IT/Station1/TopBox')), 'IT1.TopBox.Tz')
    mp2.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='IT/Station1/BottomBox')), 'IT1.BottomBox.Tz')

    mp3 = MultiPlot('2', title = '(26) Convergence IT2 Tz;Iteration;Variation [#mum]',kind='g', hlines=[0,-150,150], rangeY = [-250, 250])
    mp3.DrawOption = 'alp'
    mp3.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='IT/Station2/ASideBox')), 'IT2.ASideBox.Tz')
    mp3.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='IT/Station2/CSideBox')), 'IT2.CSideBox.Tz')
    mp3.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='IT/Station2/TopBox')), 'IT2.TopBox.Tz')
    mp3.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='IT/Station2/BottomBox')), 'IT2.BottomBox.Tz')

    mp4 = MultiPlot('2', title = '(27) Convergence IT3 Tz;Iteration;Variation [#mum]',kind='g', hlines=[0,-150,150], rangeY = [-250, 250])
    mp4.DrawOption = 'alp'
    mp4.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='IT/Station3/ASideBox')), 'IT3.ASideBox.Tz')
    mp4.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='IT/Station3/CSideBox')), 'IT3.CSideBox.Tz')
    mp4.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='IT/Station3/TopBox')), 'IT3.TopBox.Tz')
    mp4.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='IT/Station3/BottomBox')), 'IT3.BottomBox.Tz')


    mp5 = MultiPlot('3', title = '(28) Convergence OT Tz;Iteration;Variation [#mum]', kind='g', hlines=[0,-200,200], rangeY = [-250, 250] )
    mp5.DrawOption = 'alp'
    mp5.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='OT/T1X1U')), 'OT1.X1U.Tz')
    mp5.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='OT/T1VX2')), 'OT1.VX2.Tz')
    mp5.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='OT/T2X1U')), 'OT2.X1U.Tz')
    mp5.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='OT/T2VX2')), 'OT2.VX2.Tz')
    mp5.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='OT/T3X1U')), 'OT3.X1U.Tz')
    mp5.Add(RootUtils.makeGraph(*RootUtils.getDofDeltaConvergence(aout, dof='Tz', alignable='OT/T3VX2')), 'OT3.VX2.Tz')

    # c1 = getDrawnCanvas([mp1, mp2, mp3])

    mps += [mp1, mp2, mp3, mp4, mp5]

    #--> to be finished


    c1 = RootUtils.getDrawnCanvas(mps)
    c1.Print(outputFile_name)

    c.Print(outputFile_name+']')

    ##########################
