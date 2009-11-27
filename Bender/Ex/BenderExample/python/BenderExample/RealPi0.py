#!/usr/bin/env python
# =============================================================================
# $Id: RealPi0.py,v 1.1 2009-11-27 14:22:08 ibelyaev Exp $
# =============================================================================
# @file BenderExampele/RealPi0.py
#
# Attempt to recontruct pi0 on real data.
#
# The example illustrates:
#
#  - access to real data
#  - access to RAW  data
#  - configuration for "on-flight" Photon reconstruction
#  - the regular Bender algorith for study the di-photon combinations
#
# @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
# @date 2009-11-25
# ============================================================================
"""
Attempt to recontruct pi0 on real data.

The example illustrates:

  - access to real data
  - access to RAW  data
  - configuration for 'on-flight' Photon reconstruction
  - the regular Bender algorith for study the di-photon combinations
  
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

from   GaudiKernel.SystemOfUnits import GeV,MeV 
from   LoKiCore.functions        import doubles ## list -> std::vector<double>

## namespace Gaudi
Gaudi = cpp.Gaudi

# =============================================================================
## @class RealPi0
#  Simple Bender-based algorithm to reconstruct pi0 on real data 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-11-25
class RealPi0(Algo) :
    """
    Simple Bender-based algorithm to reconstruct pi0 on real data 
    """

    ## the major analysis method 
    def analyse  (self ) :
        """
        The major analysis method
        """

        ## check Calo recpontruction:
        
        clusters = self.get('Rec/Calo/EcalClusters')
        photons  = self.get('Rec/Calo/Photons')
        protos   = self.get('Rec/ProtoP/Neutrals')

        gammas = self.select('g' , 'gamma' == ID  )

        tup = self.nTuple('RealPi0')
        
        pi0s = self.loop( ' g g ', 'pi0')
        for pi0 in pi0s :

            m12 = pi0.mass(1,2) / GeV 
            if m12 > 1.0 : continue

            if m12 <   1 : self.plot ( m12 , 'gamma gamma 1' , 0 ,   1 , 200 )
                
            if m12 < 0.5 : self.plot ( m12 , 'gamma gamma 2' , 0 , 0.5 ,  50 )

            g1 = pi0(1)
            g2 = pi0(2)

            p = pi0.momentum(1,2)
            
            tup.column ( 'm12'  , m12           )
            tup.column ( 'p'    , p             )
            tup.column ( 'g1'   , g1.momentum() )
            tup.column ( 'g2'   , g2.momentum() )
            tup.column ( 'pt1'  , PT ( g1 ) )
            tup.column ( 'pt2'  , PT ( g2 ) )
            tup.write()
            
                      
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
        HistogramFile = 'RealPi0_Histos.root' ,
        EnableUnpack = False 
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
        "PI0 DATAFILE='RealPi0_Tuples.root' TYPE='ROOT' OPT='NEW'"
        ]
        )

    from Configurables import OffLineCaloRecoConf
    
    OffLineCaloRecoConf (
        EnableRecoOnDemand = True  ,
        UseTracks          = False ,        
        UseSpd             = False
        )
    
    from Configurables import NeutralProtoPAlg
    NeutralProtoPAlg (
        LightMode      = True ,                  ## "ligght-mode", no PIDs
        HyposLocations = [ 'Rec/Calo/Photons']   ## only single photons 
        )
    
    from Configurables import PhotonMaker, PhotonMakerAlg, PhysDesktop 
    
    maker =  PhotonMakerAlg (
        'StdLooseAllPhotons' ,
        DecayDescriptor = 'Gamma'
        )
    
    maker.addTool ( PhotonMaker , name = 'PhotonMaker' )
    photon = maker.PhotonMaker
    photon.UseCaloTrMatch     = False
    photon.UseCaloDepositID   = False
    photon.UseShowerShape     = False
    photon.UseClusterMass     = False
    photon.UsePhotonID        = False
    
    photon.ConvertedPhotons   = True 
    photon.UnconvertedPhotons = True 
    ## photon.PtCut              = 900 ## 2
    
    photon.PropertiesPrint = True
    
    maker.InputPrimaryVertices = "None" ## NB: it saves a lot of CPU time 
        
    ## end of static configurtaion
    
    ## instantiate application manager
    gaudi = appMgr()
    
    ## start of dynamic configuration 
    
    ## instantiate my algorithm
    alg = RealPi0(
        'RealPi0'        ,
        NTupleLUN = 'PI0' ,
        InputLocations = [ 'StdLooseAllPhotons' ] 
        )
    
    gaudi.setAlgorithms ( [
        'NeutralProtoPAlg'  ## recontruct Neutral ProtoParticles 
        , alg
        ] ) 
    
    return SUCCESS 


# =============================================================================
## the actual job steering     
if '__main__' == __name__ :
    
    print __doc__
    print __author__
    print __version__

    ## configure with no input data:
    configure ( [] )
    
    gaudi = appMgr()  

    phmaker = gaudi.tool ( 'StdLooseAllPhotons.PhotonMaker' )
    
    phmaker.PtCut           = 150 * MeV
    
    phmaker.PropertiesPrint = True 

    
    evtSel = gaudi.evtsel()
    
    mask = '/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/BEAM1/%d/0%d_0000000001.raw'
    
    runs = ( 62334 , 
             62406 ,
             62416 ,
             62426 , 
             62437 ,
             62462 ,
             62463 ,
             62500 , 
             62501 , 
             62502 , 
             62507 , 
             62508 ,
             62509 ,
             62512 ,
             62513 , 
             62514 , 
             62544 ,
             62548 ,
             62556 ,
             62558 ) 
    
    evtSel.open( [ mask % (r,r) for r in runs ] )
    
    run ( -1 )

    import GaudiPython.HistoUtils
    
    alg = gaudi.algorithm ( 'RealPi0' )
    
    histos = alg.Histos()
    for key in histos :
        histo = histos[key]
        if hasattr ( histo , 'dump' ) :
            print histo.dump( 50 , 20 )
        if hasattr ( histo , 'Draw' ) :  
            histo.Draw(  histo.title() + '.gif' )
            histo.Draw(  histo.title() + '.eps' )
            
    
# ============================================================================
# The END 
# ============================================================================
