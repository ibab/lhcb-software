########################################################################
#
# Options for Lambda Reconstruction algorithm
#
# @author Mathias Oleg Knecht  mathias.knecht@epfl.ch
# @date   2010-01-14
########################################################################
from os import environ
import GaudiKernel.SystemOfUnits as Units
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import DaVinci, CombineParticles
import CommonParticles.StdNoPIDsPions
import CommonParticles.StdNoPIDsProtons
import CommonParticles.StdNoPIDsDownPions
import CommonParticles.StdNoPIDsDownProtons
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from CommonParticles.Utils import *

importOptions("$PATPVROOT/options/PVLoose.py")
from Configurables import PatPVOffline
from Configurables import TESCheck, EventNodeKiller
PVCheck = TESCheck('PVCheck')
PVCheck.Inputs = ['Rec/Vertex']
eventNodeKiller = EventNodeKiller('PVkiller')
eventNodeKiller.Nodes = ['pRec/Vertex','Rec/Vertex']
removePV = GaudiSequencer( "RemovePV",
                          Members = [PVCheck, eventNodeKiller] )
redoPV = GaudiSequencer( "RedoPV",
                        IgnoreFilterPassed=True,
                        Members = [removePV, PatPVOffline("PatPVOffline")] )



LambdaAllCombineGeo = StrippingMember( CombineParticles
                                    , 'LambdaCombineAllGeo'    
                                    ,  InputLocations = [ "StdNoPIDsDownPions", "StdNoPIDsDownProtons","StdNoPIDsPions", "StdNoPIDsProtons" ]
                                    ,  DecayDescriptor = "[Lambda0 -> p+ pi-]cc"
                                       , CombinationCut = "ADAMASS('Lambda0')<20*MeV"
                                       , MotherCut = "(((CHILD(MIPDV(PRIMARY), 1) * CHILD(MIPDV(PRIMARY), 2) / MIPDV(PRIMARY))> 0.37*mm))"
                                    )

##########################################################
KsAllCombineGeo = StrippingMember( CombineParticles
                                    , 'KsCombineAllGeo'    
                                    ,  InputLocations = [ "StdNoPIDsDownPions", "StdNoPIDsDownProtons","StdNoPIDsPions", "StdNoPIDsProtons" ]
                                    ,  DecayDescriptor = "[KS0 -> pi+ pi-]cc"
                                    , CombinationCut = " ADAMASS('KS0')<100*MeV"
                                    , MotherCut = "(((CHILD(MIPDV(PRIMARY), 1) * CHILD(MIPDV(PRIMARY), 2) / MIPDV(PRIMARY))> 0.37*mm))"
                                    )
##########################################################################
line_lambda_all= StrippingLine('LambdaAllGeo'
                                , prescale = 1
                               , algos = [ ##redoPV,
    LambdaAllCombineGeo ])


line_KS_all= StrippingLine('KSAllGeo'
                                , prescale = 1
                               , algos = [ ##redoPV,
    KsAllCombineGeo ])
                           
############################################################################
