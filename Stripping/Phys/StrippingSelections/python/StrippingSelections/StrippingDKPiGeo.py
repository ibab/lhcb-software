############################################################################
#
# Stripping selection for prompt D0->KPi events
# This version is for x-section measurements
# for the use in "private" stripping on 14/nb data for the
# open charm production task force
#
# author: Markward Britsch (markward@mpi-hd.mpg.de)
#
# result on 100000 events from a MagDown dst of the reco04stripping05 minibias line:
#
# StrippingReport                  INFO Decision name                                      :     Rate   Accepted   Mult.  <T>,ms
# StrippingReport                  INFO -- StrippingDKPiGeoLine                            : 0.007830        783   1.386
# 
#
############################################################################

#__author__ = ['Markward Britsch']
#__date__ = '21/09/2010'
#__version__ = '$Revision: 1.0 $'

__all__ = ('name', 'D0', 'sequence')

from Gaudi.Configuration import *
from Configurables import CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine, StrippingMember

name = "DKPiGeo"

_Preambulo  = [
    "PFUNA = LoKi.Particles.PFunA",
    ## define nu_2
    "NU_2 = log ( CHILD ( MIPDV ( PRIMARY ) , 1 ) * CHILD ( MIPDV ( PRIMARY ) , 2 ) / ( MIPDV ( PRIMARY )*MIPDV ( PRIMARY) + PFUNA(AMAXDOCA(''))*PFUNA(AMAXDOCA(''))  ) )  ",
    ## define nu_2pt
    "NU_2PT = log(NU_2 * CHILD ( PT , 1 ) * CHILD ( PT , 2 ))"
    #"massPlot = monitor ( M / MeV , Gaudi.Histo1DDef ( 'D0 mass' ,1700 , 2000 , 100 ) , 'D0 mass'     ) "
    ]

_D0DauCuts = { "": "ISLONG"  }
_D0CombCut = "(ADAMASS(1864.8*MeV)<250*MeV)"
_D0Cuts = "(ADMASS(1864.8*MeV)<125*MeV) & (NU_2 > 0) & (NU_2PT > 14) & (BPVVDSIGN>1*mm) & (PFUNA(AMAXDOCA(''))<10*mm)"

_D0 = CombineParticles( name,
                        DecayDescriptor = "[D0 -> K- pi+]cc",
                        Preambulo       = _Preambulo,
                        MotherCut = _D0Cuts,
                        DaughtersCuts = _D0DauCuts,
                        CombinationCut = _D0CombCut,

                        #Monitor       = True,
                        #HistoProduce  = True,
                        #HistoPrint    = True,
                        
                        #MotherMonitor = """
                        #process ( massPlot ) >> EMPTY
                        #"""
                      )

 
_stdNoPIDsKaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons" )
_stdNoPIDsPions = DataOnDemand(Location = "Phys/StdNoPIDsPions" )

D0 = Selection ( "Sel"+name,
                 Algorithm = _D0,
                 RequiredSelections = [_stdNoPIDsKaons,_stdNoPIDsPions]
               )


############################################
# Create StrippingLine with this selection #
############################################
line = StrippingLine(name+"Line"
                          , prescale = 1.
                          , algos = [ D0 ]
                          )
