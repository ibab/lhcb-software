'''
Module for construction of B-->MuMuMuMu and D-->MuMuMuMu stripping selections and lines


Exported symbols (use python help!):
     - ..
'''

__author__ = ['Diego Martinez Santos','Johannes Albrecht']
__date__ = '20/10/2010'
__version__ = '$Revision: 1.0 $'

__all__ = ('B2MuMuMuMuLinesConf',
           'makeB2MuMuMuMu'
           )

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
#from StrippingSelections.Utils import checkConfig

class B2MuMuMuMuLinesConf(LineBuilder) :
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

    __configuration_keys__ = ('B2MuMuMuMuLinePrescale',
                              'B2MuMuMuMuLinePostscale',
                              'D2MuMuMuMuLinePrescale',
                              'D2MuMuMuMuLinePostscale'
                              )
    
    #### This is the dictionary of all tunable cuts ########
    config_default={
        'B2MuMuMuMuLinePrescale'    : 1,
        'B2MuMuMuMuLinePostscale'   : 1,
        'D2MuMuMuMuLinePrescale'    : 1,
        'D2MuMuMuMuLinePostscale'   : 1
        }                
    
    


    def __init__(self, 
                 name = 'B2MuMuMuMu',
                 config = None) :

        LineBuilder.__init__(self, name, config)
        #checkConfig(B2MuMuMuMuLinesConf.__configuration_keys__,config)

        default_name=name

        D_name='D2MuMuMuMu'
	Dst_name = 'Dstar2D2MuMuMuMu'
	Dimuon_name = 'Dimuon'
        self.inPions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")
        self.inMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")

        self.selDefault = makeDefault(default_name,
						inputSel = [ self.inMuons ])

        self.selD2MuMuMuMu = makeD2MuMuMuMu(D_name,
						inputSel = [ self.inMuons ])



        self.defaultLine = StrippingLine(default_name+"Line",
                                            prescale = config['B2MuMuMuMuLinePrescale'],
                                            postscale = config['B2MuMuMuMuLinePostscale'],
                                            algos = [ self.selDefault ]
                                            )

        self.D2MuMuMuMuLine = StrippingLine(D_name+"Line",
                                            prescale = config['D2MuMuMuMuLinePrescale'],
                                            postscale = config['D2MuMuMuMuLinePostscale'],
                                            algos = [ self.selD2MuMuMuMu ]
                                            )
        
      
        self.registerLine( self.defaultLine )

        self.registerLine( self.D2MuMuMuMuLine )


def makeDefault(name,inputSel) :
    """
    B --> 4 mu selection
    should become     inclusive bb-->4 mu selection  ??
    """
    from Configurables import OfflineVertexFitter
    Detached4mu = CombineParticles("Combine"+name)
    Detached4mu.DecayDescriptor = "B_s0 -> mu+ mu- mu+ mu-"
    # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
    Detached4mu.addTool( OfflineVertexFitter )
    Detached4mu.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    Detached4mu.OfflineVertexFitter.useResonanceVertex = False
    Detached4mu.ReFitPVs = True
    Detached4mu.DaughtersCuts = { "mu+" : "(TRCHI2DOF < 2.5 ) "\
                                  " & (MIPCHI2DV(PRIMARY)> 9.)"}
                                 
    Detached4mu.CombinationCut = "(ADAMASS('B_s0')<1000*MeV) "\
                                   "& (AMAXDOCA('')<0.3*mm)"
    Detached4mu.MotherCut = "(VFASPF(VCHI2/VDOF)<9) "\
                              "& (BPVDIRA > 0) "\
                              "& (BPVVDCHI2>100)"\
                              " & (M>4366.3) & (M<6366.3)"\
                              "& (BPVIPCHI2()< 25) "
    

    return Selection (name,
                      Algorithm = Detached4mu,
                      RequiredSelections = inputSel)



def makeD2MuMuMuMu(name,inputSel) :
    """
    D --> 4 mu selection
    """
    from Configurables import OfflineVertexFitter
    D2MuMuMuMu = CombineParticles("Combine"+name)

    D2MuMuMuMu.DecayDescriptor = "D0 -> mu+ mu- mu+ mu-"
    D2MuMuMuMu.addTool( OfflineVertexFitter )
    D2MuMuMuMu.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )

    D2MuMuMuMu.DaughtersCuts = { "mu+" : "(TRCHI2DOF < 2.0 ) "\
                                  " & (MIPCHI2DV(PRIMARY)> 4.)"\
                                  " & (P> 3000.*MeV)"}

    D2MuMuMuMu.CombinationCut =  "(ADAMASS('D0')<300*MeV) "\
                                 "& (AMAXDOCA('')<0.2*mm) "

 
    D2MuMuMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)<12.) "\
			      "& (BPVVDZ > 0.) " \
			      "& (MIPCHI2DV(PRIMARY) < 25. )"

    return Selection (name,
                      Algorithm = D2MuMuMuMu,
                      RequiredSelections = inputSel)

