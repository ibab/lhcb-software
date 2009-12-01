#!/usr/bin/env python
# =============================================================================
# $Id: RealMip.py,v 1.1 2009-12-01 12:49:24 ibelyaev Exp $
# =============================================================================
# @file BenderExampel/RealMio.py
# An attempt to find mip on real data 
# @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
# @date 2009-11-25
# ============================================================================
"""

An attempt to find mip on real data 

"""
# ============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# ============================================================================
## import all needed stuff:
import ROOT                           ## needed to produce/visualize the histograms

from   Bender.Main         import *   ## import the ba
import LHCbMath.Types                 ## easy access to various geometry routines 

from   Gaudi.Configuration import *   ## needed for job configuration

from   GaudiKernel.SystemOfUnits import GeV
from   LoKiCore.functions        import doubles ## list -> std::vector<double>

import CaloUtils.CellID

import math

## namespace Gaudi
Gaudi = cpp.Gaudi
LHCb  = cpp.LHCb
ICaloDigits4Track = cpp.ICaloDigits4Track

## get the total energy of digits 
def ene ( digits ) :
    """
    get the total energy of digits 
    """
    e = 0
    
    for d in digits : e += d.e()
    
    return e

# =============================================================================
## @class RealMip
#  An attempt to find Mip on real data
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-11-25
class RealMip(Algo) :
    """
    
    An attempt to find Mip on real data
    
    """
    
    def initialize ( self ) :
        
        
        sc = Algo.initialize ( self )
        if sc.isFailure() : return sc
        
        self.tEcal = self.tool( ICaloDigits4Track , 'EcalEnergyForTrack:PUBLIC' ) 
        self.tHcal = self.tool( ICaloDigits4Track , 'HcalEnergyForTrack:PUBLIC' ) 
        self.tPrs  = self.tool( ICaloDigits4Track , 'PrsEnergyForTrack:PUBLIC'  ) 

        return SUCCESS

    
    ## the major analysis method 
    def analyse  (self ) :
        """
        The major analysis method
        """


        tup = self.nTuple ( 'RealMip')

        
        ## get tracks and count them!
        
        tracks = self.get('Rec/Track/Best')
        cnt    = self.counter('#tracks')
        cnt   += tracks.size()
        
        self.plot ( tracks.size() , '#tracks' , 0 , 50 )
        
        ## tup.column ( 'nTracks' , tracks.size() )

        
        nE = []
        eE = []
        nH = []
        eH = []
        nP = []
        eP = []
        
        ## loop over all tracks
        for track in tracks :
            
            
            nEcal , eEcal = self.treat ( track , self.tEcal )
            nHcal , eHcal = self.treat ( track , self.tHcal )
            nPrs  , ePrs  = self.treat ( track , self.tPrs  )
            
            nE += [ float(nEcal) ]
            nH += [ float(nHcal) ]
            nE += [ float(nPrs)  ]
            
            eE += [ eEcal ]
            eH += [ eHcal ]
            eE += [ ePrs  ]

            ## energy in Calorimeters 
            self.plot ( eEcal , 'e Ecal' , 0 , 4000 ,  50 )
            self.plot ( eHcal , 'e Hcal' , 0 , 4000 ,  50 )
            self.plot ( ePrs  , 'e Prs'  , 0 ,  100 ,  50 )

            ## number of fired cells in Calorimeters 
            self.plot ( nEcal , 'n Ecal' , 0 , 50 ,  50 )
            self.plot ( nHcal , 'n Hcal' , 0 , 50 ,  50 )
            self.plot ( nPrs  , 'n Prs'  , 0 , 50 ,  50 )

            ## energy in Calorimeters 
            if nEcal > 0 : self.plot ( eEcal , 'e1 Ecal' , 0 , 4000 ,  50 )
            if nHcal > 0 : self.plot ( eHcal , 'e1 Hcal' , 0 , 4000 ,  50 )
            if nPrs  > 0 : self.plot ( ePrs  , 'e1 Prs'  , 0 ,  100 ,  50 )

            tup.column ( 'eEcal' , float( eEcal )  )
            tup.column ( 'eHcal' , float( eHcal )  )
            tup.column ( 'ePrs'  , float( ePrs  )  )

            tup.column ( 'nEcal' , int ( nEcal )  )
            tup.column ( 'nHcal' , int ( nHcal )  )
            tup.column ( 'nPrs'  , int ( nPrs  )  )

            slopes = track.slopes()            
            tup.column ( 'tx' , slopes.x() )
            tup.column ( 'ty' , slopes.y() )

            tup.write()

            tx = slopes.x()
            ty = slopes.y()            
            r2 = tx*tx+ty*ty
            theta = math.sqrt( r2 / ( 1 + r2 ) )
            if nEcal > 0 and nPrs > 0 and ePrs < 10 and theta > 0.080 :
                self.plot ( eEcal , 'mip in Ecal' , 0 , 4000 ,  50 )

                


        return SUCCESS 
    
    def treat ( self , track , tool ) :
        """
        Get Calorimeter information for the given track 
        """

        digits = LHCb.CaloDigit.Set()
        
        line = Gaudi.Math.XYZLine( track.position() , track.slopes() )
     
        sc = tool.collect ( line , digits )
        
        if sc.isFailure() : return (-1,0.0)
        
        return ( digits.size() , ene ( digits ) )
    
                          

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
    
    # The main configurable 
    davinci = DaVinci (
        DataType      = '2009' ,             ## NB:    2k+9
        Simulation    = False  ,             ## NB:   *REAL DATA*
        EvtMax        = -1     ,             ## all events in the input files 
        HistogramFile = 'RealMip_Histos.root' 
        )
    
    EventSelector (
        PrintFreq = 100       , 
        Input     = datafiles 
        )
    
    FileCatalog (
        Catalogs = [ 'xmlcatalog_file:RealDataRec2.xml' ]
        )
    
    NTupleSvc (
        Output = [
        "MIP DATAFILE='RealMip.root' TYPE='ROOT' OPT='NEW'"
        ]
        )


    from Configurables import EcalEnergyForTrack , HcalEnergyForTrack , PrsEnergyForTrack

    for t in ( EcalEnergyForTrack , HcalEnergyForTrack , PrsEnergyForTrack ) : 
        
        t ( AddNeighbours = 1 , PropertiesPrint = True )


    ## end of static configurtaion
    
    ## instantiate application manager
    gaudi = appMgr()

    
    ## start of dynamic configuration 
    
    ## instantiate my algorithm
    alg = RealMip(
        'Mip'        ,
        NTupleLUN = 'MIP' 
        )
    
    gaudi.setAlgorithms ( [ alg ] ) 
    

    return SUCCESS 


# =============================================================================
## the actual job steering     
if '__main__' == __name__ :
    
    print __doc__
    print __author__
    print __version__
    
    input   = [
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005648/0000/00005648_00000002_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005648/0000/00005648_00000004_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005648/0000/00005648_00000005_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005648/0000/00005648_00000006_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005648/0000/00005648_00000007_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005648/0000/00005648_00000008_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005648/0000/00005648_00000009_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005648/0000/00005648_00000010_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005648/0000/00005648_00000011_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005648/0000/00005648_00000012_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005648/0000/00005648_00000013_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005649/0000/00005649_00000002_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005649/0000/00005649_00000003_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005650/0000/00005650_00000001_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005650/0000/00005650_00000003_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005650/0000/00005650_00000004_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005650/0000/00005650_00000005_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005650/0000/00005650_00000006_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005651/0000/00005651_00000001_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005652/0000/00005652_00000001_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005653/0000/00005653_00000001_1.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'"
        ]

    configure ( input )

    gaudi = appMgr()

    run ( -1 )

    import GaudiPython.HistoUtils
    
    alg = gaudi.algorithm ( 'Mip' )
    
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
