#!/usr/bin/env python
# =============================================================================
# $Id: RealPi0.py,v 1.4 2010-01-22 14:26:01 ibelyaev Exp $
# =============================================================================
# @file BenderExampele/RealPi0.py
# Attempt to recontruct pi0 on real data 
# @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
# @date 2009-11-25
# ============================================================================
"""

Attempt to recontruct pi0 on real data 

"""
# ============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
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

Photons = LoKi.Photons

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

            m12 = pi0.mass(1,2) / GeV 
            if m12 > 1.0 : continue

            if m12 <   1 : self.plot ( m12 , 'gamma gamma 1' , 0 ,   1 , 200 )
                
            if m12 < 0.5 : self.plot ( m12 , 'gamma gamma 2' , 0 , 0.5 ,  50 )

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

            if m12 < 0.5 and min ( prs1 ,prs2 ) > 20 :
                self.plot ( m12 , 'gamma gamma 2' , 0 , 0.5 ,  50 )

                      
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
        Catalogs = [ 'xmlcatalog_file:RealDataRec2.xml' ]
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
        UseSpd             = True  
        )
    
    from Configurables import NeutralProtoPAlg
    NeutralProtoPAlg (
        LightMode      = True ,                  ## "light-mode", no PIDs
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
    ## photon.PtCut              = 200
    
    photon.PropertiesPrint = True
    
    maker.InputPrimaryVertices = "None" ## NB: it saves a lot of CPU time 

    ## end of static configuration
    
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
        'NeutralProtoPAlg' , alg ] ) 
    

    return SUCCESS 


# =============================================================================
## the actual job steering     
if '__main__' == __name__ :
    
    print __doc__
    print __author__
    print __version__
    
    input = []
    
    configure ( input )
    
    gaudi = appMgr()

    evtSel = gaudi.evtsel()


    from BenderExample.JuanFiles2009 import files
    
    evtSel.open( files )
    
    run ( -1 )

    import GaudiPython.HistoUtils
    
    alg = gaudi.algorithm ( 'RealPi0' )
    
    histos = alg.Histos()
    for key in histos :
        histo = histos[key]
        if hasattr ( histo , 'dump' ) :
            print 'KEY:', key 
            print histo.dump( 50 , 20 )
        if hasattr ( histo , 'Draw' ) :  
            histo.Draw(  histo.title() + '.gif' )
            histo.Draw(  histo.title() + '.eps' )
        if 'gamma gamma 2' == key :
            
            from KaliCalo.Pi0HistoFit import fitPi0Histo, pi0FitFun
            
            print fitPi0Histo ( histo , 0.050 , 0.300 ) 
    
    
# ============================================================================
# The END 
# ============================================================================
