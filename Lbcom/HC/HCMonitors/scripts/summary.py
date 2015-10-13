from optparse import OptionParser
import xml.etree.cElementTree as ET
import xml.dom.minidom as mindom
import commands,os

from ROOT import *
import numpy as n
from array import *
from math import *
parser = OptionParser()
from subprocess import Popen, PIPE, STDOUT

parser.add_option("-r", "--runNumber", type="string",
                  help="Run number to be analysed",
                  dest="runNumber",default = "")

parser.add_option( "--Directory", type="string",
                  help="Reference directory for analysed runs", 
                  dest="Directory",default = '/group/hrc/calib/EOFCALIB')

parser.add_option( "--DataDirectory", type="string",
                  help="Top directory for data",
                  dest="DataDirectory", default = "/daqarea/lhcb/data/2015/RAW/FULL/LHCb/EOF_CALIB15")
                  # dest="DataDirectory", default = "/daqarea/lhcb/data/2015/RAW/FULL/HRC/TEST")

options, arguments = parser.parse_args()

colors = {'Odd':kBlue,'Even':kRed}
style = {'mean':kFullCircle,'mean_Cor':kOpenCircle,'rms':kFullCircle,'rms_Cor':kOpenCircle,'X0':kFullCircle,'Y0':kOpenCircle,'theta':kFullCircle,'X0Y0':kFullCircle}
linestyle = {'mean':1,'mean_Cor':2,'rms':1,'rms_Cor':2,'X0':1,'Y0':2,'theta':1,'X0Y0':2}
title = {'mean':'Pedestal Mean','mean_Cor':'Pedestal Mean','rms':'Pedestal RMS','rms_Cor':'Pedestal RMS','X0':'X_{0},Y_{0}','Y0':'X_{0},Y_{0}','theta':'#theta','X0Y0':'Y0' }
Graph = {}
for g in ['mean','rms','mean_Cor','rms_Cor','theta','X0','Y0','X0Y0']:
    Graph[g]= {}
    for s in ['B0','B1','B2','F1','F2']:
        Graph[g][s]= {}
        for q in ['0','1','2','3']:
            Graph[g][s][q]= {}
            for p in ['Odd','Even']:
                Graph[g][s][q][p]=TGraphErrors()
                Graph[g][s][q][p].SetName(q+'_'+s+q+p)
                Graph[g][s][q][p].SetTitle(q+'_'+s+q+p)
                Graph[g][s][q][p].SetLineColor(colors[p])
                Graph[g][s][q][p].SetMarkerColor(colors[p])
                Graph[g][s][q][p].SetMarkerStyle(style[g])
                Graph[g][s][q][p].SetLineStyle(linestyle[g])
                Graph[g][s][q][p].SetMarkerSize(0.2)


allFiles = os.listdir(options.Directory)
runNumbers = []
for f in allFiles:
    if f.count('.')>0.5:continue
    runNumbers.append(f)

if options.runNumber!="":
    runNumbers = options.runNumber.split(',')

runNumbersSorted = [int(r) for r in runNumbers]
runNumbersSorted.sort()
runNumbers = [str(r) for r in runNumbersSorted]

for runNum in runNumbers:
    commonmodeXML = ET.parse(options.Directory+'/'+runNum+'/CommonMode_'+runNum+".xml")
    pedestalXML = ET.parse(options.Directory+'/'+runNum+'/Pedestals_'+runNum+".xml")
    
    for g in ['mean','rms','mean_Cor','rms_Cor']:
        for s in ['B0','B1','B2','F1','F2']:
            for q in ['0','1','2','3']:
                for p in ['Odd','Even']:
                    Graph[g][s][q][p].SetPoint(Graph[g][s][q][p].GetN(),int(runNum),float(pedestalXML.find(s+q+p).find(g).text)) 

    for g in ['theta','X0','Y0','X0Y0']:
        for s in ['B0','B1','B2','F1','F2']:
            for q in ['0','1','2','3']:
                for p in ['Odd','Even']:
                    if g == 'X0Y0':
                        Graph[g][s][q][p].SetPoint(Graph[g][s][q][p].GetN(),float(commonmodeXML.find(s+q+p).find('X0').text),float(commonmodeXML.find(s+q+p).find('Y0').text)) 
                        if commonmodeXML.find(s+q+p).find('X0_err')!=None:
                            Graph[g][s][q][p].SetPointError(Graph[g][s][q][p].GetN()-1,float(commonmodeXML.find(s+q+p).find('X0_err').text),float(commonmodeXML.find(s+q+p).find('Y0_err').text)) 
                    else:
                        Graph[g][s][q][p].SetPoint(Graph[g][s][q][p].GetN(),int(runNum),float(commonmodeXML.find(s+q+p).find(g).text)) 
                        if commonmodeXML.find(s+q+p).find(g+'_err')!=None:
                            Graph[g][s][q][p].SetPointError(Graph[g][s][q][p].GetN()-1,0.,float(commonmodeXML.find(s+q+p).find(g+'_err').text)) 



Canvas = {}

for c in ['mean','rms','theta','XY','X0Y0']:
    Canvas[c] = TCanvas(c,c)
    Canvas[c].Divide(4,5)
    for j,s in enumerate(['B0','B1','B2','F1','F2']):
        for i,q in enumerate(['0','1','2','3']):
            Canvas[c].cd(i+1+j*4)
            opt = 'APLE'
            for p in ['Odd','Even']:
                if c == 'mean' or c == 'rms':
                    Graph[c][s][q][p].Draw(opt)
                    Graph[c][s][q][p].GetXaxis().SetTitle('EOFCALIB15 Run Number')
                    Graph[c][s][q][p].GetYaxis().SetTitle(title[c])
                    if c== 'mean': Graph[c][s][q][p].GetYaxis().SetRangeUser(-10.,100.)
                    if c== 'rms': Graph[c][s][q][p].GetYaxis().SetRangeUser(0.,30.)
                    opt = 'PLEsame'
                    Graph[c+'_Cor'][s][q][p].Draw(opt)
                if c == 'theta':
                    Graph[c][s][q][p].Draw(opt)
                    Graph[c][s][q][p].GetXaxis().SetTitle('EOFCALIB15 Run Number')
                    Graph[c][s][q][p].GetYaxis().SetTitle(title[c])
                    Graph[c][s][q][p].GetYaxis().SetRangeUser(0.6,1.2)
                    opt = 'PLEsame'
                if c == 'XY':
                    for cc in ['X0','Y0']:
                        Graph[cc][s][q][p].Draw(opt)
                        Graph[cc][s][q][p].GetXaxis().SetTitle('EOFCALIB15 Run Number')
                        Graph[cc][s][q][p].GetYaxis().SetTitle(title[cc])
                        Graph[cc][s][q][p].GetYaxis().SetRangeUser(0.,250.)
                        opt = 'PLEsame'
                if c == 'X0Y0':
                        Graph[c][s][q][p].GetXaxis().SetRangeUser(0.,250.)
                        Graph[c][s][q][p].GetYaxis().SetRangeUser(0.,250.)
                        Graph[c][s][q][p].Draw(opt)
                        Graph[c][s][q][p].GetXaxis().SetTitle('X0')
                        Graph[c][s][q][p].GetYaxis().SetTitle('Y0')
                        Graph[c][s][q][p].GetXaxis().SetRangeUser(0.,250.)
                        Graph[c][s][q][p].GetYaxis().SetRangeUser(0.,250.)
                        opt = 'PLEsame'
    Canvas[c].SaveAs(options.Directory+'/'+c+'_VS_runNumber.pdf')


ftex = open(options.Directory+'/CSTVariation.tex','w')
ftex.write("""
\\documentclass{beamer}
\\usepackage{graphicx}
\\begin{document}
\\title{Variation of Calib constante with time}
\\author{Herschel} 
\\date{\\today} 
\\frame{\\titlepage} 
""")


ftex.write("""   
\\frame{
\\frametitle{Variation of X0 and Y0} 
\\framesubtitle{X0 full circle, Y0 open circle, Odd Blue, Even Red }
\\begin{center}
\\includegraphics[height=7cm]{"""+options.Directory+'/XY_VS_runNumber.pdf'+"""}
\\end{center}
}
""")  
ftex.write("""   
\\frame{
\\frametitle{Variation of X0 vs Y0} 
\\framesubtitle{ Odd Blue, Even Red, should give theta}
\\begin{center}
\\includegraphics[height=7cm]{"""+options.Directory+'/X0Y0_VS_runNumber.pdf'+"""}
\\end{center}
}
""")    
ftex.write("""   
\\frame{
\\frametitle{Variation of $\\theta$} 
\\framesubtitle{Odd Blue, Even Red }
\\begin{center}
\\includegraphics[height=7cm]{"""+options.Directory+'/theta_VS_runNumber.pdf'+"""}
\\end{center}
}
""")   
ftex.write("""   
\\frame{
\\frametitle{Variation of Pedestal Mean} 
\\framesubtitle{RAW full circle, Corrected open circle, Odd Blue, Even Red }
\\begin{center}
\\includegraphics[height=7cm]{"""+options.Directory+'/mean_VS_runNumber.pdf'+"""}
\\end{center}
}
""")   
ftex.write("""   
\\frame{
\\frametitle{Variation of Pedestal RMS} 
\\framesubtitle{RAW full circle, Corrected open circle, Odd Blue, Even Red }
\\begin{center}
\\includegraphics[height=7cm]{"""+options.Directory+'/rms_VS_runNumber.pdf'+"""}
\\end{center}
}
""")   

ftex.write("""   
\\frame{
\\frametitle{Run Number considered} 
\\framesubtitle{from EOFCALIB15}
""")
line = ''
for r in runNumbers:
   line+= r+', '
ftex.write(line+'\n')
ftex.write("""   
}
""")   




ftex.write("""   
\\end{document}
""")
ftex.close()


commands.getoutput('pdflatex '+options.Directory+'/CSTVariation.tex')
commands.getoutput('mv CSTVariation.pdf '+options.Directory+'/CSTVariation.pdf')
commands.getoutput('rm CSTVariation.*')

