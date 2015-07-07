import os, sys
from AlignmentOutputParser.AlPlotter import *
from AlignmentOutputParser.AlignOutput import *
from matplotlib.backends.backend_pdf import PdfPages
###########################
# Load log-files
alIters = []
for fdir in ['startfromITsurvey','startfromITsurvey2']:
    fileName = '/afs/cern.ch/work/m/mamartin/Alignment2015/outputs/run154182_%s/alignlog.txt' %fdir
    aout = AlignOutput(fileName)
    aout.Parse()
    alIters += aout.AlignIterations

###########################
# Check img directory existence
imgPath = 'img/TrackerAlignmentResults/'
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
for cname, cdesc in Correction.iteritems():

    print 'TT Layers', cname
    # Plot TT layers
    yranges = None#{'1': {'min':-15,'max':20.}, '2': {'min':-5,'max':+6}, '3': {'min':-1.5,'max':2.5} } if dof == 'Tz' else None
    dof = 'Tz'
    mdofs = []    
    for lay in ['aX','aU','bV','bX']:
        mdofs += ['TT/TT'+str(lay[0])+'/TT'+str(lay)+'Layer.'+dof]
    apl.PlotAlignablesDofs( mdofs, corr=cdesc, title='TTLayers '+dof+' ('+cname+')', yrange= yranges[str(stat)] if yranges !=None else yranges)
    apl.Plots[-1].savefig(imgPath+'TTLayers_'+dof+'_'+cname)
    pdf.savefig()
    plt.close()

    print 'TT modules', cname
    # Plot TT modules
    yranges = None#{'1': {'min':-15,'max':20.}, '2': {'min':-5,'max':+6}, '3': {'min':-1.5,'max':2.5} } if dof == 'Tz' else None
    dof = 'Tx'
    TTModulesStruct = { 'a': {1: xrange(1,7), 2:[1,'2B','2T',3], 3:xrange(1,7)},
                        'b': {1: xrange(1,7), 2:[1,2,'3B','3T',4,5], 3:xrange(1,7)}}
    for lay in ['aX','aU','bV','bX']:
        for sector in xrange(1,4):
            mdofs = [] 
            for module in TTModulesStruct[lay[0]][sector]:
                mdofs += ['TT'+lay+'LayerR'+str(sector)+'Module'+str(module)+'.'+dof]
            apl.PlotAlignablesDofs( mdofs, corr=cdesc, title='TT'+lay+' R'+str(sector)+' Modules '+dof+' ('+cname+')', yrange= yranges[str(sector)] if yranges !=None else yranges)
            apl.Plots[-1].savefig(imgPath+'TTLayer_'+lay+'_R'+str(sector)+'_'+dof+'_'+cname)
            pdf.savefig()
            plt.close()

    print 'IT boxes', cname
    # Plot IT boxes
    dofs = ['Tx','Tz','Rz']
    for dof in dofs:
        yranges = {'1': {'min':-15,'max':20.}, '2': {'min':-5,'max':+6}, '3': {'min':-1.5,'max':2.5} } if dof == 'Tz' else None
        for stat in xrange(1,4):
            mdofs = []
            for box in ['Top','Bottom','ASide','CSide']:
                mdofs += ['IT/Station'+str(stat)+'/'+box+'Box.'+dof]
            apl.PlotAlignablesDofs( mdofs, corr=cdesc, title='IT'+str(stat)+' Boxes '+dof+' ('+cname+')', yrange= yranges[str(stat)] if yranges !=None else yranges)
            apl.Plots[-1].savefig(imgPath+'Station'+str(stat)+'_'+dof+'_'+cname)
            pdf.savefig()
            plt.close()

    print 'OT C-frames', cname            
    # Plot OT C-frames
    dofs = ['Tx','Rz']
    for dof in dofs:
        yranges = None# {'1': {'min':-15,'max':20.}, '2': {'min':-5,'max':+6}, '3': {'min':-1.5,'max':2.5} } if dof == 'Tz' else None
        for stat in xrange(1,4):
            mdofs = []
            for box in ['%s%sSide' % (a,b) for a in ['X1U','VX2'] for b in ['A','C']]:
                mdofs += ['OT/T'+str(stat)+box+'.'+dof]
            apl.PlotAlignablesDofs( mdofs, corr=cdesc, title='OT'+str(stat)+' C-frames '+dof+' ('+cname+')', yrange= yranges[str(stat)] if yranges !=None else yranges)
            apl.Plots[-1].savefig(imgPath+'T'+str(stat)+'_Cframes_'+dof+'_'+cname)
            pdf.savefig()     
            plt.close()

    print 'OT Layers', cname
    # Plot T Layers
    dofs = ['Tz']
    for dof in dofs:
        yranges = None# {'1': {'min':-15,'max':20.}, '2': {'min':-5,'max':+6}, '3': {'min':-1.5,'max':2.5} } if dof == 'Tz' else None
        for stat in xrange(1,4):
            mdofs = []
            for box in ['X1U','VX2']:
                mdofs += ['OT/T'+str(stat)+box+'.'+dof]
            apl.PlotAlignablesDofs( mdofs, corr=cdesc, title='OT'+str(stat)+' layers '+dof+' ('+cname+')', yrange= yranges[str(stat)] if yranges !=None else yranges)
            apl.Plots[-1].savefig(imgPath+'T'+str(stat)+'_layers_'+dof+'_'+cname)
            pdf.savefig()  
            plt.close()

    print 'OT modules'
    # Loop on quadrants and stations
    dofs = ['Tx','Rz']
    for dof in dofs:
        yranges = None# {'1': {'min':-15,'max':20.}, '2': {'min':-5,'max':+6}, '3': {'min':-1.5,'max':2.5} } if dof == 'Tz' else None
        for stat in xrange(1,4):
            for lay in ['X1','U','V','X2']:
                for quad in ['Q(0|2)','Q(1|3)']:
                    mdofs = ['OT/T'+str(stat)+lay+quad+'M'+str(module)+'.'+dof for module in xrange(1,10)]
                    apl.PlotAlignablesDofs( mdofs, corr=cdesc, title='OT'+str(stat)+lay+quad+' modules '+dof+' ('+cname+')', yrange= yranges[str(stat)] if yranges !=None else yranges)
                    apl.Plots[-1].savefig(imgPath+'T'+str(stat)+lay+quad+'_modules_'+dof+'_'+cname)
                    pdf.savefig()  
                    plt.close()
# Close the pdf
pdf.close()
