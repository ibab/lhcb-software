# $Id: HistoVis.py,v 1.3 2004-10-27 14:08:19 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================

# =============================================================================
# @file
#
# "Solution"-file for 'RCselect.py' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================


# import everything from BENDER
from bendermodule import *

# get the CONFIGURATION utilities
import benderconfig as bender

# =============================================================================
# Job configuration 
# =============================================================================
def configure() :
    
    bender.config ( files = ['$BENDERTUTOROPTS/BenderTutor.opts' ] )
    
    # modify/update the configuration:
    
    # Preload all charged particles 
    g.TopAlg +=  [ 'LoKiPreLoad/Charged' ]
    
    import benderPreLoad as preload
    preload.Charged( Name  = 'Charged' ,
                     Kaons = [ "det='RICH'  k-pi='0.0' k-p='-5.0' " ] )
    
    # 1) create the algorithm
    from HandsOn4 import RCKaons    
    alg = RCKaons( 'RCKaons' )
    
    # 2) replace the list of top level algorithm by only *THIS* algorithm
    g.TopAlg += [ 'RCKaons' ]
    
    # 3) configure algorithm
    desktop = g.property('RCKaons.PhysDesktop')
    desktop.InputLocations = [ "/Event/Phys/Charged" ]

    # 4) ouput histograms 
    g.HistogramPersistency = 'ROOT' 
    hsvc = g.service('HistogramPersistencySvc')
    hsvc.OutputFile = 'rckaonhistos.root'

    # 5) output N-Tuples:
    ntsvc = g.service('NTupleSvc')
    ntsvc.Output = [ "RC DATAFILE='rckaontuples.root' TYP='ROOT' OPT='NEW'" ]

    myAlg = g.algorithm('RCKaons')
    myAlg.NTupleLUN = 'RC'

    # redefine input files 
    evtsel = g.evtSel()
    evtsel.open( [ 'LFN:/lhcb/production/DC04/v1/DST/00000543_00000017_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000018_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000016_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000020_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000024_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000019_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000021_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000022_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000001_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000002_5.dst' ] )
        
    return SUCCESS 
# =============================================================================


# =============================================================================
# The control flow 
# =============================================================================
if __name__ == '__main__' :

    # job configuration
    configure()

    # event loop 
    g.run(500)

    # get histograms
    hsvc = g.histSvc()
    
    h1 = hsvc['/stat/RCKaons/1']
    h2 = hsvc['/stat/RCKaons/2']
    print 'h1=',h1
    print 'h2=',h2
    
    import benderPiRoot 

    plotter1 = benderPiRoot.plotter
    plotter1.plot ( h1 ) 
    
    import benderPiHippo
    plotter2 = benderPiHippo.plotter 
    plotter2.plot ( h1 )
    plotter2.plot ( h2 )
    
    import benderROOT
    plotter3 = benderROOT.plotter
    plotter3.plot ( h2 ) 
    
    # for the interactive mode it is better to comment the last line
    # g.exit()
# =============================================================================

# =============================================================================
# The END 
# =============================================================================
