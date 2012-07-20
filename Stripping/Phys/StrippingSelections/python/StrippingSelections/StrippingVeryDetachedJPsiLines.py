'''
Module for construction of very detached JPsi --> MuMu stripping selections and lines

Exported symbols (use python help!):
     - ..
'''

__author__ = ['Johannes Albrecht']
__date__ = '20/07/2011'
__version__ = '$Revision: 1.0 $'

__all__ = ('VDetJPsiLinesConf',
           'makeVDetJPsi'
           )

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
#from StrippingSelections.Utils import checkConfig

class VDetJPsiLinesConf(LineBuilder) :
    """
    Builder of:
       ...
    

    Usage:
    >>> config = { .... }
    >>> bsConf = Bs2MuMuLinesConf('PrescaledBs2MuMuTest',config)
    >>> bsLines = bsConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.


    Exports as instance data members:
    selDefault     : nominal Bs2mumu stripping line
    selLoose       : loose Bs2MuMu stripping line to understand systematics
    defaultLine    : Stripping line made from selDefault
    looseLine      : Stripping line made from selLoose
    lines          : lsit of lines:  [ defaultLine, looseLine ]
    
    Exports as class data member:
    Bs2MuMuLinesConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('VDetJPsiLinePrescale',
                              'VDetJPsiLinePostscale'
                              )
    
    #### This is the dictionary of all tunable cuts ########
    config_default={
        'VDetJPsiLinePrescale'    : 1,
        'VDetJPsiLinePostscale'   : 1,
        }                
    
    


    def __init__(self, 
                 name = 'VDetJPsi',
                 config = None) :

        LineBuilder.__init__(self, name, config)

        default_name=name

        self.selDefault = makeDefault(default_name)

        self.defaultLine = StrippingLine(default_name+"Line",
                                            prescale = config['VDetJPsiLinePrescale'],
                                            postscale = config['VDetJPsiLinePostscale'],
                                            algos = [ self.selDefault ]
                                            )
        
      
        self.registerLine( self.defaultLine )



def makeDefault(name) :
    """
    very detached JPSi selection
    """
    from Configurables import OfflineVertexFitter
    Detached2mu = CombineParticles("Combine"+name)
    Detached2mu.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
    Detached2mu.addTool( OfflineVertexFitter )
    Detached2mu.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    Detached2mu.OfflineVertexFitter.useResonanceVertex = False
    Detached2mu.ReFitPVs = True
    Detached2mu.DaughtersCuts = { "mu+" : "(TRCHI2DOF < 4 ) "\
                                  " & (MIPCHI2DV(PRIMARY)> 100)"\
                                  "& (PT > 0.7) "}
                                 
    Detached2mu.CombinationCut = " (AMAXDOCA('')<0.1*mm)"
    #"(ADAMASS('B_s0')<1000*MeV) "\
    Detached2mu.MotherCut = "(VFASPF(VCHI2/VDOF)<15) "\
                            "& (M>1500)"\
                            "& (BPVDIRA > 0) "\
                            "& (BPVVDCHI2>625)"\
                            "& (BPVVD>50)"

           
    

    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")

    return Selection (name,
                      Algorithm = Detached2mu,
                      RequiredSelections = [ _stdLooseMuons ])


