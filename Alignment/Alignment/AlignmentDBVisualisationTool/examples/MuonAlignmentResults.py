import os, sys
from AlignmentOutputParser.AlPlotter import *
from AlignmentOutputParser.AlignOutput import *
from matplotlib.backends.backend_pdf import PdfPages
###########################
# Load log-files
alIters = []
#for fdir in ['startfromITsurvey','startfromITsurvey2']:
for fdir in ['startfromITsurvey']:
    fileName = '/afs/cern.ch/work/m/mamartin/Alignment2015/outputs/run154182_%s/alignlog.txt' %fdir
    fileName = '/afs/cern.ch/user/s/siborghi/MuonAlign/run154725_muon/alignlog.txt'
    aout = AlignOutput(fileName)
    aout.Parse()
    alIters += aout.AlignIterations

###########################
# Check img directory existence
imgPath = 'img/MuonAlignmentResults/'
if not os.path.exists(imgPath): os.makedirs(imgPath)

###########################
# Create Pdf file
pdf = PdfPages('AlignOutput_run154182.pdf')

###########################
# Load info into plots creator
apl = AlPlotter(alIters)

# Plot Chi2
apl.PlotExpression('Chi2/NDOFs')
apl.Plots[-1].savefig(imgPath+'chi2ndof')
pdf.savefig()
    
Correction = {'survey': 'correction to survey', 'delta': 'correction to initial'}
#Correction = {'Delta': 'correction to initial'}
for cname, cdesc in Correction.iteritems():

    print 'Muon ', cname
    # Plot IT boxes
    dofs = ['Tx','Ty']
    for dof in dofs:
        yranges = {'1': {'min':-15,'max':20.}, '2': {'min':-5,'max':+6}, '3': {'min':-1.5,'max':2.5} } if dof == 'Tz' else None
        for stat in xrange(1,6):
            mdofs = []
            for box in ['ASide','CSide']:
                mdofs += ['Muon/M'+str(stat)+'/M'+str(stat)+box+'.'+dof]
                dum='Muon'+str(stat)+dof+' ('+cname+')'
                mycondition='Muon/M'+str(stat)+'/M'+str(stat)+box
                #print mdofs, "corr=",cdesc, "title=",dum, "yrange=", yranges[str(stat)], yranges
                apl.PlotAlignablesDofs( mdofs, corr=cdesc, title='Muon'+str(stat)+dof+' ('+cname+')', yrange= yranges[str(stat)] if yranges !=None else yranges)
                print mycondition,dof,cname
                #apl.PlotAlignableDof(mycondition,dof,par=cname)
            apl.Plots[-1].savefig(imgPath+'Station'+str(stat)+'_'+dof+'_'+cname)
            pdf.savefig()
            plt.close()

# Close the pdf
pdf.close()
