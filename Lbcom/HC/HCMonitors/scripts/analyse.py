from ROOT import *
from optparse import OptionParser
import commands,os
parser = OptionParser()
from subprocess import Popen, PIPE, STDOUT

parser.add_option("-a", "--analysisType", type="string",
                  help="type of analyses (DelayScan, Pedestals)",
                  dest="analysisType")

parser.add_option("-n", "--NumberOfEvents", type="int",
                  help="Number of events to be analysed",
                  dest="NumberOfEvents" , default = -1)

parser.add_option("-r", "--runNumber", type="string",
                  help="Run number to be analysed",
                  dest="runNumber")

parser.add_option("-c", "--comments", type="string",
                  help="Comments",
                  dest="comments" , default = "")

parser.add_option("-s", "--scheme", type="string",
                  help="Optimisation scheme (relativeMax, relativeMax2, or absoluteMax)",
                  dest="scheme" , default = "relativeMax")

parser.add_option( '-b',"--RunBrunel",action = 'store_true',
                  help="Run Brunel (needed once at least to get the ntuple)", 
                  dest="RunBrunel",default = False)

parser.add_option("-t", "--TAE", type="int",
                  help="TAE mode, number of prev and next",
                  dest="TAE", default = -1 )

parser.add_option( "--bxIDRange", type="string",
                  help="bxID range to consider",
                  dest="bxIDRange",default = "0,10000")

parser.add_option( "--PedestalOffset", type="int",
                  help="Offset to apply to the pedestals",
                  dest="PedestalOffset" , default = 0)

parser.add_option( "--BrunelScriptName", type="string",
                  help="Name of the Brunel script", 
                  dest="BrunelScriptName", default = 'clockScan.py')
parser.add_option( "--Directory", type="string",
                  help="Reference directory for analysed runs", 
                  dest="Directory",default = '/home/herschel/AnalysedRuns')
parser.add_option( "--DataDirectory", type="string",
                  help="Top directory for data",
                  dest="DataDirectory", default = "/daqarea/lhcb/data/2015/RAW/FULL/HRC/TEST")

Results = {}

stations = ['B0','B1','B2','F1','F2']
for s in stations:
    parser.add_option( "--step"+s, type = 'int',
                        help="force optimal step for "+s, 
                        dest="step"+s,default = -1)
    Results[s] ={'VFEclock':-1,'ADCclock':-1,'OddPedestals':{},'EvenPedestals':{},'OddPedestalsFit':{},'EvenPedestalsFit':{},'OddPedestalsRMS':{},'EvenPedestalsRMS':{}}

options, arguments = parser.parse_args()

if options.RunBrunel:
    if options.analysisType == 'DelayScan' and options.TAE == -1: options.TAE = 1
    try:  
        os.environ["BRUNELROOT"]
    except KeyError: 
        print "No Brunel environment is setup"
        sys.exit(1)
    version = os.environ["BRUNELROOT"].split('/')[-3].split('_')[1]
    scriptLocation = os.environ["User_release_area"]+'/Brunel_'+version+'/scripts/'+options.BrunelScriptName
    mycommand = ['python',scriptLocation,'-a',options.analysisType, '-r', options.runNumber, '-t' ,str(options.TAE), '-n', str(options.NumberOfEvents),'-d',options.DataDirectory ,  '--minBx',options.bxIDRange.split(',')[0],  '--maxBx',options.bxIDRange.split(',')[1]]
    p = Popen(mycommand, stdout=PIPE, stdin=PIPE, stderr=STDOUT, bufsize=1)
    for line in iter(p.stdout.readline, ''):
        print line, 
    p.stdout.close()


if len(options.runNumber.split(','))>1:
   options.runNumber = options.runNumber.split(',')[0]+'_'+options.runNumber.split(',')[-1]

def fitHist(plot, name = 'gg' ):
    gau = TF1(name,"gaus",0.,500.)
    gau.SetParameters(1,63.)
    gau.SetParameters(2,10.)
    plot.Fit( gau,'QNR') 
    param = gau.GetParameters()
    gau.SetParLimits(0,param[0]*0.01,param[0]*100.)
    gau.SetParLimits(1,param[1]*0.5,param[0]*2.)
    gau.SetParLimits(2,param[2]*0.1,param[2]*10.)
    plot.Fit( gau,'QNR') 
    plot.Fit( gau,'QNR') 
    plot.Fit( gau,'QNR') 
    return gau.GetParameter(1),gau.GetParameter(2),gau


if options.analysisType == 'DelayScan':
    ftex = open('results/run_'+options.runNumber+'.tex','w')
    ftex.write(
    """
    \\documentclass{beamer}
    \\usepackage{graphicx}
    \\begin{document}
    \\title{Clock Scan of run """+options.runNumber+""" }
    \\subtitle{"""+options.comments+"""}   
    \\author{Herschel} 
    \\date{\\today} 

    \\frame{\\titlepage} 
    """
    )
    onlyEven = False
    onlyOdd = False
    folderScan = "HCClockScan/SCAN/"
    folderOffset = "HCClockScan/OFFSET/"
    if onlyEven:
      folderScan += "EVEN/"
      folderOffset += "EVEN/"
    elif onlyOdd:
      folderScan += "ODD/" 
      folderOffset += "ODD/" 
    f = TFile('scan_' + options.runNumber + '.root', 'READ')
    gStyle.SetOptStat("")
    slots = ['Previous', 'Central', 'Next']
    quadrants = ['0', '1', '2', '3']
    slotColor = {'Previous': kBlue, 'Central': kGreen, 'Next': kRed}
    Plots = {}
    Canvas = {}
    maxStation = {}
    for s in stations:
        hScan = f.Get(folderScan + s)
        Plots[s + '_scan'] = hScan
        binx = Long()
        biny = Long()
        binz = Long() 
        noise = True
        if noise:
          bin = hScan.GetMinimumBin(binx, biny, binz)
        else:
          bin = hScan.GetMaximumBin(binx, biny, binz)
        vfe = int(hScan.GetXaxis().GetBinCenter(binx))
        adc = int(hScan.GetYaxis().GetBinCenter(biny))
        step = 32 * (adc / 2) + vfe 

        stepForce = getattr(options, 'step' + s)
        if stepForce > -0.5:
          step = stepForce
          vfe = stepForce % 32
          adc = int(stepForce / 32) * 2
          print 'Force step', s, vfe, adc

        Results[s]['VFEClock'] = vfe
        Results[s]['ADCClock'] = adc

        gOptimal = TGraph()
        gOptimal.SetName(s + 'Optimal')
        gOptimal.SetTitle(s + 'Optimal')
        gOptimal.SetPoint(0, vfe, adc)
        gOptimal.SetMarkerStyle(30)
        gOptimal.SetMarkerSize(3)
        Plots[s + 'Optimal'] = gOptimal

        Canvas[s + 'ScanSummary'] = TCanvas(s + 'ScanSummary', s + 'ScanSummary', 1200, 1000)
        Canvas[s + 'ScanSummary'].cd()
        Plots[s + '_scan'].Draw('col4z')
        gOptimal.Draw('Psame')

        Canvas[s + 'Scan'] = TCanvas(s + 'Scan', s + 'Scan', 1200, 1000)
        Canvas[s + 'Scan'].Divide(2, 2)
        Canvas[s + 'ScanHist'] = TCanvas(s + 'ScanHist', s + 'ScanHist', 1200, 1000)
        Canvas[s + 'ScanHist'].Divide(2, 2)
        Canvas[s + 'Offset'] = TCanvas(s + 'Offset', s + 'Offset', 1200, 1000)
        Canvas[s + 'Offset'].Divide(2, 2)
        for ii, q in enumerate(quadrants):
          Canvas[s + 'ScanHist'].cd(ii + 1)
          Canvas[s + 'ScanHist'].cd(ii + 1).SetLogy()
          opt = ''
          for slot in slots:
            title = s + str(step) + q + slot
            Plots[title] = f.Get('HCClockScan/ADC/' + str(step) + '/' + s + '/' + q + '/' + slot)

            if slot == 'Central':
              mean, rms, funct = fitHist(Plots[title])
              Results[s]['EvenPedestals'][q] = int(mean)
              Results[s]['EvenPedestalsRMS'][q] = round(rms,1)
            elif slot == 'Previous':
              mean, rms, funct = fitHist(Plots[title])
              Results[s]['OddPedestals'][q] = int(mean)
              Results[s]['OddPedestalsRMS'][q] = round(rms,1)

            Plots[title].SetLineColor(slotColor[slot])
            Plots[title].SetMarkerColor(slotColor[slot])
            Plots[title].Draw(opt)
            opt = 'same'

          Canvas[s + 'Scan'].cd(ii + 1)
          Plots[s + q + '_scan'] = f.Get(folderScan + s + q)
          Plots[s + q + '_scan'].Draw('col4z') 
          Plots[s + 'Optimal'].Draw('Psame')  
  
          Canvas[s + 'Offset'].cd(ii + 1)
          Plots[s + q + '_offset'] = f.Get(folderOffset + s + q)
          Plots[s + q + '_offset'].Draw('col4z')
          Plots[s + 'Optimal'].Draw('Psame')

    for s in stations:
        Canvas[s+'ScanSummary'].SaveAs('results/plots/'+options.runNumber+'_'+s+'_Summary.pdf')
        ftex.write("""   
        \\frame{
        \\frametitle{"""+s+""" Clock scan} 
        \\framesubtitle{Sum of the quadrants}
        \\begin{center}
        \\includegraphics[height=8cm]{"""+os.getcwd()+'/results/plots/'+options.runNumber+'_'+s+'_Summary.pdf'+"""}
        \\end{center}
        }
        """)   
        ftex.write("""   
        \\frame{
        \\frametitle{"""+s+""" configuration} 
        \\framesubtitle{}
        \\begin{itemize}
        \\item VFE clock:"""+ str(Results[s]['VFEClock'])+"""
        \\item ADC clock:"""+ str(Results[s]['ADCClock'])+"""
        \\item Pedestals:
        \\begin{itemize}
        \\item Quadrant 0: Odd """ +str(Results[s]['OddPedestals']['0'])+'('+str(Results[s]['OddPedestalsRMS']['0'])+') Even '+str(Results[s]['EvenPedestals']['0'])+'('+str(Results[s]['EvenPedestalsRMS']['0'])+')'+"""
        \\item Quadrant 1: Odd """ +str(Results[s]['OddPedestals']['1'])+'('+str(Results[s]['OddPedestalsRMS']['1'])+') Even '+str(Results[s]['EvenPedestals']['1'])+'('+str(Results[s]['EvenPedestalsRMS']['1'])+')'+"""
        \\item Quadrant 2: Odd """ +str(Results[s]['OddPedestals']['2'])+'('+str(Results[s]['OddPedestalsRMS']['2'])+') Even '+str(Results[s]['EvenPedestals']['2'])+'('+str(Results[s]['EvenPedestalsRMS']['2'])+')'+"""
        \\item Quadrant 3: Odd """ +str(Results[s]['OddPedestals']['3'])+'('+str(Results[s]['OddPedestalsRMS']['3'])+') Even '+str(Results[s]['EvenPedestals']['3'])+'('+str(Results[s]['EvenPedestalsRMS']['3'])+')'+"""
        \\end{itemize}
        \\end{itemize}
        }
        """)

    ftex.write("""   
        \\frame{
        \\frametitle{For recipe .conf file}
       \\begin{itemize}
       \\item B side \\\\
       Delay\\_Chip\\_0 """+ str(int(Results['B0']['VFEClock']+ Results['B2']['VFEClock'])/2)+""" """+str(Results['B1']['VFEClock'])+""" 11 17 \\\\
       Delay\\_Chip\\_2 """+ str(Results['B0']['ADCClock'])+""" """+ str(Results['B2']['ADCClock'])+""" """+ str(Results['B1']['ADCClock'])+"""  0  0
       \\item F side \\\\
       Delay\\_Chip\\_0 """+ str(Results['F2']['VFEClock'])+""" """+str(Results['F1']['VFEClock'])+""" 11 17 \\\\
       Delay\\_Chip\\_2 """+ str(Results['F2']['ADCClock'])+""" """+ str(Results['F1']['ADCClock'])+""" 0  0
       \\end{itemize}
        }
        """)  
    ftex.write("""   
        \\frame{
        \\frametitle{COMPLEMENTARY INFORMATIONS} 
        }
        """)  


    for s in stations:
        Canvas[s+'Scan'].SaveAs('results/plots/'+options.runNumber+'_'+s+'_Scan.pdf')
        ftex.write("""   
        \\frame{
        \\frametitle{"""+s+""" Clock scan} 
        \\framesubtitle{Per quadrants}
        \\begin{center}
        \\includegraphics[height=8cm]{"""+os.getcwd()+'/results/plots/'+options.runNumber+'_'+s+'_Scan.pdf'+"""}
        \\end{center}
        }
        """)
        Canvas[s+'Offset'].SaveAs('results/plots/'+options.runNumber+'_'+s+'_Offset.pdf')
        ftex.write("""   
        \\frame{
        \\frametitle{"""+s+""" resulting bxID offset} 
        \\framesubtitle{Per quadrants:increasing bxID correspond to decreasing L0Latencies}
        \\begin{center}
        \\includegraphics[height=8cm]{"""+os.getcwd()+'/results/plots/'+options.runNumber+'_'+s+'_Offset.pdf'+"""}
        \\end{center}
        }
        """)
        Canvas[s+'ScanHist'].SaveAs('results/plots/'+options.runNumber+'_'+s+'_Hist.pdf')
        ftex.write("""   
        \\frame{
        \\frametitle{"""+s+""" ADC distributions} 
        \\frametitle{For optimal point: Blue: Previous, Green: Central, Red: Next}
        \\begin{center}
        \\includegraphics[height=8cm]{"""+os.getcwd()+'/results/plots/'+options.runNumber+'_'+s+'_Hist.pdf'+"""}
        \\end{center}
        }
        """)

    ftex.write("""
    \end{document}
    """)
    ftex.close()

    commands.getoutput('pdflatex results/run_'+options.runNumber+'.tex')
    commands.getoutput('mv run_'+options.runNumber+'.pdf results/run_'+options.runNumber+'.pdf')
    commands.getoutput('rm run_'+options.runNumber+'.*')


if options.analysisType == 'Pedestals':
    ftex = open('results/run_'+options.runNumber+'.tex','w')
    ftex.write(
    """
    \\documentclass{beamer}
    \\usepackage{graphicx}
    \\begin{document}
    \\title{Pedestal analysis of run """+options.runNumber+""" }
    \\subtitle{"""+options.comments+"""}   
    \\author{Herschel} 
    \\date{\\today} 

    \\frame{\\titlepage} 
    """
    )
    f = TFile('scan_'+options.runNumber+'.root','READ')
    gStyle.SetOptStat("")
    slots = ['Odd','Even']
    quadrants = ['0','1','2','3']
    slotColor = {'Odd':kBlue,'Even':kRed}
    Plots = {}
    Canvas = {}
    for s in stations:
        Canvas[s+'Pedestals']=TCanvas(s+'Pedestals',s+'Pedestals',1200,1000)
        Canvas[s+'Pedestals'].Divide(2,2)
        for ii,q in enumerate(quadrants):
            Canvas[s+'Pedestals'].cd(ii+1)
            opt = ''
            for sl in slots:
                
                Plots[s+q+sl+'_pedestals']=f.Get('HCDigitMonitor/ADC/'+s+'/'+sl+'/Quadrant'+q)
                print s+q+sl+'_pedestals','HCDigitMonitor/ADC/'+s+'/'+sl+'/Quadrant'+q,Plots[s+q+sl+'_pedestals']
                Plots[s+q+sl+'_pedestals'].GetXaxis().SetTitle(s+q+' ADC')
                Plots[s+q+sl+'_pedestals'].SetLineColor(slotColor[sl])
                Plots[s+q+sl+'_pedestals'].SetMarkerColor(slotColor[sl])
                mean,rms,Results[s][sl+'PedestalsFit'][q] = fitHist(Plots[s+q+sl+'_pedestals'],s+q+sl+'_pedestalsFit')
                Results[s][sl+'Pedestals'][q] = int(mean)
                Results[s][sl+'PedestalsRMS'][q] = round(rms,1)
                Results[s][sl+'PedestalsFit'][q].SetLineColor(slotColor[sl])
                Plots[s+q+sl+'_pedestals'].Draw(opt)
                opt = 'same'
                Results[s][sl+'PedestalsFit'][q].Draw(opt)
                
        Canvas[s+'Pedestals'].SaveAs('results/plots/'+options.runNumber+'_'+s+'_Pedestals.pdf')
        ftex.write("""   
        \\frame{
        \\frametitle{"""+s+""" Pedestals} 
        \\framesubtitle{ADCs}
        \\begin{center}
        \\includegraphics[height=8cm]{"""+os.getcwd()+'/results/plots/'+options.runNumber+'_'+s+'_Pedestals.pdf'+"""}
        \\end{center}
        }
        """)   

        ftex.write("""   
        \\frame{
        \\frametitle{"""+s+""" Pedestals} 
        \\framesubtitle{Mean and RMS}
        \\begin{itemize}
        \\item Pedestals:
        \\begin{itemize}
        \\item Quadrant 0: Odd """ +str(Results[s]['OddPedestals']['0'])+'('+str(Results[s]['OddPedestalsRMS']['0'])+') Even '+str(Results[s]['EvenPedestals']['0'])+'('+str(Results[s]['EvenPedestalsRMS']['0'])+')'+"""
        \\item Quadrant 1: Odd """ +str(Results[s]['OddPedestals']['1'])+'('+str(Results[s]['OddPedestalsRMS']['1'])+') Even '+str(Results[s]['EvenPedestals']['1'])+'('+str(Results[s]['EvenPedestalsRMS']['1'])+')'+"""
        \\item Quadrant 2: Odd """ +str(Results[s]['OddPedestals']['2'])+'('+str(Results[s]['OddPedestalsRMS']['2'])+') Even '+str(Results[s]['EvenPedestals']['2'])+'('+str(Results[s]['EvenPedestalsRMS']['2'])+')'+"""
        \\item Quadrant 3: Odd """ +str(Results[s]['OddPedestals']['3'])+'('+str(Results[s]['OddPedestalsRMS']['3'])+') Even '+str(Results[s]['EvenPedestals']['3'])+'('+str(Results[s]['EvenPedestalsRMS']['3'])+')'+"""
        \\end{itemize}
        \\end{itemize}
        }
        """)

    ftex.write("""   
        \\frame{
        \\frametitle{Pedestals} 
        \\framesubtitle{BW Configuration for offset of """+str(options.PedestalOffset)+"""}
        \\begin{tiny}
        """)
    ftex.write('FEPGA0\_o255sets 255 255 255 255 255 255 '+str(Results['B0']['EvenPedestals']['0']-options.PedestalOffset)+' '+str(Results['B0']['OddPedestals']['0']-options.PedestalOffset)+' 255 255 255 255 255 255 '+str(Results['B0']['EvenPedestals']['1']-options.PedestalOffset)+' '+str(Results['B0']['OddPedestals']['1']-options.PedestalOffset)+'\\\\')
    ftex.write('FEPGA1\_o255sets 255 255 255 255 255 255 '+str(Results['B0']['EvenPedestals']['2']-options.PedestalOffset)+' '+str(Results['B0']['OddPedestals']['2']-options.PedestalOffset)+' 255 255 255 255 255 255 '+str(Results['B0']['EvenPedestals']['3']-options.PedestalOffset)+' '+str(Results['B0']['OddPedestals']['3']-options.PedestalOffset)+'\\\\')
    ftex.write('FEPGA2\_o255sets 255 255 255 255 '+str(Results['B2']['EvenPedestals']['0']-options.PedestalOffset)+' '+str(Results['B2']['OddPedestals']['0']-options.PedestalOffset)+' 255 255 255 255 255 255 '+str(Results['B2']['EvenPedestals']['1']-options.PedestalOffset)+' '+str(Results['B2']['OddPedestals']['1']-options.PedestalOffset)+' 255 255'+'\\\\')
    ftex.write('FEPGA3\_o255sets 255 255 255 255 '+str(Results['B2']['EvenPedestals']['2']-options.PedestalOffset)+' '+str(Results['B2']['OddPedestals']['2']-options.PedestalOffset)+' 255 255 255 255 255 255 '+str(Results['B2']['EvenPedestals']['3']-options.PedestalOffset)+' '+str(Results['B2']['OddPedestals']['3']-options.PedestalOffset)+' 255 255'+'\\\\')
    ftex.write('FEPGA4\_o255sets 255 255 '+str(Results['B1']['EvenPedestals']['0']-options.PedestalOffset)+' '+str(Results['B1']['OddPedestals']['0']-options.PedestalOffset)+' 255 255 255 255 255 255 '+str(Results['B1']['EvenPedestals']['1']-options.PedestalOffset)+' '+str(Results['B1']['OddPedestals']['1']-options.PedestalOffset)+' 255 255 255 255'+'\\\\')
    ftex.write('FEPGA5\_o255sets 255 255 '+str(Results['B1']['EvenPedestals']['2']-options.PedestalOffset)+' '+str(Results['B1']['OddPedestals']['2']-options.PedestalOffset)+' 255 255 255 255 255 255 '+str(Results['B1']['EvenPedestals']['3']-options.PedestalOffset)+' '+str(Results['B1']['OddPedestals']['3']-options.PedestalOffset)+' 255 255 255 255'+'\\\\')
    ftex.write('FEPGA6\_o255sets 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255'+'\\\\')
    ftex.write('FEPGA7\_o255sets 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255'+'\\\\')
    ftex.write("""   
        \\end{tiny}
        }
        \\frame{
        \\frametitle{Pedestals} 
        \\framesubtitle{FW Configuration for offset of """+str(options.PedestalOffset)+"""}
        \\begin{tiny}
        """)
    ftex.write('FEPGA0\_o255sets 255 255 255 255 255 255 '+str(Results['F2']['OddPedestals']['0']-options.PedestalOffset)+' '+str(Results['F2']['EvenPedestals']['0']-options.PedestalOffset)+' 255 255 255 255 255 255 '+str(Results['F2']['OddPedestals']['1']-options.PedestalOffset)+' '+str(Results['F2']['EvenPedestals']['1']-options.PedestalOffset)+'\\\\')
    ftex.write('FEPGA1\_o255sets 255 255 255 255 255 255 '+str(Results['F2']['OddPedestals']['2']-options.PedestalOffset)+' '+str(Results['F2']['EvenPedestals']['2']-options.PedestalOffset)+' 255 255 255 255 255 255 '+str(Results['F2']['OddPedestals']['3']-options.PedestalOffset)+' '+str(Results['F2']['EvenPedestals']['3']-options.PedestalOffset)+'\\\\')

    ftex.write('FEPGA2\_o255sets 255 255 '+str(Results['F1']['OddPedestals']['0']-options.PedestalOffset)+' '+str(Results['F1']['EvenPedestals']['0']-options.PedestalOffset)+' 255 255 255 255 255 255 '+str(Results['F1']['OddPedestals']['1']-options.PedestalOffset)+' '+str(Results['F1']['EvenPedestals']['1']-options.PedestalOffset)+' 255 255 255 255'+'\\\\')
    ftex.write('FEPGA3\_o255sets 255 255 '+str(Results['F1']['OddPedestals']['2']-options.PedestalOffset)+' '+str(Results['F1']['EvenPedestals']['2']-options.PedestalOffset)+' 255 255 255 255 255 255 '+str(Results['F1']['OddPedestals']['3']-options.PedestalOffset)+' '+str(Results['F1']['EvenPedestals']['3']-options.PedestalOffset)+' 255 255 255 255'+'\\\\')
    ftex.write('FEPGA4\_o255sets 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255'+'\\\\')
    ftex.write('FEPGA5\_o255sets 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255'+'\\\\')
    ftex.write('FEPGA6\_o255sets 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255'+'\\\\')
    ftex.write('FEPGA7\_o255sets 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255'+'\\\\')

    ftex.write(""" 
        \\end{tiny}
        }
        
    \end{document}
    """)
    ftex.close()

    commands.getoutput('pdflatex results/run_'+options.runNumber+'.tex')
    commands.getoutput('mv run_'+options.runNumber+'.pdf results/run_'+options.runNumber+'.pdf')
    commands.getoutput('rm run_'+options.runNumber+'.*')
