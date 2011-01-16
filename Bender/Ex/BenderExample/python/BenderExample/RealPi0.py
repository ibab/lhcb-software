#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
# @file BenderExampele/RealPi0.py
#
# Attempt to recontruct pi0 on real data
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

Attempt to recontruct pi0 on real data

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
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2009-11-25 "
__version__ = " CVS tag $Name: not supported by cvs2svn $, version $Revision$ "
# ============================================================================
## import all needed stuff:
import ROOT                           ## needed to produce/visualize the histograms
from   Bender.Main         import *   ## import the ba
import LHCbMath.Types                 ## easy access to various geometry routines 
from   Gaudi.Configuration import *   ## needed for job configuration

from   GaudiKernel.SystemOfUnits import GeV, MeV
from   LoKiCore.functions        import doubles ## list -> std::vector<double>
import CaloUtils.CellID
## namespace Gaudi
Gaudi = cpp.Gaudi

Photons = LoKi.Photons

fitPi0 = None
getPi0 = None

try :
    
    from KaliCalo.Pi0HistoFit import fitPi0, getPi0Params
    
except ImportError :
    
    fitPi0 = None
    getPi0 = None

    
# =============================================================================
## @class RealPi0
#  Simple Bender-based algorithm to reconstruct pi0 on real data 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-11-25
class RealPi0(Algo) :
    """
    Simple Bender-based algorithm to reconstruct pi0 on real data 
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
        
        clusters = self.get ( 'Rec/Calo/EcalClusters' )
        photons  = self.get ( 'Rec/Calo/Photons'      )
        protos   = self.get ( 'Rec/ProtoP/Neutrals'   )

        gammas = self.select('g' , 'gamma' == ID  )

        tup = self.nTuple('RealPi0')
        
        pi0s = self.loop( ' g g ', 'pi0')
        for pi0 in pi0s :

            m12 = pi0.mass(1,2) 
            if m12 > 1.0 * GeV : continue

            if m12 < 500 * MeV :
                self.plot ( m12 , 'gamma gamma 2' , 0 , 500 * MeV , 250 )
            if m12 < 250 * MeV :
                self.plot ( m12 , 'gamma gamma 3' , 0 , 250 * MeV , 250 )
                    
            g1 = pi0(1)
            g2 = pi0(2)

            prs1 = Photons.energyFrom     ( g1 , 'Prs') 
            prs2 = Photons.energyFrom     ( g2 , 'Prs') 

            spd1 = Photons.seedEnergyFrom ( g1 , 'Spd') 
            spd2 = Photons.seedEnergyFrom ( g2 , 'Spd') 

            p = pi0.momentum(1,2)
            
            tup.column ( 'm12'  , m12           )
            tup.column ( 'p'    , p             )
            tup.column ( 'g1'   , g1.momentum() )
            tup.column ( 'g2'   , g2.momentum() )
            tup.column ( 'pt1'  , PT ( g1 ) )
            tup.column ( 'pt2'  , PT ( g2 ) )

            tup.column ( 'prs1' , prs1 )
            tup.column ( 'prs2' , prs2 )
            
            tup.column ( 'spd1' , spd1 )
            tup.column ( 'spd2' , spd2 )

            tup.write()
            
            if   m12 < 250 * MeV  and max ( prs1 ,prs2 ) < 10 :
                self.plot ( m12 , 'gamma gamma 4 - LL' , 0 , 250 *MeV  , 250 )
            elif m12 < 250 * MeV  and min ( prs1 ,prs2 ) > 10 :
                self.plot ( m12 , 'gamma gamma 4 - GG' , 0 , 250 *MeV  , 250 )
            elif m12 < 250 * MeV  :
                self.plot ( m12 , 'gamma gamma 4 - LG' , 0 , 250 *MeV  , 250 )
          
        return SUCCESS 

    ## finalize & print histos 
    def finalize ( self ) :
        """
        Finalize & print histos         
        """
        histos = self.Histos()
        for key in histos :
            h = histos[key]
            if hasattr ( h , 'dump' ) :
                print h.dump(50,30,True)
                if fitPi0 and getPi0Params : 
                    fitPi0( h )
                    print 'FIT pi0:' , getPi0Params ( h )
                    
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
    
    from Configurables           import DaVinci
    
    davinci = DaVinci (
        DataType     = '2009' ,             ## NB:    2k+9
        Simulation   = False  ,             ## NB:   *REAL DATA*
        HistogramFile = 'RealPi0_Histos.root' ,
        ##
        )
    
    
    from GaudiConf.Configuration import NTupleSvc  
    NTupleSvc (
        Output = [
        "PI0 DATAFILE='RealPi0_Tuples.root' TYPE='ROOT' OPT='NEW'"
        ]
        )
    
    ## define the input data:
    setData ( datafiles , catalogs )
    
    ##
    ## 2. Jump into the wonderful world of the actual Gaudi components!
    ## 
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## instantiate my algorithm
    alg = RealPi0(
        'RealPi0'        ,
        NTupleLUN = 'PI0' ,
        InputLocations = [ 'StdLooseAllPhotons' ] ,
        ##
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

    import BenderExample.Data2009Reco07
    
    ##
    ## "regular data"
    ##
    
    from Gaudi.Configuration import EventSelector 
    files  = EventSelector().Input
    files.reverse() 
    configure ( files )
    
    ##  
    ##   few "special" runs 
    ##
    
##     prefix = '/castor/cern.ch/grid/lhcb/data/2009/DST/'
##     _files = [ '00005845/0000/00005845_00000057_1.dst' ,
##                '00005845/0000/00005845_00000058_1.dst' ,
##                '00005844/0000/00005844_00000003_1.dst' ,
##                '00005845/0000/00005845_00000049_1.dst' ]
##     files = [ prefix + f for f in _files ]
##     configure  (files ) 
    
    run ( 1000 )
    
# ============================================================================
# The END 
# ============================================================================
