'''
Module for construction of lepton flavor violation stripping selections and lines

Exported symbols (use python help!):
   - 
'''

__author__ = ['Johannes Albrecht']
__date__ = '13/01/2012'
__version__ = '$Revision: 1.0 $'

__all__ = ('LFVLinesConf',
           'config_default',
           'makeTau2PhiMu',
           'makeTau2MuMue',
           'makeB2eMu',
           'makeB2ee',
           'makeB2hemu'
           'makeB2pMu',
           'makeBu',
           )

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
#from StrippingSelections.Utils import checkConfig
from GaudiKernel.PhysicalConstants import c_light

class LFVLinesConf(LineBuilder) :
    """
    Builder 

 
    """

    __configuration_keys__ = (    'Postscale',
                                  'TauPrescale',
                                  'Tau2MuMuePrescale',
                                  'B2eMuPrescale',
                                  'B2eePrescale',                                  
                                  'B2heMuPrescale',
                                  'B2pMuPrescale',
                                  'Bu2KJPsieePrescale',
                                  )
    
    #### This is the dictionary of all tunable cuts ########
    config_default={
        'Postscale'           :1,
        'TauPrescale'         :1,
        'Tau2MuMuePrescale'         :1,
        'B2eMuPrescale'       :1,
        'B2eePrescale'        :1,
        'B2heMuPrescale'      :1,
        'B2pMuPrescale'       :1,
        'Bu2KJPsieePrescale'  :1,
        }                
    
    
    def __init__(self, 
                 name = 'LFV',
                 config = None) :

        LineBuilder.__init__(self, name, config)
        
        tau_name=name+'Tau2PhiMu'
        mme_name=name+'Tau2eMuMu'
        emu_name=name+'B2eMu'
        ee_name=name+'B2ee'
        hemu_name=name+'B2heMu'
        pmu_name=name+'B2pMu'
        bu_name=name+'Bu2KJPsiee'

        self.selTau2PhiMu = makeTau2PhiMu(tau_name)
        self.selTau2eMuMu = makeTau2eMuMu(mme_name)
        self.selB2eMu = makeB2eMu(emu_name)
        self.selB2ee = makeB2ee(ee_name)
        self.selB2heMu = makeB2heMu(hemu_name)
        self.selB2pMu = makeB2pMu(pmu_name)
        self.selBu = makeBu(bu_name)
                
        self.tau2PhiMuLine = StrippingLine(tau_name+"Line",
                                     prescale = config['TauPrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selTau2PhiMu ]
                                     )

        self.tau2eMuMuLine = StrippingLine(mme_name+"Line",
                                           prescale = config['Tau2MuMuePrescale'],
                                           postscale = config['Postscale'],
                                           algos = [ self.selTau2eMuMu ]
                                           )

        self.b2eMuLine = StrippingLine(emu_name+"Line",
                                     prescale = config['B2eMuPrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selB2eMu ]
                                     )

        self.b2eeLine = StrippingLine(ee_name+"Line",
                                     prescale = config['B2eePrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selB2ee ]
                                       )

        self.b2heMuLine = StrippingLine(hemu_name+"Line",
                                     prescale = config['B2heMuPrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selB2heMu ]
                                     )


        self.b2pMuLine = StrippingLine(pmu_name+"Line",
                                     prescale = config['B2pMuPrescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.selB2pMu ]
                                     )

        self.buLine = StrippingLine(bu_name+"Line",
                                    prescale = config['Bu2KJPsieePrescale'],
                                    postscale = config['Postscale'],
                                    algos = [ self.selBu ]
                                    )
              
        self.registerLine(self.tau2PhiMuLine)
        self.registerLine(self.tau2eMuMuLine)
        self.registerLine(self.b2eMuLine)
        self.registerLine(self.b2eeLine)
        self.registerLine(self.b2heMuLine)
        self.registerLine(self.b2pMuLine)
        self.registerLine(self.buLine)        

def makeTau2PhiMu(name):
    """
    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    from Configurables import OfflineVertexFitter
    Tau2PhiMu = CombineParticles("Combine"+name)
    Tau2PhiMu.DecayDescriptor = " [ tau+ -> phi(1020) mu+ ]cc"

    makePhi = CombineParticles(name+"makePhi")
    makePhi.DecayDescriptor =  "phi(1020) -> K+ K-"
    makePhi.DaughtersCuts = {"K+": "(ISLONG) & (TRCHI2DOF < 4 ) & ( BPVIPCHI2 () >  9 ) "\
                                 "& (PT>300*MeV) & (PIDK > 5)"}
    
    _kaons = DataOnDemand(Location='Phys/StdLooseKaons/Particles')
    
    makePhi.CombinationCut =  "(ADAMASS('phi(1020)')<20*MeV)"
    makePhi.MotherCut = " (MIPCHI2DV(PRIMARY)> 25.)"
    
    SelPhi = Selection( name+"SelPhi",                       Algorithm= makePhi,
                        RequiredSelections=[_kaons] )


    Tau2PhiMu.DaughtersCuts = { "mu+" : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 4  ) & ( BPVIPCHI2 () >  9 ) " }
    Tau2PhiMu.CombinationCut = "(ADAMASS('tau+')<150*MeV)"

    Tau2PhiMu.MotherCut = """
            ( VFASPF(VCHI2) < 15 ) &
            ( (BPVLTIME ( 225 ) * c_light)   > 100 * micrometer ) &
            ( BPVIPCHI2() < 225 )
            """ 
                             
    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")

    return Selection (name,
                      Algorithm = Tau2PhiMu,
                      RequiredSelections = [ _stdLooseMuons ,SelPhi ])


def makeTau2eMuMu(name):
    """
    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    from Configurables import OfflineVertexFitter
    Tau2eMuMu = CombineParticles("Comine"+name)
    Tau2eMuMu.DecayDescriptors = [" [ tau+ -> e+ mu+ mu- ]cc"," [ tau+ -> mu+ mu+ e- ]cc"]
    Tau2eMuMu.DaughtersCuts = { "mu+" : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 4  ) & ( BPVIPCHI2 () >  9 ) " ,
                                "e+" : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 4  ) & ( BPVIPCHI2 () >  9 ) " }
    Tau2eMuMu.CombinationCut = "(ADAMASS('tau+')<200*MeV)"

    Tau2eMuMu.MotherCut = """
            ( VFASPF(VCHI2) < 15 ) &
            ( (BPVLTIME ( 225 ) * c_light)   > 100 * micrometer ) &
            ( BPVIPCHI2() < 225 )
            """ 
                             
    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    _stdLooseElectrons= DataOnDemand(Location = "Phys/StdLooseElectrons/Particles")

    return Selection (name,
                      Algorithm = Tau2eMuMu,
                      RequiredSelections = [ _stdLooseMuons,_stdLooseElectrons]) 



def makeB2eMu(name):
    """
    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    
    from Configurables import OfflineVertexFitter
    Bs2eMu = CombineParticles("Combine"+name)
    Bs2eMu.DecayDescriptor = "[B_s0 -> e+ mu-]cc"
    # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
    Bs2eMu.addTool( OfflineVertexFitter() )
    Bs2eMu.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Bs2eMu.OfflineVertexFitter.useResonanceVertex = False
    #Bs2eMu.ReFitPVs = True
    Bs2eMu.DaughtersCuts = { "mu+" : "(MIPCHI2DV(PRIMARY)> 25.)&(TRCHI2DOF < 4 )",
                             "e+" : "(MIPCHI2DV(PRIMARY)> 25.)&(TRCHI2DOF < 4 )"}

    Bs2eMu.CombinationCut = "(ADAMASS('B_s0')<1200*MeV)"\
                            "& (AMAXDOCA('')<0.3*mm)"

    Bs2eMu.MotherCut = "(VFASPF(VCHI2/VDOF)<9) "\
                              "& (ADMASS('B_s0') < 1200*MeV )"\
                              "& (BPVDIRA > 0) "\
                              "& (BPVVDCHI2> 225)"\
                              "& (BPVIPCHI2()< 25) "

    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    _stdLooseElectrons= DataOnDemand(Location = "Phys/StdLooseElectrons/Particles")

    return Selection (name,
                      Algorithm = Bs2eMu,
                      RequiredSelections = [ _stdLooseMuons,_stdLooseElectrons])



def makeB2ee(name):
    """
    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    
    from Configurables import OfflineVertexFitter
    Bs2ee = CombineParticles("Combine"+name)
    Bs2ee.DecayDescriptor = "B_s0 -> e+ e-"
    # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
    Bs2ee.addTool( OfflineVertexFitter() )
    Bs2ee.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Bs2ee.OfflineVertexFitter.useResonanceVertex = False
    #Bs2ee.ReFitPVs = True
    Bs2ee.DaughtersCuts = { "e+" : "(MIPCHI2DV(PRIMARY)> 25.)&(TRCHI2DOF < 4 )"}

    Bs2ee.CombinationCut = "(ADAMASS('B_s0')<1200*MeV)"\
                            "& (AMAXDOCA('')<0.3*mm)"

    Bs2ee.MotherCut = "(VFASPF(VCHI2/VDOF)<9) "\
                              "& (ADMASS('B_s0') < 1200*MeV )"\
                              "& (BPVDIRA > 0) "\
                              "& (BPVVDCHI2> 225)"\
                              "& (BPVIPCHI2()< 25) "
                             
    _stdLooseElectrons= DataOnDemand(Location = "Phys/StdLooseElectrons/Particles")

    return Selection (name,
                      Algorithm = Bs2ee,
                      RequiredSelections = [ _stdLooseElectrons])




def makeB2heMu(name):
    """
    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    
    from Configurables import OfflineVertexFitter
    Bs2heMu = CombineParticles("Combine"+name)
    Bs2heMu.DecayDescriptors = ["[B+ -> K+ e+ mu-]cc","[B+ -> K+ e- mu+]cc",
                                "[B+ -> pi+ e+ mu-]cc","[B+ -> pi+ e- mu+]cc" ]
    Bs2heMu.addTool( OfflineVertexFitter() )
    Bs2heMu.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Bs2heMu.OfflineVertexFitter.useResonanceVertex = False
    #Bs2heMu.ReFitPVs = True
    Bs2heMu.DaughtersCuts = { "mu+" : "(MIPCHI2DV(PRIMARY)>25.)&(TRCHI2DOF<4)",
                              "e+" : "(MIPCHI2DV(PRIMARY)>25.)&(TRCHI2DOF<4)",
                              "pi+" : "(MIPCHI2DV(PRIMARY)>25.)&(TRCHI2DOF<4)",
                              "K+" : "(MIPCHI2DV(PRIMARY)>25.)&(TRCHI2DOF<4)"}

    Bs2heMu.CombinationCut = "(ADAMASS('B+')<600*MeV)"\
                            "& (AMAXDOCA('')<0.3*mm)"

    Bs2heMu.MotherCut = "(VFASPF(VCHI2/VDOF)<9) "\
                              "& (ADMASS('B_s0') < 600*MeV )"\
                              "& (BPVDIRA > 0) "\
                              "& (BPVVDCHI2> 225)"\
                              "& (BPVIPCHI2()< 25) "

    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    _stdLooseElectrons= DataOnDemand(Location = "Phys/StdLooseElectrons/Particles")
    _stdLoosePions= DataOnDemand(Location = "Phys/StdLoosePions/Particles")
    _stdLooseKaons= DataOnDemand(Location = "Phys/StdLooseKaons/Particles")

    return Selection (name,
                      Algorithm = Bs2heMu,
                      RequiredSelections = [ _stdLooseMuons,_stdLooseElectrons,
                                             _stdLoosePions, _stdLooseKaons ])


def makeB2pMu(name):
    """
    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    
    from Configurables import OfflineVertexFitter
    Bs2pMu = CombineParticles("Combine"+name)
    Bs2pMu.DecayDescriptor = "[B_s0 -> p+ mu-]cc"
    #Bs2pMu.addTool( OfflineVertexFitter() )
    #Bs2pMu.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    #Bs2pMu.OfflineVertexFitter.useResonanceVertex = False
    #Bs2pMu.ReFitPVs = True
    Bs2pMu.DaughtersCuts = { "mu+" : "(MIPCHI2DV(PRIMARY)> 25.)&(TRCHI2DOF < 4 )",
                             "p+" : "(MIPCHI2DV(PRIMARY)> 25.)&(TRCHI2DOF < 4 )"}

    Bs2pMu.CombinationCut = "(ADAMASS('B_s0')<600*MeV)"\
                            "& (AMAXDOCA('')<0.3*mm)"
    
    Bs2pMu.MotherCut = "(VFASPF(VCHI2/VDOF)<9) "\
                       "& (ADMASS('B_s0') < 600*MeV )"\
                       "& (BPVDIRA > 0) "\
                       "& (BPVVDCHI2> 225)"\
                       "& (BPVIPCHI2()< 25) "

    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
    _stdLooseProtons= DataOnDemand(Location = "Phys/StdLooseProtons/Particles")

    return Selection (name,
                      Algorithm = Bs2pMu,
                      RequiredSelections = [ _stdLooseMuons,_stdLooseProtons])


def makeBu(name) :
    """
    detached Bu-->JPsiK selection.

    Please contact Flavio Archilli if you think of prescaling this line!

    Arguments:
    name        : name of the Selection.
    """

    
    from Configurables import OfflineVertexFitter
   
    SelDJPsi = makeDetachedJPsi(name)

    PreselBu2JPsiKCommon = CombineParticles("PreselBu2JPsiKCommon")
    PreselBu2JPsiKCommon.DecayDescriptor =  " [B+ -> J/psi(1S) K+]cc ";
    PreselBu2JPsiKCommon.addTool( OfflineVertexFitter() )
    PreselBu2JPsiKCommon.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    PreselBu2JPsiKCommon.OfflineVertexFitter.useResonanceVertex = False
    PreselBu2JPsiKCommon.ReFitPVs = True
    PreselBu2JPsiKCommon.DaughtersCuts = { "K+" : "(ISLONG) & (TRCHI2DOF < 5 ) &(MIPCHI2DV(PRIMARY)>25)& (PT>250*MeV) "}
    PreselBu2JPsiKCommon.CombinationCut = "(ADAMASS('B+') < 600*MeV)"
    PreselBu2JPsiKCommon.MotherCut = "(BPVIPCHI2()< 25)& (VFASPF(VCHI2)<45) "

    _kaons = DataOnDemand(Location='Phys/StdNoPIDsKaons/Particles')

    return Selection( "SelBu2KJPsiee",
                         Algorithm = PreselBu2JPsiKCommon,
                         RequiredSelections=[SelDJPsi,_kaons] )


def makeDetachedJPsi(name) :
    """
    detached JPsi selection for B--> JPsi X calibration and
    normalization channels. 

    Please contact Flavio Archilli if you think of prescaling this line!

    Arguments:
    name        : name of the Selection.
    """
    from Configurables import OfflineVertexFitter
    DetachedJPsi = CombineParticles("Combine"+name)
    DetachedJPsi.DecayDescriptor = "J/psi(1S) -> e+ e-"
    DetachedJPsi.addTool( OfflineVertexFitter() )
    DetachedJPsi.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    DetachedJPsi.OfflineVertexFitter.useResonanceVertex = False
    DetachedJPsi.ReFitPVs = True
    DetachedJPsi.DaughtersCuts = { "e+" : "(TRCHI2DOF < 4 ) "\
                                   "& (MIPCHI2DV(PRIMARY)> 25.) "\
                                   "& (PIDe > 2) "}
                                 
    DetachedJPsi.CombinationCut = "(ADAMASS('J/psi(1S)')<1000*MeV) "\
                                   "& (AMAXDOCA('')<0.3*mm)"

    DetachedJPsi.MotherCut = "(VFASPF(VCHI2)<9) "\
                             "& (ADMASS('J/psi(1S)') < 1000*MeV )"\
                             "& (BPVDIRA > 0) "\
                             "& (BPVVDCHI2>169)"

    _stdLooseElectrons = DataOnDemand(Location = "Phys/StdLooseElectrons/Particles")

    return Selection (name,
                      Algorithm = DetachedJPsi,
                      RequiredSelections = [ _stdLooseElectrons ])

