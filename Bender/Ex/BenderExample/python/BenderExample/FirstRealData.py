#!/usr/bin/env python
# =============================================================================
# $Id: FirstRealData.py,v 1.2 2010-01-12 10:58:34 ibelyaev Exp $
# =============================================================================
# @file BenderExampele/FirstRealData.py
#
# Bender-based version of the original
#  GaudiPython/AnalysisPython-based script written
# by Juan Palacious for analysis of real data from the first
# collsions in November 2k+9
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
# @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
# @date 2009-11-25
# ============================================================================
"""

Bender-based version of the original GaudiPython/AnalysisPython-based
script written by Juan Palacious for analysis of real data from the first
collsions in November 2k+9

"""
# ============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# ============================================================================
## import all needed stuff:
import ROOT                           ## needed to produce/visualize the histograms

from   Bender.Main         import *   ## import the ba
import LHCbMath.Types                 ## easy access to various geometry routines 

from   Gaudi.Configuration import *   ## needed for job configuration

from   GaudiKernel.SystemOfUnits import GeV
from   LoKiCore.functions        import doubles ## list -> std::vector<double>

## namespace Gaudi
Gaudi = cpp.Gaudi

# =============================================================================
## @class RealData
#  Simple Bender-based algorithm tp 'analyse' the first real data
#  the idea has been stollen from Juan Palacious
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-11-25
class RealData(Algo) :
    """
    Simple Bender-based algorithm tp 'analyse' the first real data
    the idea has been stollen from Juan Palacious
    """

    def initialize ( self ) :

        
        sc = Algo.initialize ( self )
        if sc.isFailure() : return sc 

        self.beamLine = Gaudi.Math.XYZLine ( Gaudi.Math.XYZPoint (0,0,0) ,
                                             Gaudi.Math.XYZVector(0,0,1) )

        return SUCCESS

    
    ## the major analysis method 
    def analyse  (self ) :
        """
        The major analysis method
        """


        tup = self.nTuple ( 'RealData')

        
        ## get tracks and count them!
        
        tracks = self.get('Rec/Track/Best')
        cnt    = self.counter('#tracks')
        cnt   += tracks.size()
        
        self.plot ( tracks.size() , '#tracks' , 0 , 50 )
        
        tup.column ( 'nTracks' , tracks.size() )
        

        zs = [] 
        ## loop over all tracks
        for track in tracks :
            
            trackLine = Gaudi.Math.XYZLine( track.position() , track.slopes() )

            doca = trackLine.distance( self.beamLine )

            self.plot ( doca , 'distance to beamline' , 0 , 500 ) 

            trackPoint, beamPoint , flag = trackLine.closestPoints ( self.beamLine )
            if flag :
                self.plot ( trackPoint.z() , 'Z of track at POCA', -5000, 5000, 200)
                zs += [ trackPoint.z() ] 

        tup.farray ( 'zPoca' , doubles( zs ) , 'nT' , 200 )

        
        ## get Calo info:
        caloE  = {}  ## energy in Calorimeter            (in GeV) 
        caloET = {}  ## transverse energy in calorimeter (in GeV) 
        caloN  = {}  ## number of digits 
        
        for det in ( 'Spd' , 'Prs' , 'Ecal' , 'Hcal' ) :

            # get digits and count them 
            digits = self.get( 'Raw/' + det + '/Digits' )

            dd = self.getDet('/dd/Structure/LHCb/DownstreamRegion/' + det )
            
            e  = 0 
            eT = 0 
            for d in digits :
                e  += d.e() / GeV 
                eT += d.e() * dd.cellSine( d.cellID() ) / GeV 

            e  = e 
            eT = eT  
            caloE  [ det ] = e
            caloET [ det ] = eT 
            caloN  [ det ] = digits.size()

            
        for det in ( 'Spd' , 'Prs' , 'Ecal' , 'Hcal' ) :

            n  = int   ( caloN  [ det ] ) 
            e  = float ( caloE  [ det ] ) 
            eT = float ( caloET [ det ] ) 
            
            cnt  = self.counter('#  ' + det )
            cnt += n
            
            cnt  = self.counter('e  ' + det )
            cnt += e
            
            cnt  = self.counter('eT ' + det )
            cnt += eT 
            
            tup.column ('e'  + det , e  ) 
            tup.column ('eT' + det , eT ) 
            tup.column ('n'  + det , n  )
            
            
        tup.write()
        
        if caloET[ 'Ecal' ] > 3 :
            
            for z in zs : self.plot ( z , 'zPoca for eT(Ecal)>3 GeV ' , -5000 , 5000 , 200 )
            
        else :
            
            for z in zs : self.plot ( z , 'zPoca for eT(Ecal)<3 GeV' , -5000 , 5000 , 200 )
            
        
        return SUCCESS 


# =============================================================================
## the actual job configuration
def configure ( datafiles ) :
    """
    The actual job configuration
    """
    from Configurables           import DaVinci       ## needed for job configuration
    from Configurables           import EventSelector ## needed for job configuration 
    from GaudiConf.Configuration import FileCatalog   ## needed for job configuration 
    from GaudiConf.Configuration import NTupleSvc     ## needed for job configuration 
    
    # the main configurable 
    davinci = DaVinci (
        DataType   = '2009' ,             ## NB:    2k+9
        Simulation = False  ,             ## NB:   *REAL DATA*
        EvtMax     = -1     ,             ## all eventts in the input files 
        )
    
    EventSelector (
        PrintFreq = 10        , 
        Input     = datafiles 
        )
    
    FileCatalog (
        Catalogs = [ 'xmlcatalog_file:FirstBeamData2009.xml' ]
        )
    
    NTupleSvc (
        Output = [
        "FRD DATAFILE='FirstRealData.root' TYPE='ROOT' OPT='NEW'"
        ]
        )
 
    
    ## end of static configurtaion
    
    ## instantiate application manager
    gaudi = appMgr()

    
    ## start of dynamic configuration 
    
    ## instantiate my algorithm
    alg = RealData(
        'RealData'        ,
        NTupleLUN = 'FRD' 
        )
    
    gaudi.setAlgorithms ( [ alg ] ) 
    

    return SUCCESS 


# =============================================================================
## the actual job steering     
if '__main__' == __name__ :
    
    print __doc__
    print __author__
    print __version__
    
    beamgas = [
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005616/0000/00005616_00000002_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005616/0000/00005616_00000003_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005616/0000/00005616_00000005_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005616/0000/00005616_00000006_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005616/0000/00005616_00000007_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005616/0000/00005616_00000008_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005616/0000/00005616_00000009_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005616/0000/00005616_00000010_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'"
        ]

    collisions = [
        "DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005616/0000/00005616_00000011_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'"
        ]

    configure ( collisions )
    ## configure ( beamgas )

    gaudi = appMgr()

    run ( -1 )

    import GaudiPython.HistoUtils
    
    alg = gaudi.algorithm ( 'RealData' )
    
    histos = alg.Histos()
    for key in histos :
        histo = histos[key]
        if hasattr ( histo , 'dump' ) :
            print histo.dump( 40 , 20 )
        if hasattr ( histo , 'Draw' ) :  
            histo.Draw(  histo.title() + '.gif' )
            histo.Draw(  histo.title() + '.eps' )
            
    
# ============================================================================
# The END 
# ============================================================================
