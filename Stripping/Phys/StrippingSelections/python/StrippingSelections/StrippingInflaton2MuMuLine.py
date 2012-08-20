'''
Module for construction of very very detached dimuon --> MuMu stripping selections and lines (for super speculative inflaton search)

Exported symbols (use python help!):
     - ..
'''

__author__ = ['Michel De Cian']
__date__ = '18/08/2012'
__version__ = '$Revision: 1.1 $'

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
                              'Inflaton2MuMuDownstreamPostscale',
                              'Bu2InflatonKPrescale', 
                              'Bu2InflatonKPostscale', 
                              'Bs2InflatonPhiPrescale', 
                              'Bs2InflatonPhiPostscale', 
                              'Bd2InflatonKstPrescale', 
                              'Bd2InflatonKstPostscale',
                              'Bd2InflatonRhoPrescale', 
                              'Bd2InflatonRhoPostscale'
                              )
    
    #### This is the dictionary of all tunable cuts ########
    config_default={
        'Inflaton2MuMuLongPrescale'    : 1,
        'Inflaton2MuMuDownstreamPrescale'    : 1,
        'Inflaton2MuMuLongPostscale'    : 1,
        'Inflaton2MuMuDownstreamPostscale'    : 1,
        'Bu2InflatonKPrescale' : 1, 
        'Bu2InflatonKPostscale' : 1, 
        'Bs2InflatonPhiPrescale' : 1, 
        'Bs2InflatonPhiPostscale' : 1, 
        'Bd2InflatonKstPrescale' : 1, 
        'Bd2InflatonKstPostscale' : 1,
        'Bd2InflatonRhoPrescale' : 1, 
        'Bd2InflatonRhoPostscale' : 1
        }                
    
    


    def __init__(self, 
                 name = 'Inflaton2MuMu',
                 config = None) :

        LineBuilder.__init__(self, name, config)

        default_name=name

        self.selLongLoose = makeLong(default_name+"_longLoose", 16, 5)
        self.selLongTight = makeLong(default_name+"_longTight", 625, 90)
        self.selDownstream = makeDownstream(default_name+"_downstream")
        self.selRho = makeRho(default_name+"_rho")

        
        self.selBu2InflatonK = makeBu2InflatonK(default_name+"_Bu2InflatonKBuilder", self.selLongLoose)
        self.selBs2InflatonPhi = makeBs2InflatonPhi(default_name+"_Bs2InflatonPhiBuilder", self.selLongLoose)
        self.selBd2InflatonKst = makeBd2InflatonKst(default_name+"_Bd2InflatonKstBuilder", self.selLongLoose)
        self.selBd2InflatonRho = makeBd2InflatonRho(default_name+"_Bd2InflatonRhoBuilder", self.selLongLoose, self.selRho)



        self.longLine = StrippingLine(default_name+"LongLine",
                                            prescale = config['Inflaton2MuMuLongPrescale'],
                                            postscale = config['Inflaton2MuMuLongPostscale'],
                                            algos = [ self.selLongTight ]
                                            )

        self.downstreamLine = StrippingLine(default_name+"DownstreamLine",
                                            prescale = config['Inflaton2MuMuDownstreamPrescale'],
                                            postscale = config['Inflaton2MuMuDownstreamPostscale'],
                                            algos = [ self.selDownstream ]
                                            )

        self.Bu2InflatonKLine = StrippingLine(default_name+"_Bu2InflatonK", 
                                              prescale = config['Bu2InflatonKPrescale'],
                                              postscale = config['Bu2InflatonKPostscale'],
                                              algos = [ self.selBu2InflatonK ]
                                              )

        self.Bs2InflatonPhiLine = StrippingLine(default_name+"_Bs2InflatonPhi", 
                                              prescale = config['Bs2InflatonPhiPrescale'],
                                              postscale = config['Bs2InflatonPhiPostscale'],
                                              algos = [ self.selBs2InflatonPhi ]
                                              )
        
        self.Bd2InflatonKstLine = StrippingLine(default_name+"_Bd2InflatonKst", 
                                              prescale = config['Bd2InflatonKstPrescale'],
                                              postscale = config['Bd2InflatonKstPostscale'],
                                              algos = [ self.selBd2InflatonKst ]
                                              )
        
        self.Bd2InflatonRhoLine = StrippingLine(default_name+"_Bd2InflatonRho", 
                                              prescale = config['Bd2InflatonRhoPrescale'],
                                              postscale = config['Bd2InflatonRhoPostscale'],
                                              algos = [ self.selBd2InflatonRho ]
                                              )

      
        self.registerLine( self.longLine )
        self.registerLine( self.Bu2InflatonKLine )
        self.registerLine( self.Bs2InflatonPhiLine )
        self.registerLine( self.Bd2InflatonKstLine )
        self.registerLine( self.Bd2InflatonRhoLine )
        #self.registerLine( self.downstreamLine )
        


def makeLong(name, vertexDistChi2, vertexDist) :
    """
    very very detached dimuon selection with long tracks
    """
    from Configurables import OfflineVertexFitter
    Detached2mu = CombineParticles("Combine"+name)
    Detached2mu.DecayDescriptor = "KS0 -> mu+ mu-"
    Detached2mu.addTool( OfflineVertexFitter )
    #Detached2mu.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Detached2mu.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    Detached2mu.OfflineVertexFitter.useResonanceVertex = False
    Detached2mu.ReFitPVs = True
    Detached2mu.DaughtersCuts = { "mu+" : "(TRCHI2DOF < 3 ) "\
                                  " & (MIPCHI2DV(PRIMARY)> 100)"\
                                  " & (PIDmu > -4)" \
                                  "& (PT > 125*MeV) "}
                                 
    Detached2mu.CombinationCut = " (AMAXDOCA('')<0.1*mm)"
    #"(ADAMASS('B_s0')<1000*MeV) "\
    Detached2mu.MotherCut = "(VFASPF(VCHI2/VDOF)<10) "\
                            "& (M>250)"\
                            "& (BPVDIRA > 0) "\
                            "& (BPVVDCHI2>  %(vertexDistChi2)s)"\
                            "& (BPVVD> %(vertexDist)s)" % locals() 
             

           
    

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
    Detached2mu.addTool( OfflineVertexFitter )
    #Detached2mu.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Detached2mu.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
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


def makeRho(name):
    """
    Selection to reconstruct detached rho(770)0 -> pi+ pi-
    """
    RhoPiPi = CombineParticles("RhoPiPi"+name)
    RhoPiPi.DecayDescriptor = 'rho(770)0 -> pi+ pi-'
    RhoPiPi.DaughtersCuts = { "pi+" : "(TRCHI2DOF < 3 )", "pi-" : "(TRCHI2DOF < 3 )" }
    RhoPiPi.MotherCut = "(VFASPF(VCHI2/VDOF)<25) "\
                        "& (M > 600)" \
                        "& (M < 1000)" \
                        "& (MIPCHI2DV(PRIMARY)> 2.25)"

    _stdLoosePions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")

    return Selection(name,
                     Algorithm = RhoPiPi,
                     RequiredSelections = [ _stdLoosePions ])




def makeBu2InflatonK(name, SelInflatonLong) :
    """
    detached Bu -> Inflaton K selection.

    Arguments:
    name        : name of the Selection.
    """

    
    from Configurables import OfflineVertexFitter
   
    Bu2InflatonK = CombineParticles("Combine"+name)
    Bu2InflatonK.DecayDescriptor =  " [B+ -> KS0 K+]cc ";
    Bu2InflatonK.addTool( OfflineVertexFitter )
    #Bu2InflatonK.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Bu2InflatonK.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    Bu2InflatonK.OfflineVertexFitter.useResonanceVertex = False
    Bu2InflatonK.ReFitPVs = True
    Bu2InflatonK.DaughtersCuts = { "K+" : "(ISLONG) & (TRCHI2DOF < 3 ) &(MIPCHI2DV(PRIMARY)>25)& (PT>250*MeV) "}
    Bu2InflatonK.CombinationCut = "(ADAMASS('B+') < 500*MeV)"
    Bu2InflatonK.MotherCut = "(BPVIPCHI2()< 50)& (VFASPF(VCHI2)<100)& (D2DVVDDOT(1) > 0.3*mm)"

    _kaons = DataOnDemand(Location='Phys/StdLooseKaons/Particles')

    return Selection( "Sel"+name,
                      Algorithm = Bu2InflatonK,
                      RequiredSelections=[SelInflatonLong,_kaons] )


def makeBs2InflatonPhi(name, SelInflatonLong) :
    """
    detached Bs -> Inflaton Phi selection. 

    Arguments:
    name        : name of the Selection.
    """

    
    from Configurables import OfflineVertexFitter
   
    _phi = DataOnDemand(Location='Phys/StdLooseDetachedPhi2KK/Particles')

    Bs2InflatonPhi = CombineParticles("Combine"+name)
    Bs2InflatonPhi.DecayDescriptor = "B_s0 -> KS0 phi(1020)"
    Bs2InflatonPhi.addTool( OfflineVertexFitter )
    #Bs2InflatonPhi.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Bs2InflatonPhi.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    Bs2InflatonPhi.OfflineVertexFitter.useResonanceVertex = False
    Bs2InflatonPhi.ReFitPVs = True
    Bs2InflatonPhi.DaughtersCuts = {"phi(1020)" : "MIPCHI2DV(PRIMARY)> 25."}
    Bs2InflatonPhi.CombinationCut = "(ADAMASS('B_s0') < 500*MeV)"
    Bs2InflatonPhi.MotherCut = "(BPVIPCHI2()< 50)& (VFASPF(VCHI2)<100)& (D2DVVDDOT(1) > 0.3*mm)"
    
    return  Selection( "Sel"+name,
                       Algorithm = Bs2InflatonPhi,
                       RequiredSelections=[SelInflatonLong, _phi] )



def makeBd2InflatonKst(name, SelInflatonLong) :
    """
    detached Bd -> Inflaton K* selection.

    Arguments:
    name        : name of the Selection.
    """

    
    from Configurables import OfflineVertexFitter
   
    
    _kstar = DataOnDemand(Location='Phys/StdLooseDetachedKst2Kpi/Particles')
    
    Bd2InflatonKst = CombineParticles("Combine"+name)
    Bd2InflatonKst.DecayDescriptor = "[B0 -> KS0 K*(892)0]cc"
    Bd2InflatonKst.addTool( OfflineVertexFitter )
    #Bd2InflatonKst.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Bd2InflatonKst.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    Bd2InflatonKst.OfflineVertexFitter.useResonanceVertex = False
    Bd2InflatonKst.ReFitPVs = True
    Bd2InflatonKst.DaughtersCuts = { "K*(892)0" : "MIPCHI2DV(PRIMARY)> 25."}
    Bd2InflatonKst.CombinationCut = "(ADAMASS('B0') < 500*MeV)"
    Bd2InflatonKst.MotherCut = "(BPVIPCHI2()< 50) & (VFASPF(VCHI2)<100)& (D2DVVDDOT(1) > 0.3*mm)"

    return Selection( "Sel"+name,
                      Algorithm = Bd2InflatonKst,
                      RequiredSelections=[SelInflatonLong, _kstar] )


def makeBd2InflatonRho(name, SelInflatonLong, SelRho) :
    """
    detached Bd -> Inflaton rho selection.

    Arguments:
    name        : name of the Selection.
    """

    
    from Configurables import OfflineVertexFitter
   
    
    Bd2InflatonRho = CombineParticles("Combine"+name)
    Bd2InflatonRho.DecayDescriptor = "B0 -> KS0 rho(770)0"
    Bd2InflatonRho.addTool( OfflineVertexFitter )
    #Bd2InflatonRho.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Bd2InflatonRho.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    Bd2InflatonRho.OfflineVertexFitter.useResonanceVertex = False
    Bd2InflatonRho.ReFitPVs = True
    Bd2InflatonRho.DaughtersCuts = { "rho(770)0" : "MIPCHI2DV(PRIMARY)> 25."}
    Bd2InflatonRho.CombinationCut = "(ADAMASS('B0') < 500*MeV)"
    Bd2InflatonRho.MotherCut = "(BPVIPCHI2()< 50) & (VFASPF(VCHI2)<100)& (D2DVVDDOT(1) > 0.3*mm)"

    return Selection( "Sel"+name,
                      Algorithm = Bd2InflatonRho,
                      RequiredSelections=[SelInflatonLong, SelRho] )

