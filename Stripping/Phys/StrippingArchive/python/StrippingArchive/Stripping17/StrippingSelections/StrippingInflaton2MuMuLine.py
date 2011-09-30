'''
Module for construction of very very detached dimuon --> MuMu stripping selections and lines (for super speculative inflaton search)

Exported symbols (use python help!):
     - ..
'''

__author__ = ['Michel De Cian']
__date__ = '29/09/2011'
__version__ = '$Revision: 1.0 $'

__all__ = ('StrippingInflaton2MuMuConf',
           'makeLong',
           'makeDownstream'
           )

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
#from StrippingSelections.Utils import checkConfig

class StrippingInflaton2MuMuConf(LineBuilder) :
    """
    Builder of StrippingInflaton2MuMu:
       ...
    

    Usage:
    >>> config = { .... }
    >>> bsConf = Bs2MuMuLinesConf('PrescaledBs2MuMuTest',config)
    >>> bsLines = bsConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.
    
    """

    __configuration_keys__ = ('Inflaton2MuMuLongPrescale',
                              'Inflaton2MuMuDownstreamPrescale',
                              'Inflaton2MuMuLongPostscale',
                              'Inflaton2MuMuDownstreamPostscale'
                              )
    
    #### This is the dictionary of all tunable cuts ########
    config_default={
        'Inflaton2MuMuLongPrescale'    : 1,
        'Inflaton2MuMuDownstreamPrescale'    : 1,
        'Inflaton2MuMuLongPostscale'    : 1,
        'Inflaton2MuMuDownstreamPostscale'    : 1,
        }                
    
    


    def __init__(self, 
                 name = 'Inflaton2MuMu',
                 config = None) :

        LineBuilder.__init__(self, name, config)

        default_name=name

        self.selLong = makeLong(default_name+"long")
        self.selDownstream = makeDownstream(default_name+"downstream")

        self.longLine = StrippingLine(default_name+"LongLine",
                                            prescale = config['Inflaton2MuMuLongPrescale'],
                                            postscale = config['Inflaton2MuMuLongPostscale'],
                                            algos = [ self.selLong ]
                                            )

        self.downstreamLine = StrippingLine(default_name+"DownstreamLine",
                                            prescale = config['Inflaton2MuMuDownstreamPrescale'],
                                            postscale = config['Inflaton2MuMuDownstreamPostscale'],
                                            algos = [ self.selDownstream ]
                                      )
      
        self.registerLine( self.longLine )
        #self.registerLine( self.downstreamLine )
        


def makeLong(name) :
    """
    very very detached dimuon selection with long tracks
    """
    from Configurables import OfflineVertexFitter
    Detached2mu = CombineParticles("Combine"+name)
    Detached2mu.DecayDescriptor = "KS0 -> mu+ mu-"
    Detached2mu.addTool( OfflineVertexFitter() )
    Detached2mu.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Detached2mu.OfflineVertexFitter.useResonanceVertex = False
    Detached2mu.ReFitPVs = True
    Detached2mu.DaughtersCuts = { "mu+" : "(TRCHI2DOF < 4 ) "\
                                  " & (MIPCHI2DV(PRIMARY)> 100)"\
                                  " & (PIDmu > -4)" \
                                  "& (PT > 125*MeV) "}
                                 
    Detached2mu.CombinationCut = " (AMAXDOCA('')<0.1*mm)"
    #"(ADAMASS('B_s0')<1000*MeV) "\
    Detached2mu.MotherCut = "(VFASPF(VCHI2/VDOF)<10) "\
                            "& (M>250)"\
                            "& (BPVDIRA > 0) "\
                            "& (BPVVDCHI2>625)"\
                            "& (BPVVD>90)" 
             

           
    

    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")

    return Selection (name,
                      Algorithm = Detached2mu,
                      RequiredSelections = [ _stdLooseMuons ])


def makeDownstream(name) :
    """
    very very detached dimuon selection with downstream tracks
    """
    from Configurables import OfflineVertexFitter
    Detached2mu = CombineParticles("Combine"+name)
    Detached2mu.DecayDescriptor = "KS0 -> mu+ mu-"
    Detached2mu.addTool( OfflineVertexFitter() )
    Detached2mu.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Detached2mu.OfflineVertexFitter.useResonanceVertex = False
    Detached2mu.ReFitPVs = True
    Detached2mu.DaughtersCuts = { "mu+" : "(TRCHI2DOF < 4 ) "\
                                  " & (MIPCHI2DV(PRIMARY)> 150)"\
                                  "& (PT > 125*MeV) "\
                                  "& (PIDmu > -4)"}
                                 
    Detached2mu.CombinationCut = " (AMAXDOCA('')<0.2*mm)"
    #"(ADAMASS('B_s0')<1000*MeV) "\
    Detached2mu.MotherCut = "(VFASPF(VCHI2/VDOF)<12) "\
                            "& (M>250)"\
                            "& (BPVDIRA > 0) "\
                            "& (BPVVDCHI2>1000)"\
                            "& (BPVVD>200)" 
                          
    

           
    
    _stdLooseDownMuons = DataOnDemand(Location = "Phys/StdLooseDownMuons/Particles")

    return Selection (name,
                      Algorithm = Detached2mu,
                      RequiredSelections = [ _stdLooseDownMuons ])
