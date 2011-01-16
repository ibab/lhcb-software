#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
# @file BenderExample/RealMip.py
#
# An attempt to find mip on real data
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
# @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
# @date 2009-11-25
# ============================================================================
"""

An attempt to find mip on real data

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# ============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__date__    = " 2009-11-25 "
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision$"
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
        
        
        nE = []
        eE = []
        nH = []
        eH = []
        nP = []
        eP = []
        
        ## loop over all tracks
        for track in tracks :
            
            if track.p() < 3 * GeV             : continue
            if LHCb.Track.Long != track.type() : continue 
            
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
            if nEcal > 0 and eEcal < 4000 :
                self.plot ( eEcal , 'e1 Ecal' , 0 , 4000 ,  50 )
            if nHcal > 0 and eHcal < 4000 :
                self.plot ( eHcal , 'e1 Hcal' , 0 , 4000 ,  50 )
            if nPrs  > 0 and ePrs  <  100 :
                self.plot ( ePrs  , 'e1 Prs'  , 0 ,  100 ,  50 )

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
        
        if not track : return (-1,0.0)
        
        if hasattr ( track , 'proto' ) : return self.treat ( track.proto() , tool )
        if hasattr ( track , 'track' ) : return self.treat ( track.track() , tool )

        digits = LHCb.CaloDigit.Set()

        sc = tool.collect ( track , digits )
        
        if sc.isFailure() : return (-1,0.0)
        
        return ( digits.size() , ene ( digits ) )
    
    ## finalize & print histos 
    def finalize ( self ) :
        """
        Finalize & print histos         
        """
        histos = self.Histos()
        for key in histos :
            h = histos[key]
            if hasattr ( h , 'dump' ) : print h.dump(50,30,True)
        return Algo.finalize ( self )                      

# =============================================================================
## the actual job configuration
def configure ( datafiles , catalogs = [] ) :
    """
    The actual job configuration
    """
    
    ##
    ## 1. Static configuration using "Configurables"
    ##

    from Configurables   import DaVinci
    davinci = DaVinci (
        DataType      = '2009' ,             ## NB:    2k+9
        Simulation    = False  ,             ## NB:   *REAL DATA*
        HistogramFile = 'RealMip_Histos.root' 
        )

    from GaudiConf.Configuration import NTupleSvc
    NTupleSvc (
        Output = [
        "MIP DATAFILE='RealMip.root' TYPE='ROOT' OPT='NEW'"
        ]
        )

    from Configurables import ( EcalEnergyForTrack ,
                                HcalEnergyForTrack ,
                                PrsEnergyForTrack  ) 
    
    for t in ( EcalEnergyForTrack ,
               HcalEnergyForTrack ,
               PrsEnergyForTrack  ) : 
        
        t ( AddNeighbours   = 1                         ,
            Extrapolator    = 'TrackMasterExtrapolator' , 
            PropertiesPrint = True                      )

    ##
    ## 2. Jump into the wonderful world of the actual Gaudi components!
    ## 
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = RealMip(
        'Mip'        ,
        NTupleLUN = 'MIP' 
        )
    
    gaudi.setAlgorithms ( [ alg ] ) 
    
    return SUCCESS 

# =============================================================================
## the actual job steering     
if '__main__' == __name__ :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' dir(%s) : %s ' % ( __name__    , dir() )
    print '*'*120  

    ##
    ## "regular data"
    ##
    import BenderExample.Data2009Reco07    
    from Gaudi.Configuration import EventSelector 
    files  = EventSelector().Input
    files.reverse() 
    configure ( files )

    run ( 2000 )
    
# ============================================================================
# The END 
# ============================================================================
