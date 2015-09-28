from optparse import OptionParser
import xml.etree.cElementTree as ET
import xml.dom.minidom as mindom
import commands,os


import numpy as n
from array import *
from math import *
parser = OptionParser()
from subprocess import Popen, PIPE, STDOUT

parser.add_option("-r", "--runNumber", type="string",
                  help="Run number to be analysed",
                  dest="runNumber")

parser.add_option( "--Directory", type="string",
                  help="Reference directory for analysed runs", 
                  dest="Directory",default = '/group/hrc/calib/EOFCALIB')


parser.add_option( "--DataDirectory", type="string",
                  help="Top directory for data",
                  dest="DataDirectory", default = "/daqarea/lhcb/data/2015/RAW/FULL/LHCb/EOF_CALIB15")
                  # dest="DataDirectory", default = "/daqarea/lhcb/data/2015/RAW/FULL/HRC/TEST")

options, arguments = parser.parse_args()

for runNum in options.runNumber.split(','):
    
    print 'Running evaluation of common mode subtraction for run '+runNum+' ... can take a few minutes'
    h = commands.getoutput('python analyseHRC.py -r '+runNum+' -a CommonMode -n 300000 -b --DataDirectory '+options.DataDirectory+' --Directory '+options.Directory)

    print 'Running evaluation of corrected and uncorrected pedestals for run '+runNum+' ... can take a few minutes'
    h = commands.getoutput('python analyseHRC.py -r '+runNum+' -a Pedestals -n 300000 -b --DataDirectory '+options.DataDirectory+' --refRunNumber '+runNum+' --Directory '+options.Directory)
    

    print 'Creating the summary...'
    
    commonmodeXML = ET.parse(options.Directory+'/'+runNum+'/CommonMode_'+runNum+".xml")
    pedestalXML = ET.parse(options.Directory+'/'+runNum+'/Pedestals_'+runNum+".xml")

    ftex = open(options.Directory+'/'+runNum+'/EOFSummary_'+runNum+'.tex','w')
    ftex.write("""
    \\documentclass{beamer}
    \\usepackage{graphicx}
    \\begin{document}
    \\title{Analysis of EOFCalib run """+runNum+""" }
    \\author{Herschel} 
    \\date{\\today} 
    \\frame{\\titlepage} 
    """)



    for s in ['B0','B1','B2','F1','F2']:

        #### Put the plots

        ftex.write("""   
        \\frame{
        \\frametitle{"""+s+""" Uncorrected Pedestals} 
        \\framesubtitle{}
        \\begin{center}
        \\includegraphics[height=7cm]{"""+options.Directory+'/'+runNum+'/Plots/Pedestals_'+runNum+'_'+s+'_Pedestals.pdf'+"""}
        \\end{center}
        }
        """)   
        ftex.write("""   
        \\frame{
        \\frametitle{"""+s+""" Correlation with Reference channel} 
        \\framesubtitle{}
        \\begin{center}
        \\includegraphics[height=7cm]{"""+options.Directory+'/'+runNum+'/Plots/CommonMode_'+runNum+'_'+s+'.pdf'+"""}
        \\end{center}
        }
        """)   
        ftex.write("""   
        \\frame{
        \\frametitle{"""+s+""" Corrected Pedestals} 
        \\framesubtitle{}
        \\begin{center}
        \\includegraphics[height=7cm]{"""+options.Directory+'/'+runNum+'/Plots/Pedestals_'+runNum+'_'+s+'_PedestalsCor.pdf'+"""}
        \\end{center}
        }
        """)   

        
        #### Put the values
        ftex.write("""   
        \\frame{
        \\frametitle{"""+s+""" Pedestal Values} 
        \\framesubtitle{Mean and RMS}
        \\begin{small}
        \\begin{itemize}
        \\item Uncorrected:
        \\begin{itemize}""")
        for q in range(4):
            line="   \\item Quadrant "+str(q)+': '
            for p in ['Odd','Even']:
               line+= p+' '+pedestalXML.find(s+str(q)+p).find('mean').text+' ('+pedestalXML.find(s+str(q)+p).find('rms').text+') '
            ftex.write(line+'\n')
        ftex.write("""   
        \\end{itemize}
        """)
        ftex.write("""   
        \\item Corrected:
        \\begin{itemize}""")
        for q in range(4):
            line="   \\item Quadrant "+str(q)+': '
            for p in ['Odd','Even']:
               line+= p+' '+pedestalXML.find(s+str(q)+p).find('mean_Cor').text+' ('+pedestalXML.find(s+str(q)+p).find('rms_Cor').text+') '
            ftex.write(line+'\n')
        ftex.write("""   
        \\end{itemize} 
        \\end{itemize}
        \\end{small}
        }
        """)

        ftex.write("""   
        \\frame{
        \\frametitle{"""+s+""" calibration parameters} 
        \\framesubtitle{$ADC_{cor} = ADC-(ADC_{ref}-X_{0})*tan( \\theta )-Y_{0}$}
        \\begin{itemize}
        \\item $\\theta$:
        \\begin{itemize}""")
        for q in range(4):
            line="   \\item Quadrant "+str(q)+': '
            for p in ['Odd','Even']:
               line+= p+' '+str(round(float(commonmodeXML.find(s+str(q)+p).find('theta').text),5))+' '
            ftex.write(line+'\n')
        ftex.write("""   
        \\end{itemize}
        """)
        ftex.write("""   
        \\item $X_{0}$:
        \\begin{itemize}""")
        for q in range(4):
            line="   \\item Quadrant "+str(q)+': '
            for p in ['Odd','Even']:
               line+= p+' '+str(round(float(commonmodeXML.find(s+str(q)+p).find('X0').text),1))+' '
            ftex.write(line+'\n')
        ftex.write("""   
        \\end{itemize}
        """)
        ftex.write("""   
        \\item $Y_{0}$:
        \\begin{itemize}""")
        for q in range(4):
            line="   \\item Quadrant "+str(q)+': '
            for p in ['Odd','Even']:
               line+= p+' '+str(round(float(commonmodeXML.find(s+str(q)+p).find('Y0').text),1))+' '
            ftex.write(line+'\n')
        ftex.write("""
        \\end{itemize}
        \\end{itemize}
        }
        """)



    ftex.write("""   
        \\end{document}
        """)
    ftex.close()
        
        
    commands.getoutput('pdflatex '+options.Directory+'/'+runNum+'/EOFSummary_'+runNum+'.tex')
    print 'mv EOFSummary_'+runNum+'.pdf '+options.Directory+'/'+runNum+'/EOFSummary_'+runNum+'.pdf'
    commands.getoutput('mv EOFSummary_'+runNum+'.pdf '+options.Directory+'/'+runNum+'/EOFSummary_'+runNum+'.pdf')
    commands.getoutput('rm EOFSummary_'+runNum+'.*')


print 'Updating the '
