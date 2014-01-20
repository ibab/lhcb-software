'''
Module for construction of K0s-->MuMu stripping selections and lines

Exported symbols (use python help!):
   - K0s2MuMuLinesConf
   - makeDefault
'''

__author__ = ['Diego Martinez Santos','Xabier Cid Vidal']
__date__ = '13/07/2011'
__version__ = '$Revision: 1.2 $'

__all__ = ('K0s2MuMuLinesConf',
           'makeNoMuID',
           'makeK0s2mm'
           )

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


class K0s2MuMuLinesConf(LineBuilder) :
    """
    Builder of:
     - K0s-> mumu stripping lines: default and nomuid,

    Usage:
    >>> config = { .... }
    >>> k0sConf = K0s2MuMuLinesConf('PrescaledK0s2MuMuTest',config)
    >>> k0sLines = k0sConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.


    Exports as instance data members:
    selDefault     : nominal K0s2MuMu stripping line
    defaultLine    : Stripping line made from selDefault
    lines          : list of lines:  [ defaultLine ]
    
    Exports as class data member:
    K0s2MuMuLinesConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = (
                              'NoMuIDLinePrescale',
                              'NoMuIDLinePostscale',
                              'K0s2mmLinePrescale',
                              'K0s2mmLinePostscale',
                              'minMuPT',
                              'minKsPT'                            
                              )

    
    # 2012 DiMuonDetached Hlt2Line
    # getTCKInfo(0x40990042)
    # ('Physics_June2012', 'MOORE_v14r6')
    # dump(0x40990042,lines="Hlt2DiMuonDetached")
    # (MM>0*MeV)& (MINTREE('mu-'==ABSID,MIPCHI2DV(PRIMARY)) > 25 )& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < 4 ) & (PT>600*MeV) & (MINTREE('mu-'==ABSID,PT)>300*MeV) & (VFASPF(VCHI2PDOF)<8 )& (BPVDLS>7 )

    
    #### This is the dictionary of all tunable cuts ########
    config_default={
        'NoMuIDLinePrescale'    : 1e-03,
        'NoMuIDLinePostscale'   : 1,
        'K0s2mmLinePrescale'  : 1,
        'K0s2mmLinePostscale'  : 1,
        #'minMuPT' : 300,  #MeV
        #'minKsPT' : 600,  #MeV
        'minMuPT' : 0,  #MeV
        'minKsPT' : 0,  #MeV
        }                
    
    
    def __init__(self, 
                 name = 'K0s2MuMu',
                 config = None) :

        LineBuilder.__init__(self, name, config)

        norm_name=name+'NoMuID'
        

        self.NoMuID = makeNoMuID(norm_name, config["minMuPT"], config["minKsPT"])
        self.MuID = makeK0s2mm(name , config["minMuPT"], config["minKsPT"])

       
        self.NoMuIDLine = StrippingLine(norm_name+"Line",
                                            prescale = config['NoMuIDLinePrescale'],
                                            postscale = config['NoMuIDLinePostscale'],
                                            algos = [ self.NoMuID ]
                                            )
        
        self.MuIDLine = StrippingLine(name+"Line",
                                      prescale = config['K0s2mmLinePrescale'],
                                      postscale = config['K0s2mmLinePostscale'],
                                      algos = [ self.MuID ]
                                      )
        
        
        self.registerLine(self.NoMuIDLine)
        self.registerLine(self.MuIDLine)
       

def makeNoMuID(name,mupt=0,kspt=0) :
    """
    default K0s2mumu selection object
    starts from Phys/StdNoPIDsMuons

    Please contact Diego Martinez Santos if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    from Configurables import OfflineVertexFitter
    K0s2MuMuNoMuID = CombineParticles("Comine"+name)
    K0s2MuMuNoMuID.DecayDescriptor = "KS0 -> pi+ pi-"
    # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
    K0s2MuMuNoMuID.addTool( OfflineVertexFitter )
    K0s2MuMuNoMuID.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    K0s2MuMuNoMuID.OfflineVertexFitter.useResonanceVertex = False
    K0s2MuMuNoMuID.ReFitPVs = True
    K0s2MuMuNoMuID.DaughtersCuts = { "pi+" : "(MIPCHI2DV(PRIMARY)> 100.)&(TRCHI2DOF < 5 ) & (PT >"+str(mupt)+" * MeV )" }
    K0s2MuMuNoMuID.CombinationCut = "(ADAMASS('KS0')<100*MeV)"\
                                   "& (AMAXDOCA('')<0.3*mm)"
    

    K0s2MuMuNoMuID.MotherCut = "((BPVDIRA>0) & ((BPVVDSIGN*M/P) > 0.1*89.53*2.9979e-01) & (MIPDV(PRIMARY)<0.4*mm) & (M>400) & (M<600) & (PT > "+str(kspt)+" * MeV))"
    
                             
    _stdNoPIDsPions = DataOnDemand(Location = "Phys/StdNoPIDsPions/Particles")

    return Selection (name,
                      Algorithm = K0s2MuMuNoMuID,
                      RequiredSelections = [ _stdNoPIDsPions])


def makeK0s2mm(name,mupt = 0, kspt = 0) :
    """
    K0s2mumu selection object
    with muon Id and wide mass window
    starts from Phys/StdLooseMuons

    Please contact Diego Martinez Santos if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    from Configurables import OfflineVertexFitter
    K0s2MuMu = CombineParticles("Combine"+name)
    K0s2MuMu.DecayDescriptor = "KS0 -> mu+ mu-"
    K0s2MuMu.addTool( OfflineVertexFitter )
    K0s2MuMu.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    K0s2MuMu.OfflineVertexFitter.useResonanceVertex = False
    K0s2MuMu.ReFitPVs = True
    K0s2MuMu.DaughtersCuts = { "mu+" : "(MIPCHI2DV(PRIMARY)> 100.)&(TRCHI2DOF < 5 ) & (PT >"+str(mupt)+" * MeV)" }
    K0s2MuMu.CombinationCut ="(ADAMASS('KS0')<1000*MeV)"\
                              "& (AMAXDOCA('')<0.3*mm)"

    K0s2MuMu.MotherCut = "((BPVDIRA>0) & ((BPVVDSIGN*M/P) > 0.1*89.53*2.9979e-01) & (MIPDV(PRIMARY)<0.4*mm) & (M>450) & (PT > "+str(kspt)+" * MeV))"
    
    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")

    return Selection (name,
                      Algorithm = K0s2MuMu,
                      RequiredSelections = [ _stdLooseMuons])


