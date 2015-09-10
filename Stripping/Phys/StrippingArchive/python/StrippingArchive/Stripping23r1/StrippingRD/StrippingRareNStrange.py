__author__  = 'Diego Martinez Santos, Carlos Vazquez, Xabier Cid'
__date__    = '08/05/2015'
__version__ = '$Revision: Not sure $'

__all__ = ( 'RnSConf', 'default_config' ) 

"""
Selections for Rare&strange decays (other than those already existing), eg:
'ksmumu' : 'KS0 ==> mu+ mu-',
'kspi0mumu' : 'KS0 ==> pi0 mu+ mu-',
#'ks4mu' : 'KS0 ==> mu+ mu- mu- mu+',
#'ksemu' : '[KS0 ==> e- mu+]CC',
'kseemumuLFV' : '[KS0 ==> e+ e+ mu- mu-]cc',
'ks4e' : 'KS0 ==> e- e+ e+ e-',
'ks3emu' : '[KS0 ==> e- e+ e+ mu-]CC',
'ks3mue' : '[KS0 ==> e- mu+ mu+ mu-]CC',
'kseepipi' : 'KS0 ==> e- e+ pi+ pi-',
'lpimu': '[Lambda0 ==> pi+ mu-]CC',
'lpie': '[Lambda0 ==> pi+ e-]CC',
'lpi3mu': '[Lambda0 ==> pi+ mu- mu+ mu-]CC',
'lpimuee': '[Lambda0 ==> pi+ mu- e+ e-]CC',
'lpi3e': '[Lambda0 ==> pi+ e- e+ e-]CC',

           
"""

default_config = {
    'NAME'                      : 'RnS',
    'BUILDERTYPE'               : 'RnSConf',
    'CONFIG'                    :
    {'SignalLinePrescale'    : 1,
     'SignalLinePostscale'   : 1,
     'SidebandLinePrescale'  : 0.2,
     'SidebandLinePostscale' : 1,
     "muIpChi2"              : 36,
     "eIpChi2"              : 49,
     "piIpChi2"              : 100,
     "protonIpChi2"              : 16,
     "muTrChi2Dof"           : 5,
     "KSsignalminMass"       : 300,
     "KSsignalmaxMass"       : 600,
     "KSsidebminMass"        : 600,
     "KSsidebmaxMass"        : 1000,
     "DiDOCA"                : 0.3,
     "KSdira"                : 0,
     "KSlife"                : 0.06*89.53*2.9979e-01,
     "L0life"                : 0.06*263.2*2.9979e-01,
     "KSip"                  : 0.9,
     "MultibodyIPChi2": 25,
     "MultibodyChi2dof": 9,
     'NoMuIDLinePrescale'    : 1e-03, ## Ks2MuMu specific
     'NoMuIDLinePostscale'   : 1,
     'K0s2mmLinePrescale'  : 1,
     'K0s2mmLinePostscale'  : 1,
     'K0s2mmSBLinePrescale'  : 0.1,
     'K0s2mmSBLinePostscale'  : 1,
     'K0s2mmSBCut'  : 465  
              },
   
    'WGs'                       : [ 'RD' ],
    'STREAMS'                   : [ 'Leptonic' ]
    }
    

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection, AutomaticData
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


class RnSConf(LineBuilder) :
    """
    Builder for some Rare & strange measurements 
    """
        
    # now just define keys. Default values are fixed later
    __configuration_keys__ = tuple(default_config['CONFIG'].keys())
    
    def __init__(self, name, config):
       
        LineBuilder.__init__(self, name, config)
        
        from StandardParticles import StdLooseMuons as Muons
        from StandardParticles import StdLooseElectrons as Electrons
        
        from StandardParticles import StdLooseProtons as Protons
        from StandardParticles import StdLoosePions as Pions
        self._name = name
        self.config = config
        self.name = name
       
        self.SelMuons  = self._filterMuons( name   = "MuonsFor" + self._name,
                                        sel    = Muons 
                                        )
        self.SelElectrons  = self._filterElectrons( name   = "ElectronsFor" + self._name,
                                        sel    = Electrons
                                        )
        self.SelPions  = self._filterPions( name   = "PionsFor" + self._name,
                                        sel    = Pions 
                                        )
        self.SelProtons  = self._filterProtons( name   = "ProtonsFor" + self._name,
                                        sel    = Protons 
                                        )
        
        self.SelDiMuons = self._makePseudoJPsi("SelDiMuonsFor" + self._name)
        
        self.SelDiElectrons = self._makePseudoPhi("SelDiElectronsFor" + self._name) 
        self.SelEMu = self._makePseudoKstar("SelEMuFor" + self._name) 
       
        self.Ks2Pi0MuMuSignal = self._makePi0MuMu_Signal("Ks2Pi0MuMuSignal")
        self.Ks2Pi0MuMuSB = self._makePi0MuMu_SB("Ks2Pi0MuMuSB")

        self.K0s2PiPi = self._makeK0s2TwoProngs("Ks2PiPiForRnS","KS0 -> pi+ pi-", "(ADAMASS('KS0')<100*MeV)", "(M>400) & (M<600)")
        self.K0s2MuMu = self._makeK0s2TwoProngs("Ks2MuMu","KS0 -> mu+ mu-", "(ADAMASS('KS0')<100*MeV)", '(M>%(K0s2mmSBCut)s)'% self.config)
        self.K0s2MuMuLeftSB = self._makeK0s2TwoProngs("Ks2MuMuLeftSB","KS0 -> mu+ mu-", "(ADAMASS('KS0')<100*MeV)", '(M<%(K0s2mmSBCut)s)'% self.config)
        self.L02pPi = self._makeLambda2TwoProngs("LambdapPiForRnS","[Lambda0 -> p+ pi-]cc", "(ADAMASS('Lambda0')<100*MeV)", "(M>1000) & (M<1200)")
        self.L02PiMu = self._makeLambda2TwoProngs("LambdaPiMu","[Lambda0 -> mu+ pi-]cc", "(ADAMASS('Lambda0')<1000*MeV)", "(M>600) & (M<1500)")

        ## Multibody Lambda Algorithms (actually one is 2 body , as a test)
        self.L02PiE = self._makeMultibody("LambdaPiE","[Lambda0 -> e+ pi-]cc", "(ADAMASS('Lambda0')<1000*MeV)", "(M>600) & (M<1500)")
        self.L02Pi3Mu = self._makeMultibody("LambdaPi3Mu","[Lambda0 -> J/psi(1S) mu+  pi-]cc", "(ADAMASS('Lambda0')<1000*MeV)", "(M>600) & (M<1500)")
        self.L02PiMuMuE = self._makeMultibody("LambdaPiMuMuE","[Lambda0 -> J/psi(1S) e+  pi-]cc", "(ADAMASS('Lambda0')<1000*MeV)", "(M>600) & (M<1500)")

        ## Multibody Ks algorithms (actually one is 2 body , as a test)
        self.K0s2eMu = self._makeMultibody("Ks2eMu","[KS0 -> e+ mu-]cc", "(ADAMASS('KS0')<100*MeV)","(M>4500) & (M<600)" )
        self.K0s24ProngLFV = self._makeMultibody("Ks024prongLFV","[KS0 -> K*(892)0 K*(892)0]cc", "(ADAMASS('KS0')<300*MeV)", "(M>200) & (M<800)")
        self.K0s2e3Mu = self._makeMultibody("Ks02e3Mu","[KS0 -> J/psi(1S) K*(892)0]cc", "(ADAMASS('KS0')<300*MeV)", "(M>200) & (M<800)")
        self.K0s2Mu3e = self._makeMultibody("Ks02M3eu","[KS0 -> phi(1020) K*(892)0]cc", "(ADAMASS('KS0')<300*MeV)", "(M>200) & (M<800)")

        self.Pi0MMsignalLine = StrippingLine("Ks2Pi0MuMuSignalLine",
                                        prescale = self.config[ 'SignalLinePrescale'],
                                        postscale = self.config['SignalLinePostscale'],
                                        algos = [ self.Ks2Pi0MuMuSignal ],
                                        RequiredRawEvents = ["Muon","Calo"],
                                        MDSTFlag = True
                                        )

        self.Pi0MMsidebandLine = StrippingLine("Ks2Pi0MuMuSBLine",
                                          prescale = self.config[ 'SidebandLinePrescale'],
                                          postscale = self.config['SidebandLinePostscale'],
                                          algos = [ self.Ks2Pi0MuMuSB ],
                                          RequiredRawEvents = ["Muon","Calo"],
                                          MDSTFlag = True
                                               )
       
        self.K0s2PiPiLine = StrippingLine("Ks2PiPiForRnSLine",
                                        prescale = self.config['NoMuIDLinePrescale'],
                                        postscale = self.config['NoMuIDLinePostscale'],
                                        algos = [ self.K0s2PiPi ],
                                        RequiredRawEvents = ["Muon"],
                                        MDSTFlag = True
                                        )
        

        self.K0s2MuMuSBLine = StrippingLine("K0s2MuMuSBLine",
                                        prescale = config['K0s2mmSBLinePrescale'],
                                        postscale = config['K0s2mmSBLinePostscale'],
                                        algos = [ self.K0s2MuMuLeftSB ],
                                        RequiredRawEvents = ["Muon"],
                                        MDSTFlag = True
                                        )
        
        
        self.K0s2MuMuLine = StrippingLine("K0s2MuMuLine",
                                      prescale = config['K0s2mmLinePrescale'],
                                      postscale = config['K0s2mmLinePostscale'],
                                      algos = [ self.K0s2MuMu ],
                                      RequiredRawEvents = ["Muon"],
                                      MDSTFlag = True
                                      )
        self.Lambda02pPiLine = StrippingLine("Lambda2pPiForRnSLine",
                                        prescale = self.config['NoMuIDLinePrescale'],
                                        postscale = self.config['NoMuIDLinePostscale'],
                                        algos = [ self.L02pPi ],
                                        RequiredRawEvents = ["Muon"],
                                        MDSTFlag = True
                                        )
        self.Lambda02PiMuLine = StrippingLine("Lambda02PiMuLine",
                                      prescale = config['K0s2mmLinePrescale'],
                                      postscale = config['K0s2mmLinePostscale'],
                                      algos = [ self.L02PiMu ],
                                      RequiredRawEvents = ["Muon"],
                                      MDSTFlag = True
                                            )
        self.Lambda02Pi3MuLine = StrippingLine("Lambda02Pi3MuLine",
                                      prescale = config['K0s2mmLinePrescale'],
                                      postscale = config['K0s2mmLinePostscale'],
                                      algos = [ self.L02Pi3Mu ],
                                      RequiredRawEvents = ["Muon"],
                                      MDSTFlag = True
                                            )
       
       
        self.K0s24ProngLFVLine = StrippingLine("K0s24ProngLFVLine",
                                      prescale = config['K0s2mmLinePrescale'],
                                      postscale = config['K0s2mmLinePostscale'],
                                      algos = [ self.K0s24ProngLFV ],
                                      RequiredRawEvents = ["Muon"],
                                      MDSTFlag = True
                                      )
        self.K0s2e3MuLine = StrippingLine("K0s2e3MuLine",
                                      prescale = config['K0s2mmLinePrescale'],
                                      postscale = config['K0s2mmLinePostscale'],
                                      algos = [ self.K0s2e3Mu ],
                                      RequiredRawEvents = ["Muon"],
                                      MDSTFlag = True
                                      )
        self.K0s2Mu3eLine = StrippingLine("K0s2Mu3eLine",
                                      prescale = config['K0s2mmLinePrescale'],
                                      postscale = config['K0s2mmLinePostscale'],
                                      algos = [ self.K0s2Mu3e ],
                                      RequiredRawEvents = ["Muon"],
                                      MDSTFlag = True
                                      )
        self.Lambda02PiMuMuELine = StrippingLine("Lambda02PiMuMuELine",
                                      prescale = config['K0s2mmLinePrescale'],
                                      postscale = config['K0s2mmLinePostscale'],
                                      algos = [ self.L02PiMuMuE ],
                                      RequiredRawEvents = ["Muon"],
                                      MDSTFlag = True
                                            )       

        self.K0s2eMuLine = StrippingLine("K0s2eMuLine",
                                      prescale = config['K0s2mmLinePrescale'],
                                      postscale = config['K0s2mmLinePostscale'],
                                      algos = [ self.K0s2eMu ],
                                      RequiredRawEvents = ["Muon"],
                                      MDSTFlag = True
                                                 )
        self.Lambda02PiELine = StrippingLine("Lambda02PiELine",
                                      prescale = config['K0s2mmLinePrescale'],
                                      postscale = config['K0s2mmLinePostscale'],
                                      algos = [ self.L02PiE ],
                                      #RequiredRawEvents = ["Muon"],
                                      MDSTFlag = True
                                                 )
          # 5 : register Line

    ## Add some fake lines to eat the timing of particle containers
        #self.registerLine(self._Pi0MMsignalLine)
        #self.registerLine(self._K0s24ProngLFVLine)
        #self.registerLine(self._Lambda02PiMuLine)
        

        self.registerLine(self.Pi0MMsignalLine)
        self.registerLine(self.Pi0MMsidebandLine)

        self.registerLine(self.K0s2PiPiLine)
        self.registerLine(self.K0s2MuMuLine)
        self.registerLine(self.K0s2MuMuSBLine)
        self.registerLine(self.K0s24ProngLFVLine)
        self.registerLine(self.K0s2e3MuLine)
        self.registerLine(self.K0s2Mu3eLine)
        self.registerLine(self.K0s2eMuLine)
        
        self.registerLine(self.Lambda02PiMuLine)
        self.registerLine(self.Lambda02Pi3MuLine)
        self.registerLine(self.Lambda02PiMuMuELine)
        self.registerLine(self.Lambda02pPiLine)
        self.registerLine(self.Lambda02PiELine)
        
        
#####################################################
    def _filterMuons( self, name, sel):
        """
        Filter for all hadronic final states
        """
        # requires all basic particles to have IPCHI2 > KaonIPCHI2
        # and hadron PT > KaonPT

        _Code = "(MIPCHI2DV(PRIMARY) > %(muIpChi2)s)" \
                % self.config

        _Filter = FilterDesktop(Code = _Code)

        return Selection( name, Algorithm = _Filter, RequiredSelections = [  sel ] )
    def _filterPions( self, name, sel):
        """
        Filter for all hadronic final states
        """
        # requires all basic particles to have IPCHI2 > KaonIPCHI2
        # and hadron PT > KaonPT

        _Code = "(MIPCHI2DV(PRIMARY) > %(piIpChi2)s)" \
                % self.config

        _Filter = FilterDesktop(Code = _Code)

        return Selection( name, Algorithm = _Filter, RequiredSelections = [  sel ] )
    def _filterElectrons( self, name, sel):
        """
        Filter for all hadronic final states
        """
        # requires all basic particles to have IPCHI2 > KaonIPCHI2
        # and hadron PT > KaonPT

        _Code = "(MIPCHI2DV(PRIMARY) > %(eIpChi2)s)" \
                % self.config

        _Filter = FilterDesktop(Code = _Code)

        return Selection( name, Algorithm = _Filter, RequiredSelections = [  sel ] )
    
##################################################
    def _filterProtons( self, name, sel):
        """
        Filter for all hadronic final states
        """
        # requires all basic particles to have IPCHI2 > KaonIPCHI2
        # and hadron PT > KaonPT

        _Code = "(MIPCHI2DV(PRIMARY) > %(protonIpChi2)s)" \
                % self.config

        _Filter = FilterDesktop(Code = _Code)

        return Selection( name, Algorithm = _Filter, RequiredSelections = [  sel ] )

    def _makePseudoJPsi(self,name) :
        """
        Make PseudoJpsi (combination of two detached muons)
        Arguments:
        name : name of the selection
        """
        
        PseudoJPsi = CombineParticles(DecayDescriptor = "J/psi(1S) -> mu+ mu-")
        PseudoJPsi.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
        PseudoJPsi.CombinationCut = "(AMAXDOCA('')<%(DiDOCA)s*mm)" %self.config
        PseudoJPsi.MotherCut = "ALL"
       
        return Selection (name,
                          Algorithm = PseudoJPsi,
                          RequiredSelections = [self.SelMuons])
    
    def _makePseudoPhi(self,name) :
        """
        Make PseudoPhi (combination of two detached electrons)
        Arguments:
        name : name of the selection
        """
        PseudoPhi = CombineParticles(DecayDescriptor = "phi(1020) -> e+ e-")
        PseudoPhi.CombinationCut = " (AMAXDOCA('')<%(DiDOCA)s*mm)" %self.config
        PseudoPhi.MotherCut = "ALL"
        
        return Selection (name,
                          Algorithm = PseudoPhi,
                          RequiredSelections = [self.SelElectrons])
    
    def _makePseudoKstar(self,name) :
        """
        Make PseudoKst (combination of a detached e-mu pair)
        Arguments:
        name : name of the selection
        """
        PseudoKst = CombineParticles(DecayDescriptor = "[K*(892)0  -> mu+ e-]cc")
        PseudoKst.CombinationCut = " (AMAXDOCA('')<%(DiDOCA)s*mm)" %self.config
        PseudoKst.MotherCut = "ALL"
        
        return Selection (name,
                          Algorithm = PseudoKst,
                          RequiredSelections = [self.SelElectrons, self.SelMuons])

    def _makePi0MuMu(self,name, combcut) :
        """
        K0s2pi0mumu selection object in signal region.
        Arguments:
        name        : name of the Selection
        combcut : cut at combination level
        """
        from StandardParticles import StdLooseResolvedPi0 as Pi0s
        #from Configurables import OfflineVertexFitter
        K0s2Pi0MuMu1 = CombineParticles(DecayDescriptor = "KS0 -> pi0 J/psi(1S)")
        
        #K0s2Pi0MuMu1.addTool( OfflineVertexFitter )
        #K0s2Pi0MuMu1.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
        #K0s2Pi0MuMu1.OfflineVertexFitter.useResonanceVertex = False
        #K0s2Pi0MuMu1.ReFitPVs = True
        K0s2Pi0MuMu1.CombinationCut = combcut

        K0s2Pi0MuMu1.MotherCut = "((BPVDIRA> %(KSdira)s ) "\
                                 "& ((BPVVDSIGN*M/P) > %(KSlife)s) "\
                                 "& (MIPDV(PRIMARY)<%(KSip)s*mm) "\
                                 "& (M> %(KSsignalminMass)s * MeV) "\
                                 "& ( (M< %(KSsignalmaxMass)s * MeV) ))" %self.config

        return Selection (name,
                          Algorithm = K0s2Pi0MuMu1,
                          RequiredSelections = [ self.SelDiMuons,
                                                 Pi0s])

    def _makePi0MuMu_Signal(self,name) :
        """
        K0s2pi0mumu selection object in signal region.
        Arguments:
        name        : name of the Selection
        """
        

        return self._makePi0MuMu( name, combcut ="  (AM > %(KSsignalminMass)s * MeV)"\
                                      "& (AM < %(KSsignalmaxMass)s * MeV)" %self.config )


    def _makePi0MuMu_SB(self,name) :
        """
        K0s2pi0mumu selection object in sideband region.
        Arguments:
        name        : name of the Selection
        """
        

        return self._makePi0MuMu( name, combcut = "  (AM > %(KSsidebminMass)s * MeV)"\
                                      "& (AM < %(KSsidebmaxMass)s * MeV)" %self.config)



    def _makeK0s2TwoProngs(self, name,decay_descriptor, combcut, masscut) :
        """
        K0s2mumu selection object
        with muon Id and wide mass window
        starts from Phys/StdLooseMuons

        Please contact Diego Martinez Santos if you think of prescaling this line!
    
        Arguments:
        name        : name of the Selection.
        """
        if "mu+" in decay_descriptor:
            dname = "mu+"
            daughters = self.SelMuons
        else:
            dname = "pi+"
            daughters = self.SelPions 
        K0s2MuMu = CombineParticles(DecayDescriptor = decay_descriptor)
    
        K0s2MuMu.DaughtersCuts = { dname : "(MIPCHI2DV(PRIMARY)> 100.)"}
        K0s2MuMu.CombinationCut = combcut + "& (AMAXDOCA('')<%(DiDOCA)s*mm)"% self.config

        K0s2MuMu.MotherCut = "("+ masscut + "& (BPVDIRA>0) & ((BPVVDSIGN*M/P) > %(KSlife)s) & (MIPDV(PRIMARY)<0.4*mm))" %self.config
    
        return Selection (name,
                          Algorithm = K0s2MuMu,
                          RequiredSelections = [ daughters])


    def _makeLambda2TwoProngs(self, name,decay_descriptor, combcut, masscut) :
        """
        K0s2mumu selection object
        with muon Id and wide mass window
        starts from Phys/StdLooseMuons

        Please contact Diego Martinez Santos if you think of prescaling this line!
    
        Arguments:
        name        : name of the Selection.
        """
        if "mu+" in decay_descriptor: daughter1 = self.SelMuons
        else: daughter1 = self.SelProtons 
        Lambda2TwoProngs = CombineParticles(DecayDescriptor = decay_descriptor)
    
        Lambda2TwoProngs.CombinationCut = combcut + "& (AMAXDOCA('')<%(DiDOCA)s*mm)"% self.config

        Lambda2TwoProngs.MotherCut = "("+ masscut + "& (BPVDIRA>0) & ((BPVVDSIGN*M/P) > %(L0life)s) & (MIPDV(PRIMARY)<0.4*mm))" %self.config
    
        return Selection (name,
                          Algorithm = Lambda2TwoProngs,
                          RequiredSelections = [ daughter1, self.SelPions])

    
    def _makeMultibody(self, name,decay_descriptor, combcut, masscut) :
        """
        K0s2mumu selection object
        with muon Id and wide mass window
        starts from Phys/StdLooseMuons

        Please contact Diego Martinez Santos if you think of prescaling this line!
    
        Arguments:
        name        : name of the Selection.
        """
        daughters = []
        if "mu" in decay_descriptor: daughters += [self.SelMuons]
        if "pi" in decay_descriptor: daughters += [self.SelPions]
        if "e+" or "e-" in decay_descriptor: daughters += [self.SelElectrons]
        if "K*(892)0" in decay_descriptor:
            if not "SelEMu" in dir(self): self.SelEMu = self._makePseudoKstar("SelEMuFor" + self._name)
            daughters += [self.SelEMu]
        if "phi(1020)" in decay_descriptor:
            if not "SelDiElectrons" in dir(self): self.SelDiElectrons = self._makePseudoPhi("SelDiElectronsFor" + self._name)
            daughters += [self.SelDiElectrons]
                
        if "J/psi(1S)" in decay_descriptor:
            if not "SelDiMuons" in dir(self): self.SelDiMuons = self._makePseudoJpsi("SelDiMuonsFor" + self._name)
            daughters += [self.SelDiMuons]
            
        if "KS0" in decay_descriptor : lifecut = "%(KSlife)s" %self.config
        elif "Lambda0" in decay_descriptor: lifecut= "%(L0life)s" %self.config
        
        Multibody = CombineParticles(DecayDescriptor = decay_descriptor)
    
       
        Multibody.CombinationCut = combcut #+ "& (AMAXDOCA('')<%(DiDOCA)s*mm)"% self.config

        Multibody.MotherCut = "("+ masscut + "& (BPVDIRA>0) & ((BPVVDSIGN*M/P) >"+ lifecut +") & (MIPCHI2DV(PRIMARY)< %(MultibodyIPChi2)s) & (VFASPF(VCHI2/VDOF)<%(MultibodyChi2dof)s))" %self.config
    
        return Selection (name,
                          Algorithm = Multibody,
                          RequiredSelections = daughters)

#####################################################
#a = RnSConf("shittyname", default_config['CONFIG']) ## Test if the class works before runnung the test script!
