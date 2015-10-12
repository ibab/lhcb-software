
from ROOT import *
from optparse import OptionParser
import xml.etree.cElementTree as ET
import xml.dom.minidom as mindom
import commands,os


import numpy as n
from array import *
from math import *
parser = OptionParser()
from subprocess import Popen, PIPE, STDOUT

parser.add_option("-a", "--analysisType", type="string",
                  help="type of analyses (NTuple, Pedestals, CommonMode)",
                  dest="analysisType")

parser.add_option("-n", "--NumberOfEvents", type="int",
                  help="Number of events to be analysed",
                  dest="NumberOfEvents" , default = -1)

parser.add_option("-r", "--runNumber", type="string",
                  help="Run number to be analysed",
                  dest="runNumber")

parser.add_option("", "--refRunNumber", type="string",
                  help="reference run number for common mode suppression",
                  dest="refRunNumber",default = "-1")

parser.add_option("-s", "--scheme", type="string",
                  help="Optimisation scheme (relativeMax, relativeMax2, absoluteMax, singleBx)",
                  dest="scheme" , default = "relativeMax")

parser.add_option("-c", "--comments", type="string",
                  help="Comments",
                  dest="comments" , default = "")

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
                  dest="BrunelScriptName",default =  'rungaudi.py')

parser.add_option( "--Directory", type="string",
                  help="Reference directory for analysed runs", 
                  dest="Directory",default = '/home/herschel/AnalysedRuns')

parser.add_option( "--DataDirectory", type="string",
                  help="Top directory for data",
                  dest="DataDirectory", default = "/daqarea/lhcb/data/2015/RAW/FULL/HRC/TEST")

parser.add_option( "--BxCentral", type="int",
                  help="",
                  dest="BxCentral" , default = 10000)
parser.add_option( "--BxNext", type="int",
                  help="",
                  dest="BxNext" , default = 10000)
parser.add_option( "--BxPrev", type="int",
                  help="",
                  dest="BxPrev" , default = 10000)
Results = {}


stations = ['B0','B1','B2','F1','F2']
for s in stations:
    parser.add_option( "--step"+s, type = 'int',
                        help="force optimal step for "+s, 
                        dest="step"+s,default = -1)


options, arguments = parser.parse_args()


### Check the abrorescence for output and if Brunel needs to be run
localDir = options.Directory+'/'+options.runNumber
availDir = os.listdir(options.Directory)
if not options.runNumber in availDir:
    os.mkdir(localDir)
    options.RunBrunel = True
    os.mkdir(localDir+'/Plots')


if options.RunBrunel:
    if options.analysisType == 'DelayScan' and options.TAE == -1: options.TAE = 1
    try:  
        os.environ["BRUNELROOT"]
    except KeyError: 
        print "No Brunel environment is setup"
        sys.exit(1)
    version = os.environ["BRUNELROOT"].split('/')[-3].split('_')[1]
    scriptLocation = os.environ["User_release_area"]+'/Brunel_'+version+'/HC/HCMonitors/scripts/'+options.BrunelScriptName
    typeAnalysis = options.analysisType
    if options.analysisType == 'ApplyCommonMode':
        typeAnalysis = 'NTuple'
    mycommand = ['python',scriptLocation,'-a',typeAnalysis, '-r', options.runNumber, '-t' ,str(options.TAE), '-n', str(options.NumberOfEvents),'-d',options.DataDirectory ,  '--minBx',options.bxIDRange.split(',')[0],  '--maxBx',options.bxIDRange.split(',')[1],'--refRunNumber',options.refRunNumber,'-o',options.Directory]
    if options.analysisType == 'DelayScan' and options.scheme == 'singleBx' :
        mycommand.append("--BxCentral")
        mycommand.append(str(options.BxCentral))
        mycommand.append("--BxNext")
        mycommand.append(str(options.BxNext))
        mycommand.append("--BxPrev")
        mycommand.append(str(options.BxPrev))
    p = Popen(mycommand, stdout=PIPE, stdin=PIPE, stderr=STDOUT, bufsize=1)
    for line in iter(p.stdout.readline, ''):
        print line, 
    p.stdout.close()


if len(options.runNumber.split(','))>1:
   options.runNumber = options.runNumber.split(',')[0]+'_'+options.runNumber.split(',')[-1]


def fitHist(plot, name = 'gg', centered = False ):
    gau = TF1(name,"gaus",0.,500.)
    if centered:
        gau = TF1(name,"gaus",-250.,250.)
    gau.SetParameters(1,plot.GetMean())
    gau.SetParameters(2,plot.GetRMS())
    plot.Fit( gau,'QNR') 
    param = gau.GetParameters()
    gau.SetParLimits(0,param[0]*0.01,param[0]*100.)
    gau.SetParLimits(1,param[1]*0.5,param[0]*2.)
    gau.SetParLimits(2,param[2]*0.1,param[2]*10.)
    plot.Fit( gau,'QNR') 
    plot.Fit( gau,'QNR') 
    plot.Fit( gau,'QNR') 
    return gau.GetParameter(1),gau.GetParameter(2),gau

if options.analysisType == 'CommonMode':
    commonmode = ET.Element("COMMONMODE_RUN"+options.runNumber)
    xmlSide = {}
    f = TFile(options.Directory+'/'+options.runNumber+'/'+options.analysisType+'_'+options.runNumber+'.root','READ')
    Canvas = {}
    Hists,Fits = {},{}
    Results = {}
    for side in ['B','F']:
        for st in ['0','1','2']:
            if side == 'F' and st == '0': continue
            Canvas[side+st] = TCanvas(side+st,side+st)
            Canvas[side+st].Divide(4,2)
            for i,par in enumerate(['Odd','Even']):
                for j,q in enumerate(['0','1','2','3']):
                    Fits[side+st+q+par+'Plot']= TF1(side+st+q+par+"linFitPlot","[1]+[0]*x",0.,300.)
                    Fits[side+st+q+par+'Full']=TF2(side+st+q+par+"gaus2dfit","[0]*exp(-(cos([3])*cos([3])/(2*[4]*[4])+sin([3])*sin([3])/(2*[5]*[5]))*(x-[1])*(x-[1])-2*(-sin(2*[3])/(4*[4]*[4])+sin(2*[3])/(4*[5]*[5]))*(x-[1])*(y-[2])-(sin([3])*sin([3])/(2*[4]*[4])+cos([3])*cos([3])/(2*[5]*[5]))*(y-[2])*(y-[2]))")
                    Hists[side+st+q+par+'2D'] = f.Get('HCPedestalCorrection/Correlation/'+side+st+'/'+q+'/'+par)
                    Canvas[side+st].cd(j+4*i+1)
                    rmsX,rmsY = Hists[side+st+q+par+'2D'].GetRMS(1),Hists[side+st+q+par+'2D'].GetRMS(2)
                    if rmsX == 0.: 
                        print "RMS X null,",side+st+q+par
                        rmsX = rmsY
                    theta = TMath.ATan(rmsY/rmsX)
                    N = Hists[side+st+q+par+'2D'].GetEntries()
                    Fits[side+st+q+par+'Full'].SetParameter(0,N)
                    meanX,meanY = Hists[side+st+q+par+'2D'].GetMean(1),Hists[side+st+q+par+'2D'].GetMean(2)
                    Fits[side+st+q+par+'Full'].SetParameter(1,meanX)	#x_0
                    Fits[side+st+q+par+'Full'].SetParameter(2,meanY)	#y_0
                    Fits[side+st+q+par+'Full'].SetParameter(3,theta)	#theta  (estimated value from linear fit)
                    Fits[side+st+q+par+'Full'].SetParLimits(3,0.5,1.5)#theta limits
                    Fits[side+st+q+par+'Full'].SetParameter(4,5)	#sigma_x
                    Fits[side+st+q+par+'Full'].SetParameter(5,20)	#sigma_y
                    Fits[side+st+q+par+'Full'].SetParLimits(4,1.,20.)
                    Fits[side+st+q+par+'Full'].SetParLimits(5,10.,80.)
                    Fits[side+st+q+par+'Full'].SetContour(5)
                    Fits[side+st+q+par+'Full'].SetNpy(300)
                    Fits[side+st+q+par+'Full'].SetNpx(300)
                    Fits[side+st+q+par+'FullResult'] = Hists[side+st+q+par+'2D'].Fit(side+st+q+par+'gaus2dfit','QS')
                    theta=Fits[side+st+q+par+'Full'].GetParameter(3)
                    x0=Fits[side+st+q+par+'Full'].GetParameter(1)
                    y0=Fits[side+st+q+par+'Full'].GetParameter(2)
                    rmsX=Fits[side+st+q+par+'Full'].GetParameter(4)
                    rmsY=Fits[side+st+q+par+'Full'].GetParameter(5)
                    a_New =TMath.Tan(TMath.Pi()/2 - theta)
                    b_New = y0 - (a_New*x0)
                    xmlSide[side+st+q+par] = ET.SubElement(commonmode,side+st+q+par )
                    ET.SubElement(xmlSide[side+st+q+par], "theta", name="correlation").text = str(theta)
                    ET.SubElement(xmlSide[side+st+q+par], "X0", name="Mean Reference").text = str(x0)
                    ET.SubElement(xmlSide[side+st+q+par], "Y0", name="Mean Signal").text = str(y0)
                    ET.SubElement(xmlSide[side+st+q+par], "RMSUncorrected", name="Uncorrected RMS").text = str(rmsY)
                    Results[side+st+q+par]=[theta,x0,y0]
                    Fits[side+st+q+par+'Plot'].SetParameters(a_New,b_New)
                    Fits[side+st+q+par+'Plot'].SetLineColor(4)
                    Fits[side+st+q+par+'Full'].SetLineColor(4)
                    Hists[side+st+q+par+'2D'].Draw('col4z')
                    Fits[side+st+q+par+'Plot'].SetRange(0.,300.)
                    Fits[side+st+q+par+'Plot'].Draw("same")
            
            Canvas[side+st].SaveAs(options.Directory+'/'+options.runNumber+'/Plots/'+options.analysisType+'_'+options.runNumber+'_'+side+st+'.pdf')

    tree =  ET.ElementTree(commonmode)
    tree.write(options.Directory+'/'+options.runNumber+'/'+options.analysisType+'_'+options.runNumber+".xml")

elif options.analysisType == 'Pedestals':
    stations = ['B0','B1','B2','F1','F2']
    for s in stations:
        Results[s]={'VFEclock':-1,'ADCclock':-1,'OddPedestals':{},'EvenPedestals':{},'OddPedestalsFit':{},'EvenPedestalsFit':{},'OddPedestalsRMS':{},'EvenPedestalsRMS':{},
                    'OddPedestalsCor':{},'EvenPedestalsCor':{},'OddPedestalsFitCor':{},'EvenPedestalsFitCor':{},'OddPedestalsRMSCor':{},'EvenPedestalsRMSCor':{}}

    commonmode = ET.Element("PEDESTAL_RUN"+options.runNumber)
    xmlSide = {}

    f = TFile(options.Directory+'/'+options.runNumber+'/'+options.analysisType+'_'+options.runNumber+'.root','READ')
    gStyle.SetOptStat("")
    slots = ['Odd','Even']
    quadrants = ['0','1','2','3']
    slotColor = {'Odd':kBlue,'Even':kRed}
    Plots = {}
    Canvas = {}
    for s in stations:
        Canvas[s+'Pedestals']=TCanvas(s+'Pedestals',s+'Pedestals',1200,1000)
        Canvas[s+'Pedestals'].Divide(2,2)
        if int(options.refRunNumber) > 0:
          Canvas[s+'PedestalsCor']=TCanvas(s+'PedestalsCor',s+'PedestalsCor',1200,1000)
          Canvas[s+'PedestalsCor'].Divide(2,2)
        for ii,q in enumerate(quadrants):
            opt = ''
            ymax = 0.
            ymaxCor = 0.
            for sl in slots:
              Plots[s+q+sl+'_pedestals']=f.Get('HCDigitMonitor/ADC/'+s+'/'+sl+'/Quadrant'+q)
              if Plots[s + q + sl + "_pedestals"].GetMaximum() > ymax:
                ymax = Plots[s + q + sl + "_pedestals"].GetMaximum()
              if int(options.refRunNumber) > 0:
                Plots[s+q+sl+'_pedestals_Cor']=f.Get('HCCorrectedDigitMonitor/ADC/'+s+'/'+sl+'/Quadrant'+q)
                if Plots[s + q + sl + "_pedestals_Cor"].GetMaximum() > ymaxCor:
                  ymaxCor = Plots[s + q + sl + "_pedestals_Cor"].GetMaximum()
            for sl in slots:
                Plots[s+q+sl+'_pedestals'].GetXaxis().SetTitle(s+q+' ADC')
                Plots[s+q+sl+'_pedestals'].SetLineColor(slotColor[sl])
                Plots[s+q+sl+'_pedestals'].SetMarkerColor(slotColor[sl])
                #Plots[s+q+sl+'_pedestals'].GetXaxis().SetRangeUser(0,200)
                Plots[s + q + sl + "_pedestals"].GetYaxis().SetRangeUser(0., ymax)
                mean,rms,Results[s][sl+'PedestalsFit'][q] = fitHist(Plots[s+q+sl+'_pedestals'],s+q+sl+'_pedestalsFit')
                Results[s][sl+'Pedestals'][q] = int(mean)
                Results[s][sl+'PedestalsRMS'][q] = round(rms,1)
                xmlSide[s+q+sl] = ET.SubElement(commonmode,s+q+sl )
                ET.SubElement(xmlSide[s+q+sl], "mean", name="Pedestal Mean Value").text = str(round(mean,2))
                ET.SubElement(xmlSide[s+q+sl], "rms", name="Pedestal RMS Value").text = str(round(rms,2))
                if int(options.refRunNumber) > 0:
                  Plots[s+q+sl+'_pedestals_Cor'].GetXaxis().SetTitle(s+q+' ADC')
                  Plots[s+q+sl+'_pedestals_Cor'].SetLineColor(slotColor[sl])
                  Plots[s+q+sl+'_pedestals_Cor'].SetMarkerColor(slotColor[sl])
                  Plots[s + q + sl + "_pedestals_Cor"].GetYaxis().SetRangeUser(0., ymaxCor)
                  meanCor,rmsCor,Results[s][sl+'PedestalsFitCor'][q] = fitHist(Plots[s+q+sl+'_pedestals_Cor'],s+q+sl+'_pedestalsFitCor',True)
                  xmlSide[s+q+sl+'Cor'] = ET.SubElement(commonmode,s+q+sl+'Cor' )
                  ET.SubElement(xmlSide[s+q+sl], "mean_Cor", name="Corrected Pedestal Mean Value").text = str(round(meanCor,2))
                  ET.SubElement(xmlSide[s+q+sl], "rms_Cor", name="Corrected Pedestal RMS Value").text = str(round(rmsCor,2))

                Canvas[s+'Pedestals'].cd(ii+1)
                Results[s][sl+'PedestalsFit'][q].SetLineColor(slotColor[sl])
                Plots[s+q+sl+'_pedestals'].Draw(opt)
                opt = 'same'
                Results[s][sl+'PedestalsFit'][q].Draw(opt)
                if int(options.refRunNumber) > 0:
                  Canvas[s+'PedestalsCor'].cd(ii+1)
                  Results[s][sl+'PedestalsFitCor'][q].SetLineColor(slotColor[sl])
                  Plots[s+q+sl+'_pedestals_Cor'].Draw(opt)
                  opt = 'same'
                  Results[s][sl+'PedestalsFitCor'][q].Draw(opt)
                #Canvas[s+'Pedestals'].GetPad(ii+1).SetLogy()
                
        Canvas[s+'Pedestals'].SaveAs(options.Directory+'/'+options.runNumber+'/Plots/'+options.analysisType+'_'+options.runNumber+'_'+s+'_Pedestals.pdf')

        if int(options.refRunNumber) > 0:
          Canvas[s+'PedestalsCor'].SaveAs(options.Directory+'/'+options.runNumber+'/Plots/'+options.analysisType+'_'+options.runNumber+'_'+s+'_PedestalsCor.pdf')
    
    tree =  ET.ElementTree(commonmode)
    tree.write(options.Directory+'/'+options.runNumber+'/'+options.analysisType+'_'+options.runNumber+".xml")

elif options.analysisType == 'PMTIntensity':
    Channel,Pedestal,sigma,mean,gau = {},{},{},{},{}
    slope,exp,stepFcn,truncExp,Channel,hrcSignal,hrc0Signal,frac0Signal,hrc = {},{},{},{},{},{},{},{},{}
    myset = RooArgSet()
    for v in ['B00','B01','B02','B03','B10','B11','B12','B13','B20','B21','B22','B23','F10','F11','F12','F13','F20','F21','F22','F23']:
        Channel[v] = RooRealVar(v,v,-100.,800.)
        Pedestal[v] = RooRealVar('pedestal'+v,'pedestal'+v,0.,-200.,200.)
        sigma[v] = RooRealVar('sigma'+v,'sigma'+v,20.,5.,50.)
        mean[v] = RooRealVar('mean'+v,'mean'+v,0.)
        gau[v] = RooGaussian('gaus'+v,'gaus'+v,Channel[v],mean[v],sigma[v])
        slope[v] = RooRealVar('slope'+v,'slope'+v,-6.25300e-03,-1.e-1,-1.e-4)
        exp[v] = RooExponential("model"+v,"model"+v,Channel[v],slope[v])
        stepFcn[v] = RooFormulaVar('step'+v,v+' > pedestal'+v+' ? 1. : 0.',RooArgList(Channel[v],Pedestal[v]))
        truncExp[v] = RooEffProd ("truncExp"+v,"truncExp"+v,exp[v],stepFcn[v])
        Channel[v].setBins(10000,"cache")
        hrcSignal[v] = RooFFTConvPdf("hrcSignal"+v,"hrcSignal"+v,Channel[v],truncExp[v],gau[v])
        hrc0Signal[v] = RooGaussian('hrc0Signal'+v,'hrc0Signal+v',Channel[v],Pedestal[v],sigma[v])
        frac0Signal[v]= RooRealVar("frac0Signal"+v,"fraction of empty events"+v,0.2,0.,1.)
        hrc[v] =  RooAddPdf("hrc","hrc",hrc0Signal[v],hrcSignal[v],frac0Signal[v])
        myset.add(Channel[v])
    
    Canvas,frame={},{}
    myChain = TChain('Tree')
    myChain.Add('ApplyCommonMode_'+options.runNumber+'.root')
    dataset = RooDataSet("dataset","",myset,RooFit.Import(myChain))
    for side in ['B','F']:
        for station in ['0','1','2']:
            if side == 'F' and station == '0': continue
            Canvas[side+station]= TCanvas(side+station,side+station)
            Canvas[side+station].Divide(2,2)
            for kk, quad in enumerate(['0','1','2','3']):
                Canvas[side+station].cd(kk+1)
                hrc[side+station+quad].fitTo(dataset,RooFit.Minos(True),RooFit.Strategy(0),RooFit.Save(True),RooFit.NumCPU(2),RooFit.Verbose(False))
                frame[side+station+quad] = Channel[side+station+quad].frame()
                dataset.plotOn(frame[side+station+quad])
                hrc[side+station+quad].plotOn(frame[side+station+quad])
                frame[side+station+quad].Draw()
                
                
if options.analysisType == 'DelayScan':

    stations = ['B0','B1','B2','F1','F2']
    for s in stations:
        Results[s]={'VFEclock':-1,'ADCclock':-1,'OddPedestals':{},'EvenPedestals':{},'OddPedestalsFit':{},'EvenPedestalsFit':{},'OddPedestalsRMS':{},'EvenPedestalsRMS':{},
                    'OddPedestalsCor':{},'EvenPedestalsCor':{},'OddPedestalsFitCor':{},'EvenPedestalsFitCor':{},'OddPedestalsRMSCor':{},'EvenPedestalsRMSCor':{}}

    ftex = open(options.Directory+'/'+options.runNumber+'/'+options.analysisType+'_'+options.runNumber+'.tex','w')
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

    f = TFile(options.Directory+'/'+options.runNumber+'/'+options.analysisType+'_'+options.runNumber+'.root','READ')
    gStyle.SetOptStat("")
    slots = ['Previous','Central','Next']
    quadrants = ['0','1','2','3']
    slotColor = {'Previous':kBlue,'Central':kGreen,'Next':kRed}
    Plots = {}
    Canvas = {}
    maxStation = {}
    path = 'ADC'
    if int(options.refRunNumber) > 0:
        path = 'ADCCor'
    for s in stations:
        maxStation[s]=[-10.,-1]
        """
        Canvas[s+'Scan']=TCanvas(s+'Scan',s+'Scan',1200,1000)
        Canvas[s+'Scan'].Divide(2,2)
        Canvas[s+'ScanSummary']=TCanvas(s+'ScanSummary',s+'ScanSummary',1200,1000)
        Canvas[s+'ScanHist']=TCanvas(s+'ScanHist',s+'ScanHist',1200,1000)
        Canvas[s+'ScanHist'].Divide(2,2)
        Canvas[s+'Offset']=TCanvas(s+'Offset',s+'Offset',1200,1000)
        Canvas[s+'Offset'].Divide(2,2)
        """
        Canvas[s+'ScanMean']=TCanvas(s+'ScanMean',s+'ScanMean',1200,1000)
        Canvas[s+'ScanMean'].Divide(2,2)
        Canvas[s+'ScanRMS']=TCanvas(s+'ScanRMS',s+'ScanRMS',1200,1000)
        Canvas[s+'ScanRMS'].Divide(2,2)
        Plots[s+'_scan']=TH2D(s+'_scan',s+'_scan',16,-1.,33.,16,-1,31)
        Plots[s+'_scan'].GetXaxis().SetTitle(s+' VFE clock delay')
        Plots[s+'_scan'].GetYaxis().SetTitle(s+' ADC clock delay')
        for slot in slots:
            Plots[s+slot+'_mean']=TH1D(s+slot+'_mean',s+slot+'_mean',257,-0.5,256.5)
            Plots[s+slot+'_rms']=TH1D(s+slot+'_rms',s+slot+'_rms',257,-0.5,256.5)
            Plots[s+'_chi2Sum']=TH1D(s+'_chi2Sum',s+'_chi2Sum',257,-0.5,256.5)
        for ii,q in enumerate(quadrants):
            histos = {}
            Plots[s+q+'_scan']=TH2D(s+q+'_scan',s+q+'_scan',16,-1.,33.,16,-1,31)
            Plots[s+q+'_scan'].GetXaxis().SetTitle(s+q+' VFE clock delay')
            Plots[s+q+'_scan'].GetYaxis().SetTitle(s+q+' ADC clock delay')
            Plots[s+q+'_offset']=TH2D(s+q+'_offset',s+q+'_offset',16,-1.,33.,16,-1,31)
            Plots[s+q+'_offset'].GetXaxis().SetTitle(s+q+' VFE clock delay')
            Plots[s+q+'_offset'].GetYaxis().SetTitle(s+q+' ADC clock delay')
            Plots[s+q+'_chi2']=TH1D(s+q+'_chi2',s+q+'_chi2',257,-0.5,256.5)
            for slot in slots:
                Plots[s+q+slot+'_mean']=TH1D(s+q+slot+'_mean',s+q+slot+'_mean',257,-0.5,256.5)
                Plots[s+q+slot+'_rms']=TH1D(s+q+slot+'_rms',s+q+slot+'_rms',257,-0.5,256.5)
                histos[slot]=f.Get('HCDelayScan/'+path+'/'+slot+'/'+s+'/'+q)

            for ix in range(histos['Central'].GetNbinsX()+1):
                if ix > 255: continue
                fullHist = {}
                rms,mean = {},{}
                for slot in slots:
                    fullHist[slot] =  f.Get('HCDelayScan/'+path+'/'+str(ix)+'/'+s+'/'+q+'/'+slot)
                    rms[slot]=fullHist[slot].GetRMS()
                    mean[slot]=fullHist[slot].GetMean()
                    Plots[s+q+slot+'_mean'].Fill(ix,mean[slot])
                    thebin = Plots[s+slot+'_mean'].GetXaxis().FindBin(ix)
                    Plots[s+slot+'_mean'].SetBinContent(thebin,Plots[s+slot+'_mean'].GetBinContent(thebin)+mean[slot])
                    Plots[s+slot+'_rms'].SetBinContent(thebin,Plots[s+slot+'_rms'].GetBinContent(thebin)+rms[slot])
                    Plots[s+q+slot+'_rms'].Fill(ix,rms[slot])
        
                step = histos['Central'].GetBinCenter(ix)
                vfe = (step % 16)*2
                adc = int( step / 16 ) * 2
                maxAdc,bestSlot = -10,-10
                secondMaxADC = -100
                for kk,slot in enumerate(slots):
                    if  histos[slot].GetBinContent(ix)>maxAdc:
                        secondMaxADC = maxAdc
                        maxAdc = histos[slot].GetBinContent(ix)
                        bestSlot = kk -1
                    elif histos[slot].GetBinContent(ix)>secondMaxADC and  histos[slot].GetBinContent(ix)<=maxAdc:
                        secondMaxADC = histos[slot].GetBinContent(ix)
                bestRMSslot,maxRMS = -10,-10.
                for kk,slot in enumerate(slots):
                    if rms[slot]>maxRMS:
                        maxRMS=rms[slot]
                        bestRMSslot = kk -1
                
                if options.scheme == 'singleBx':
                    # only consider cases where the largest RMS channel is at 0
                    if bestRMSslot!=0:continue
                    # build a figure of merit as being as close as the rms of prev and next are 
                    
                    if abs(rms['Previous']+rms['Next'])<1e-6: continue
                    if abs(mean['Previous']+mean['Next'])<1e-6: continue
                    minAdc =pow(rms['Previous']-rms['Next'],2)/()#+pow(abs(mean['Previous']-mean['Next'])/(mean['Previous']+mean['Next']),2))
                    if minAdc <1e-6:continue
                    maxAdc = 1./minAdc

                    thebin = Plots[s+slot+'_mean'].GetXaxis().FindBin(ix)
                    
                    Plots[s+q+'_chi2'].Fill(ix,maxAdc*100.)
                    valMax = Plots[s+'_scan'].GetBinContent(Plots[s+q+'_scan'].GetXaxis().FindBin(vfe),Plots[s+q+'_scan'].GetYaxis().FindBin(adc))+maxAdc
                    Plots[s+q+'_scan'].SetBinContent(Plots[s+q+'_scan'].GetXaxis().FindBin(vfe),Plots[s+q+'_scan'].GetYaxis().FindBin(adc),maxAdc)
                    Plots[s+'_scan'].SetBinContent(Plots[s+q+'_scan'].GetXaxis().FindBin(vfe),Plots[s+q+'_scan'].GetYaxis().FindBin(adc),valMax)
                    Plots[s+q+'_offset'].SetBinContent(Plots[s+q+'_offset'].GetXaxis().FindBin(vfe),Plots[s+q+'_offset'].GetYaxis().FindBin(adc),mean['Central'])
                    if valMax > maxStation[s][0]:
                        maxStation[s][0]=valMax
                        maxStation[s][1]=step


                elif options.scheme == 'absoluteMax':
                    valMax = Plots[s+'_scan'].GetBinContent(Plots[s+q+'_scan'].GetXaxis().FindBin(vfe),Plots[s+q+'_scan'].GetYaxis().FindBin(adc))+maxAdc
                    Plots[s+q+'_scan'].SetBinContent(Plots[s+q+'_scan'].GetXaxis().FindBin(vfe),Plots[s+q+'_scan'].GetYaxis().FindBin(adc),maxAdc)
                    Plots[s+'_scan'].SetBinContent(Plots[s+q+'_scan'].GetXaxis().FindBin(vfe),Plots[s+q+'_scan'].GetYaxis().FindBin(adc),valMax)
                    Plots[s+q+'_offset'].SetBinContent(Plots[s+q+'_offset'].GetXaxis().FindBin(vfe),Plots[s+q+'_offset'].GetYaxis().FindBin(adc),bestSlot)
                    if valMax > maxStation[s][0]:
                        maxStation[s][0]=valMax
                        maxStation[s][1]=step
                elif options.scheme == 'relativeMax':
                    #print maxAdc,secondMaxADC
                    if maxAdc == 0. and secondMaxADC ==0.: maxAdc = 0.
                    elif secondMaxADC == 0.: maxAdc = 1.
                    else: maxAdc = maxAdc/secondMaxADC
                    valMax = Plots[s+'_scan'].GetBinContent(Plots[s+q+'_scan'].GetXaxis().FindBin(vfe),Plots[s+q+'_scan'].GetYaxis().FindBin(adc))+maxAdc
                    Plots[s+q+'_scan'].SetBinContent(Plots[s+q+'_scan'].GetXaxis().FindBin(vfe),Plots[s+q+'_scan'].GetYaxis().FindBin(adc),maxAdc)
                    Plots[s+'_scan'].SetBinContent(Plots[s+q+'_scan'].GetXaxis().FindBin(vfe),Plots[s+q+'_scan'].GetYaxis().FindBin(adc),valMax)
                    Plots[s+q+'_offset'].SetBinContent(Plots[s+q+'_offset'].GetXaxis().FindBin(vfe),Plots[s+q+'_offset'].GetYaxis().FindBin(adc),bestSlot)
                    if valMax > maxStation[s][0]:
                        maxStation[s][0]=valMax
                        maxStation[s][1]=step

        Plots[s+'Optimal']= TGraph()
        Plots[s+'Optimal'].SetName(s+'Optimal')
        Plots[s+'Optimal'].SetTitle(s+'Optimal')
        stepForce = getattr(options,'step'+s)
        vfe = (stepForce % 16)*2
        adc = int( stepForce / 16 ) * 2
        if stepForce >-0.5:
            print 'Force setp',s, vfe,adc
            maxStation[s][1]=stepForce
            maxStation[s][0]=Plots[s+'_scan'].GetBinContent(Plots[s+'_scan'].GetXaxis().FindBin(vfe),Plots[s+'_scan'].GetYaxis().FindBin(adc))
        Results[s]['VFEClock']=   (int(maxStation[s][1]) % 16) *2
        Results[s]['ADCClock']=   int( int(maxStation[s][1]) / 16 ) * 2
        Plots[s+'Optimal'].SetPoint(0,(int(maxStation[s][1]) % 16)*2,int( int(maxStation[s][1]) / 16 ) * 2)
        Plots[s+'Optimal'].SetMarkerStyle(30)
        Plots[s+'Optimal'].SetMarkerSize(3)
        '''
        Canvas[s+'ScanSummary'].cd()
        Plots[s+'_scan'].Draw('col4z')
        Plots[s+'Optimal'].Draw('Psame')
        '''
        for ix in range(histos['Central'].GetNbinsX()+1):
            if ix > 255: continue
            thebin = Plots[s+'Previous_mean'].GetXaxis().FindBin(ix)
            chi2Avg =pow(Plots[s+'Previous_rms'].GetBinContent(thebin)-Plots[s+'Next_rms'].GetBinContent(thebin),2)/(40.)
            Plots[s+'_chi2Sum'].SetBinContent(thebin,1./chi2Avg)

        for ii,q in enumerate(quadrants):
            """
            Canvas[s+'ScanHist'].cd(ii+1)
            Canvas[s+'ScanHist'].cd(ii+1).SetLogy()
            opt = ''
            for slot in ['Central','Previous','Next']:
                ##print str(int(maxStation[s][1])),s,slot
                Plots[s+str(int(maxStation[s][1]))+q+slot]=f.Get('HCDelayScan/'+path+'/'+str(int(maxStation[s][1]))+'/'+s+'/'+q+'/'+slot)
                Plots[s+str(int(maxStation[s][1]))+q+slot].SetLineColor(slotColor[slot])
                Plots[s+str(int(maxStation[s][1]))+q+slot].SetMarkerColor(slotColor[slot])
                Plots[s+str(int(maxStation[s][1]))+q+slot].Draw(opt)
                opt = 'same'
            Canvas[s+'Scan'].cd(ii+1)
            #Canvas[s+'Scan'].cd(ii+1).SetLogz()
            Plots[s+q+'_scan'].Draw('col4z') 
            Plots[s+'Optimal'].Draw('Psame')  
            Canvas[s+'Offset'].cd(ii+1)
            Plots[s+q+'_offset'].Draw('col4z')
            Plots[s+'Optimal'].Draw('Psame')
            """
            Canvas[s+'ScanRMS'].cd(ii+1)
            opt = 'hist'
            Plots[s+'_chi2Sum'].GetYaxis().SetRangeUser(0.,400.)
            Plots[s+'_chi2Sum'].SetLineColor(kBlack)
            Plots[s+'_chi2Sum'].SetLineWidth(3)
            Plots[s+'_chi2Sum'].SetMarkerColor(kBlack)
            Plots[s+'_chi2Sum'].Draw(opt)
            opt = 'histsame'
            for slot in ['Central','Previous','Next']:
                if slot == 'Central':
                    Plots[s+q+slot+'_rms'].GetYaxis().SetRangeUser(0.,400.)
                    Plots[s+q+slot+'_rms'].SetLineColor(kGreen)
                    Plots[s+q+slot+'_rms'].SetMarkerColor(kGreen)
                    Plots[s+q+slot+'_rms'].Draw(opt)
                if slot == 'Previous':
                    Plots[s+q+slot+'_rms'].SetLineColor(kBlue)
                    Plots[s+q+slot+'_rms'].SetMarkerColor(kBlue)
                    Plots[s+q+slot+'_rms'].Draw(opt)
                if slot == 'Next':
                    Plots[s+q+slot+'_rms'].SetLineColor(kRed)
                    Plots[s+q+slot+'_rms'].SetMarkerColor(kRed)
                    Plots[s+q+slot+'_rms'].Draw(opt)
                opt = 'histsame'
                
            Plots[s+q+'_chi2'].GetYaxis().SetRangeUser(0.,400.)
            Plots[s+q+'_chi2'].SetLineColor(kGray)
            Plots[s+q+'_chi2'].SetLineStyle(2)
            Plots[s+q+'_chi2'].SetMarkerColor(kGray)
            Plots[s+q+'_chi2'].Draw(opt)

'''

    for s in stations:
        Canvas[s+'ScanSummary'].SaveAs(options.Directory+'/'+options.runNumber+'/Plots/'+options.analysisType+'_'+options.runNumber+'_'+s+'_Summary.pdf')
        ftex.write("""   
        \\frame{
        \\frametitle{"""+s+""" Clock scan} 
        \\framesubtitle{Sum of the quadrants}
        \\begin{center}
        \\includegraphics[height=8cm]{"""+options.Directory+'/'+options.runNumber+'/Plots/'+options.analysisType+'_'+options.runNumber+'_'+s+'_Summary.pdf'+"""}
        \\end{center}
        }
        """) 
 

    ftex.write("""   
        \\frame{
        \\frametitle{For reciepe .conf file}
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
        Canvas[s+'Scan'].SaveAs(options.Directory+'/'+options.runNumber+'/Plots/'+options.analysisType+'_'+options.runNumber+'_'+s+'_Scan.pdf')
        ftex.write("""   
        \\frame{
        \\frametitle{"""+s+""" Clock scan} 
        \\framesubtitle{Per quadrants}
        \\begin{center}
        \\includegraphics[height=8cm]{"""+options.Directory+'/'+options.runNumber+'/Plots/'+options.analysisType+'_'+options.runNumber+'_'+s+'_Scan.pdf'+"""}
        \\end{center}
        }
        """)
        Canvas[s+'Offset'].SaveAs(options.Directory+'/'+options.runNumber+'/Plots/'+options.analysisType+'_'+options.runNumber+'_'+s+'_Offset.pdf')
        ftex.write("""   
        \\frame{
        \\frametitle{"""+s+""" resulting bxID offset} 
        \\framesubtitle{Per quadrants:increasing bxID correspond to decreasing L0Latencies}
        \\begin{center}
        \\includegraphics[height=8cm]{"""+options.Directory+'/'+options.runNumber+'/Plots/'+options.analysisType+'_'+options.runNumber+'_'+s+'_Offset.pdf'+"""}
        \\end{center}
        }
        """)
        Canvas[s+'ScanHist'].SaveAs(options.Directory+'/'+options.runNumber+'/Plots/'+options.analysisType+'_'+options.runNumber+'_'+s+'_Hist.pdf')
        ftex.write("""   
        \\frame{
        \\frametitle{"""+s+""" ADC distributions} 
        \\frametitle{For optimal point: Blue: Previous, Green: Central, Red: Next}
        \\begin{center}
        \\includegraphics[height=8cm]{"""+options.Directory+'/'+options.runNumber+'/Plots/'+options.analysisType+'_'+options.runNumber+'_'+s+'_Hist.pdf'+"""}
        \\end{center}
        }
        """)

    ftex.write("""
    \end{document}
    """)
    ftex.close()

    commands.getoutput('pdflatex '+options.Directory+'/'+options.runNumber+'/'+options.analysisType+'_'+options.runNumber+'.tex')
    commands.getoutput('mv '+options.analysisType+'_'+options.runNumber+'.pdf '+options.Directory+'/'+options.runNumber+'/'+options.analysisType+'_'+options.runNumber+'.pdf')
    commands.getoutput('rm '+options.analysisType+'_'+options.runNumber+'.*')
'''
