#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
# @file BenderExampele/RealPi0.py
#
# Attempt to recontruct pi0 on real data using 
# private recalibration of Ecal/Prs
#
#  - Prs-coefficients are taken from Olivier Deschamps
#  - Ecal coefficients are extracted using Kali
# 
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
using private recalibration of Ecal/Prs:

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

from   GaudiKernel.SystemOfUnits import GeV, MeV
from   LoKiCore.functions        import doubles ## list -> std::vector<double>
import CaloUtils.CellID
## namespace Gaudi
Gaudi = cpp.Gaudi

Photons = LoKi.Photons

## import the algorithm 
from BenderExample.RealPi0 import RealPi0 

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
        EnableUnpack = False ,
        ## 
        )


    ## apply calibration coefficients to Ecal+Prs 
    import KaliCalo.Kali      as Kali
    import KaliCalo.ZipShelve as ZipShelve 
    db_location = '/afs/cern.ch/lhcb/group/calo/ecal/vol10/ECAL-2k+9/'
    dbase = ZipShelve.open( db_location + 'Ecal_2k+9-Loose-results-zdb','r')
    ecal = dbase['Ecal']
    prs  = dbase['Prs' ]

    cnt1 = Kali.Counter() 
    cnt2 = Kali.Counter() 
    for c in prs  :
        cnt1 += prs[c]
        print c , prs [c]
    for c in ecal :
        cnt2 += ecal[c]
        print c , ecal[c]
        
    print 'PRS_coefficients  : MEAN ( %.3f+-%.3f) '         % ( cnt1.flagMean    ()  ,
                                                                cnt1.flagMeanErr ()  )  
    print 'PRS_coefficients  : Min/Max/RMS %.3f/%.3f/%.3f ' % ( cnt1.flagMin () ,
                                                                cnt1.flagMax () ,
                                                                cnt1.flagRMS () )
    
    print 'ECAL_coefficients : MEAN ( %.3f+-%.3f) '         % ( cnt2.flagMean    ()  ,
                                                                cnt2.flagMeanErr ()  )  
    print 'ECAL_coefficients : Min/Max/RMS %.3f/%.3f/%.3f ' % ( cnt2.flagMin () ,
                                                                cnt2.flagMax () ,
                                                                cnt2.flagRMS () ) 
    from Configurables import Kali__MisCalibrateCalo
    davinci.UserAlgorithms += [
        Kali__MisCalibrateCalo ( 'KaliPrs'  , Coefficients = prs  ) ,
        Kali__MisCalibrateCalo ( 'KaliEcal' , Coefficients = ecal ) 
        ]
    
    ## re-reconstruct Photons 
    from Configurables import OffLineCaloRecoConf
    calo = OffLineCaloRecoConf ()
    calo.RecList += [ "Photons" ]
    davinci.UserAlgorithms += [ calo.photons() ] 
    
    ## re-reconstruct NeutralPtotoP
    from Configurables           import NeutralProtoPAlg
    davinci.UserAlgorithms += [
        NeutralProtoPAlg (
        "NeutralProtoPMaker"  , 
        ## LightMode      = True ,               ## "light-mode", no PIDs
        HyposLocations = [ 'Rec/Calo/Photons']   ## only single photons 
        )
        ]
    
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
        InputLocations = [ 'StdLooseAllPhotons' ] 
        )
    
    ##gaudi.setAlgorithms ( [ alg ] ) 
    gaudi.addAlgorithm ( alg  ) 
    
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
    
    run ( 5000 )
    
# ============================================================================
# The END 
# ============================================================================
