from AlignmentOutputParser.AlPlotter import *
from AlignmentOutputParser.AlignOutput import *
###########################
# Load log-files
fills = [2888,2891,2892,2893,2894]#,2898,2900,2902]
files = {}
fdir = '/afs/cern.ch/work/m/mamartin/public/AlignmentTestsRun2/'
for f in fills: files.update( { str(f) : fdir+'f%d/alignlog.txt' % f } )
# Parse the info from last iteration
alJobs = {'last': []}
for fill, file in files.iteritems():
    aout = AlignOutput(file)
    aout.Parse()
    alJobs.update( {fill: aout.AlignIterations} )
    alJobs['last']+=[aout.AlignIterations[-1]]

###########################
# Check img directory existence
import os
imgPath = 'img/consecutiveFills/'
if not os.path.exists(imgPath): os.makedirs(imgPath)


###########################
# Load info into plots creator
apls = {}
for name, item in alJobs.iteritems():
  apls.update( {name: AlPlotter(item)} )

###########################
# Check chi2 distributions
chis = {}
for name, apl in apls.iteritems():
  if name!='last':
    chis.update( {name: apl.DecodeExpression('Chi2/NDOFs')} )
apls['2888'].PlotExpression('Chi2/NDOFs')
apls['2888'].Plot(chis,legend=True)
apls['2888'].ResizeAxes(yrange={'min':1.1,'max':1.2})
apls['2888'].Plots[0].savefig(imgPath+'chi2ndof')
apls['2888'].Show()

###########################
# Check IT Boxes dofs
# Tz
apls['last'].SetAxis('variable', [str(f) for f in fills])
yranges = {'1': {'min':-15,'max':20.}, '2': {'min':-5,'max':+6}, '3': {'min':-1.5,'max':2.5} }
for stat in xrange(1,4):
    dofs = []
    for box in ['Top','Bottom','ASide','CSide']:
        dofs += ['IT/Station'+str(stat)+'/'+box+'Box.Tz']
    apls['last'].PlotAlignablesDofs( dofs, corr='correction to survey', title='Station'+str(stat)+' Tz', yrange= yranges[str(stat)])
    apls['last'].Plots[-1].savefig(imgPath+'Station'+str(stat)+'_Tz')
#apls['last'].Show()
# Tx
#apls['last'].SetAxis('variable', [str(f) for f in fills])
yranges = None#{'1': {'min':-1,'max':20.}, '2': {'min':-5,'max':+6}, '3': {'min':-1.5,'max':2.5} }
for stat in xrange(1,4):
    dofs = []
    for box in ['Top','Bottom','ASide','CSide']:
        dofs += ['IT/Station'+str(stat)+'/'+box+'Box.Tx']
    apls['last'].PlotAlignablesDofs( dofs, corr='correction to survey', title='Station'+str(stat)+' Tx', yrange= yranges[str(stat)] if yranges !=None else yranges )
    apls['last'].Plots[-1].savefig(imgPath+'Station'+str(stat)+'_Tx')
apls['last'].Show()
