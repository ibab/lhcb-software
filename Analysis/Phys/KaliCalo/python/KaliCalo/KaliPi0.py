#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: KaliPi0.py,v 1.2 2009-09-18 09:55:11 ibelyaev Exp $ 
# =============================================================================
## @file  KaliCalo/KaliCaloPi0.py
#  The basic configuration to (re)run Ecal pi0-calibration
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2009-09-17
# =============================================================================
"""
The basic configuration to (re)run Ecal pi0-calibration

One reads the input data (presumably DST), (re)recontruct Calorimeter,
and produces twomajor outputs:

 -  root file with N-tuple for 'pi0-calibration'
 - 'femtoDST' which contains only 'interesting' digits

Usage:
 
  > gaudirun.py KaiPi0.py InputDataFiles.py

Or (suitable for tests)

  > ./KaliPi0.py


"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================

## the basic import 
from Gaudi.Configuration import  *

from Configurables import OutputStream, GaudiSequencer 

## the major configuration:

from Configurables import OffLineCaloRecoConf
from Configurables import OffLineCaloPIDsConf

OffLineCaloRecoConf (
    EnableRecoOnDemand = True  ,
    UseTracks          = False 
    )

OffLineCaloPIDsConf (
    EnablePIDsOnDemand = True 
    )

## miscalibrate Ecal:
from Configurables import Kali__MisCalibrateCalo

misKali = Kali__MisCalibrateCalo (
    "KaliEcal" ,
    Coefficients = {
    10   : 1.01  ,
    1124 : 10.02 ,
    1824 : 0.01  
    }
    )
## specific features:
from Configurables import NeutralProtoPAlg

## use "light-mode" 
proto = NeutralProtoPAlg (
    LightMode = True ,
    HyposLocations = [ 'Rec/Calo/Photons'] 
    )

## redefine a bit the rules for photon creation: 
from Configurables import PhotonMakerAlg, PhotonMaker, PhysDesktop 

## create the algorithm 
maker =  PhotonMakerAlg (
    'StdLooseAllPhotons'      ,
    DecayDescriptor = 'Gamma' )

maker.addTool ( PhotonMaker , name = 'PhotonMaker' )
photon = maker.PhotonMaker
photon.UseCaloTrMatch     = False
photon.UseCaloDepositID   = False
photon.UseShowerShape     = False
photon.UsePhotonID        = False
photon.ConvertedPhotons   = True 
photon.UnconvertedPhotons = True 
photon.PtCut              = 250 
maker.addTool ( PhysDesktop )
maker.PhysDesktop.InputPrimaryVertices = "None" ## important: it saves a lot of CPU time 

 
## "the main" algorithm 
from Configurables import LoKi__Kali_Pi0 as Kali

kali = Kali (
    "Kali"             ,
    NTupleLUN  = "PI0" ,
    HistoPrint = True  ,
    InputLocations = [ 'StdLooseAllPhotons' ]
    )
kali.addTool ( PhysDesktop )
desktop = kali.PhysDesktop
desktop.InputPrimaryVertices = "None"    ## important: it saves a lot of CPU time!

kaliSeq  = GaudiSequencer  (
    'KaliSeq',
    Members = [
    ## misKali  ,    ## (mis)calibrate Ecal 
    proto    ,    ## (re)create Neutral ProtoParticles (implies also re-reconstruct Calo!)
    maker    ,    ## (re)make photons 
    kali     ]    ## analyse pi0 peak 
    ) 

from Configurables import DaVinci

daVinci = DaVinci (
    DataType       = 'MC09' , # default  
    Simulation     = True   ,
    HltType        = ''     ,
    EvtMax         = 1000   ,
    UserAlgorithms = [ kaliSeq ] 
    ) 

from Configurables import NTupleSvc, HistogramPersistencySvc
HistogramPersistencySvc ( OutputFile = 'KaliPi0_Histos.root' ) 
NTupleSvc ( Output = [ "PI0 DATAFILE='KaliPi0_Tuples.root' TYPE='ROOT' OPT='NEW'"] )


writer = OutputStream (
    'WRITER', 
    ItemList =  [
    "/Event#1"            ,
    "/Event/DAQ#1"        ,
    "/Event/DAQ/ODIN#1"   ,
    #
    "/Event/Rec#1"        ,
    "/Event/pRec#1"       ,
    #
    "/Event/Raw#1"        ,
    "/Event/Raw/Spd#1"    ,
    "/Event/Raw/Prs#1"    ,
    "/Event/Raw/Ecal#1"   ,
    "/Event/Raw/Hcal#1"   ,
    "/Event/Raw/Spd/Digits#1"  ,
    "/Event/Raw/Prs/Digits#1"  ,
    "/Event/Raw/Ecal/Digits#1" ,
    "/Event/Raw/Hcal/Digits#1" 
    ] ,
    Output = "DATAFILE='PFN:KaliPi0.fdst' TYP='POOL_ROOTTREE' OPT='REC'" ,
    RequireAlgs = [ "Kali" ] 
    )

am = ApplicationMgr( OutStream = [ writer ] )


## IMPORTANT: It saves a lot of CPU time!!!
def  action ( ) :
    
    dvinit = getConfigurable('DaVinciInitSeq')
    dvinit.Members = []

    
appendPostConfigAction ( action )
    

# =============================================================================
## the actual job steering 
if '__main__' == __name__ :
    
    print __doc__
    print __author__
    print __version__
    
    from Configurables import EventSelector
    
    #-- GAUDI jobOptions generated on Fri Jul 31 16:40:59 2009
    #-- Contains event types : 
    #--   30000000 - 98 files - 3911764 events - 110.64 GBytes
    
    EventSelector(
        Input   = [
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021324_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021330_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021335_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021346_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021348_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021349_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021350_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021353_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021357_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021360_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021362_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021363_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021364_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021367_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021368_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021372_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021374_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021376_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021378_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021379_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021380_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021382_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021384_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021385_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021386_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021387_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021389_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021390_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021392_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021395_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021398_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021400_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021401_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021402_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021404_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021410_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021413_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021417_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021418_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021421_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021423_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021426_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021431_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021436_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021438_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021474_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021844_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021848_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021855_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021863_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021868_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021874_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021879_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021886_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021895_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021899_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021906_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021910_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021917_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021920_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021925_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021933_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021946_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021951_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021954_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021963_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021966_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021974_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021981_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021984_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021993_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00021997_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00022000_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00022009_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00022017_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00022018_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00022026_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00022033_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00022037_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00022047_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00022053_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00022376_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00022465_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00022539_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00023770_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00023783_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00023784_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00023788_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00023791_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00023795_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00023798_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00023804_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00023808_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00023818_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00023821_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00023822_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00023827_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004838/0002/00004838_00023881_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
        )
    
    from GaudiPython.Bindings import AppMgr
    
    
    gaudi = AppMgr()
    
    ##evtSel = gaudi.evtSel()
    ##evtSel.open('output1.fdst')
    
    gaudi.run(1000)

    
    
# =============================================================================
# The END 
# =============================================================================
