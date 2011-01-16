#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
# @file BenderExampele/RealPi0.py
#
# Attempt to recontruct pi0 on real data using calibrated femto-DST
#
#  - Prs-coefficients are taken from Olivier Deschamps
#  - Ecal coefficients are extracted using Kali
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

Attempt to recontruct pi0 on real data using calibrated femto-DST

- Prs-coefficients are taken from Olivier Deschamps
- Ecal coefficients are extracted using Kali

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


## import the algorithm 
from BenderExample.RealPi0 import RealPi0 


# =============================================================================
## configure for reading femto-DST 
def configure_for_fmDST () :
    """
    
    Configure for reading femto-DST
    
    """

    from Configurables import DataOnDemandSvc
    dod = DataOnDemandSvc  ( Dump = True ) 
    
    ## re-reconstruct Photons 
    from Configurables import OffLineCaloRecoConf
    calo = OffLineCaloRecoConf ( ForceDigits = False )
    calo.RecList    += [ "Photons" ]
    calo.RecList.remove ( "Digits" )
    
    dod.AlgMap['Rec/Calo/Photons'] = calo.photons () 
    
    ## re-reconstruct NeutralProtoP
    from Configurables import NeutralProtoPAlg
    dod.AlgMap['Rec/ProtoP/Neutrals'] =  NeutralProtoPAlg (
        "NeutralProtoPMaker"  , 
        LightMode      = True ,                  ## "light-mode", no PIDs
        HyposLocations = [ 'Rec/Calo/Photons']   ## only single photons
        )

    from CommonParticles.StdLooseAllPhotons import StdLooseAllPhotons as Gamma 
    Gamma.InputPrimaryVertices = 'None'

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
        EnableUnpack = False , ## IMPORTANT for femto-DST!!!
        )
    
    ## configure for femto-DST 
    configure_for_fmDST ()
    
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
        InputPrimaryVertices = 'None'
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

    ## use preprocessed fmDST
    configure (
        '/castor/cern.ch/user/i/ibelyaev/Kali/KaliPi0_Calib-02.fmDST'
        )
    
    run ( 10000 )
    
# ============================================================================
# The END 
# ============================================================================
