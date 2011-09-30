__author__ = 'Ulrich Kerzel'
__date__ = '05/07/2011'
__version__ = '$Revision: 1.3 $'

"""
ChiC -> JPsi Gamma -> Mu Mu e e
"""

__author__ = ['Ulrich Kerzel']
__date__ = '05/07/2011'
__version__ = '$Revision: 1.1 $'

__all__ = ('StrippingChiCJPsiGammaConversionConf',
           'makeMuMu')

#config_params =  {
#    'trackChi2'               :    5.0
#    , 'JPsiMassMin'           :    3.04 # GeV
#    , 'JPsiMassMax'           :    3.14 # GeV
#    , 'JPsiVertexChi2'        :   25.0
#    , 'NBCutJPsi'             :    0.7
#    , 'ExpertiseJPsi'         : 'Muon/mumu_net_noip.nb'
#    , 'NBVersionJPsi'         : "TuneSep2010"
#    , 'eDLLe'                 :  -5.0
#    , 'GammaEEMass'           :   7.0 #MeV
#    , 'GammaEEChi2'           :   2.0
#    , 'GammaEETau'            :   0.1  #ps
#    , 'mMinChiCRaw'           :   2.9 #GeV
#    , 'mMaxChiCRaw'           :   4.1 #GeV
#    , 'mMinChiCFit'           :   3.0 #GeV
#    , 'mMaxChiCFit'           :   4.0 #GeV
#    , 'PrescaleChiC'          :   1.0
#    , 'PostscaleChiC'         :   1.0
#    }

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder



# make (mu mu) neurobayes network available
from Configurables import StrippingNBMuMu


class StrippingChiCJPsiGammaConversionConf(LineBuilder):
    """
    Stripping selection for ChiC -> JPsi Gamma
    where JPsi -> mu mu selected by the NeuroBayes neural network package
    and Gamma -> e+ e- (i.e. conversions before the magnet)
    
    N.B. For technical reasons the photon is currently implemented using the KShort
    """
    
    __configuration_keys__ = ( 
        'trackChi2'           #    :    5.0
        , 'JPsiMassMin'       #    :    3.04 # GeV
        , 'JPsiMassMax'       #    :    3.14 # GeV
        , 'JPsiVertexChi2'    #    :   25.0
        , 'NBCutJPsi'         #    :    0.7
        , 'ExpertiseJPsi'     #    : 'Muon/mumu_net_noip.nb'
        , 'NBVersionJPsi'     #    : "TuneSep2010"
        , 'GammaEEMass'       #    :   7.0 #MeV
        , 'GammaEEChi2'       #    :   2.0
        , 'GammaEETau'        #    :   0.02
        , 'eDLLe'             #    :  -5.0
        , 'mMinChiCRaw'       #    :   2.9 GeV
        , 'mMaxChiCRaw'       #    :   4.1 GeV
        , 'mMinChiCFit'       #    :   3.0 GeV
        , 'mMaxChiCFit'       #    :   4.0 GeV
        , 'PrescaleChiC'
        , 'PostscaleChiC'
        )



    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self.SelJPsi           = self.MuMu(name+"JPsi",
                                           massMin    = config['JPsiMassMin'],
                                           massMax    = config['JPsiMassMax'],
                                           trackChi2  = config['trackChi2'],
                                           vertexChi2 = config['JPsiVertexChi2']
                                           )

        self.SelJPsiNB         = self.MuMuNB(name+"JPsiNB",
                                             self.SelJPsi  ,
                                             Expertise     = config['ExpertiseJPsi'],
                                             NBVersion     = config['NBVersionJPsi'],
                                             NBCut         = config['NBCutJPsi'],
                                             massMin       = config['JPsiMassMin'],
                                             massMax       = config['JPsiMassMax'],
                                             nBins         = 120,
                                             doPlot        = False)

        self.SelGammaEE        = self.GammaEE(name+"GammaEE",
                                              eDLLe        = config['eDLLe'],
                                              GammaEEChi2  = config['GammaEEChi2'],
                                              GammaEEMass  = config['GammaEEMass'],
                                              GammaEETau   = config['GammaEETau'])

        self.SelChiC           = self.ChiC(name+"ChiC",
                                           self.SelJPsiNB,
                                           self.SelGammaEE,
                                           mMinChiCRaw = config['mMinChiCRaw'],       
                                           mMaxChiCRaw = config['mMaxChiCRaw'],      
                                           mMinChiCFit = config['mMinChiCFit'],       
                                           mMaxChiCFit = config['mMaxChiCFit'])              
            

        self.ChiCLine           = StrippingLine(name+"ChiCConversionLine",
                                                prescale  = config['PrescaleChiC'],
                                                postscale = config['PostscaleChiC'],
                                                selection = self.SelChiC)

        self.registerLine(self.ChiCLine)


    #####################################################################################################################
    #    
    # create candidates J/Psi -> mu mu
    #
    ######################################################################################################################  
    def MuMu( self, Name, massMin, massMax, trackChi2, vertexChi2):
        from GaudiKernel.SystemOfUnits import GeV

        muonCut   = "(TRCHI2DOF< %(trackChi2)s ) & (ISMUON)"                                                 % locals()
        combCut   = "(AM> %(massMin)s *GeV) & (AM< %(massMax)s*GeV)"                                         % locals()
        motherCut = "(VFASPF(VCHI2/VDOF)< %(vertexChi2)s) &(MM > %(massMin)s *GeV) & (MM < %(massMax)s*GeV)" % locals()
        
        from StandardParticles import StdLooseMuons
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        muonFilter = FilterDesktop(Code = muonCut)
        myMuons    = Selection(Name+'_MuonSel', Algorithm = muonFilter, RequiredSelections = [StdLooseMuons])
        
        _DiMu = CombineParticles (DecayDescriptor = "J/psi(1S) -> mu+ mu-" ,
                                  CombinationCut  = combCut,
                                  MotherCut       = motherCut)
        DiMuSel = Selection(Name+"_DiMuonSel", Algorithm = _DiMu, RequiredSelections = [myMuons])
        
        return DiMuSel

    ######################################################################################################################  
    #
    # NeuroBayes for J/Psi -> MuMu
    #
    ######################################################################################################################  
    def MuMuNB(self, Name, inputSel, Expertise, NBVersion, NBCut, massMin, massMax, nBins, doPlot):
        name = "mumuNB"+Name
        mumuNB                    = StrippingNBMuMu(name)
        mumuNB.PlotHisto          = doPlot
        mumuNB.PlotMassMin        = massMin
        mumuNB.PlotMassMax        = massMax
        mumuNB.PlotNBins          = nBins
        mumuNB.Expertise          = Expertise
        mumuNB.NetworkVersion     = NBVersion
        mumuNB.NetworkCut         = NBCut
                                                        
        nbSel = Selection("NeuroBayesSel"+Name ,
                          Algorithm = mumuNB,
                          RequiredSelections = [inputSel]
                          )
        
        return nbSel
            

    ######################################################################################################################  
    #
    # create conversion candidates: gamma-> e+ e-
    #   implemented using KShort for the photon
    #
    ######################################################################################################################  
    def GammaEE(self, Name, eDLLe, GammaEEChi2, GammaEEMass, GammaEETau):

        # cuts
        daughterCut =  "PIDe > %(eDLLe)s"                                                                                                % locals()
        motherCut   =  "(VFASPF(VCHI2/VDOF)<%(GammaEEChi2)s) & (MM < %(GammaEEMass)s*MeV) & (abs(BPVLTIME()) <   %(GammaEETau)s*ps )"    % locals()
        
        

        # imports
        from PhysSelPython.Wrappers import MergedSelection

        # all required electrons
        from CommonParticles import NoBremNoPIDsElectrons
        from CommonParticles import NoBremNoPIDsDownElectrons
        from CommonParticles import NoBremNoPIDsUpElectrons
        from CommonParticles import NoBremNoPIDsVeloElectrons 

        eLong        = DataOnDemand('Phys/NoBremNoPIDsElectrons/Particles')
        eUp          = DataOnDemand('Phys/NoBremNoPIDsUpElectrons/Particles')
        eDown        = DataOnDemand('Phys/NoBremNoPIDsDownElectrons/Particles')
        eTtrack      = DataOnDemand('Phys/NoBremNoPIDsTtrackElectrons/Particles')
        allElectrons = MergedSelection('allElectrons', RequiredSelections = [eLong, eUp, eDown, eTtrack])
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        electronFilter = FilterDesktop(Code = daughterCut)
        myElectrons    = Selection(Name+'_MuonSel', Algorithm = electronFilter, RequiredSelections = [allElectrons])
        
        ConvFind  =  CombineParticles( DecayDescriptor  = "KS0 -> e+ e-",
                                       CombinationCut   = "(AM>0*MeV)"  ,
                                       MotherCut        = motherCut)
                                                       
        #ConvFind.DecayDescriptor  = "KS0 -> e+ e-"
        #ConvFind.DaughtersCuts    = {"e+" : "ALL" }
        #ConvFind.CombinationCut   = "(AM>0*MeV)"
        #ConvFind.MotherCut        = motherCut

        GammaEESel = Selection(Name+"_GammaEESel", Algorithm = ConvFind, RequiredSelections = [myElectrons])

        
        return GammaEESel

    ######################################################################################################################
    #
    # Build ChiC Candidates
    #
    ######################################################################################################################
    def ChiC(self, Name, inputSelJPsi, inputSelGamma, mMinChiCRaw, mMaxChiCRaw, mMinChiCFit, mMaxChiCFit):

        combCut   = "(AM > %(mMinChiCRaw)s *GeV) & (AM < %(mMaxChiCRaw)s*GeV)"  % locals()
        motherCut = "(MM > %(mMinChiCFit)s *GeV) & (MM < %(mMaxChiCFit)s*GeV)"  % locals()

        ChiC                  = CombineParticles(DecayDescriptor  = "chi_c0(1P) -> J/psi(1S) KS0",
                                                 CombinationCut   = combCut ,
                                                 MotherCut        = motherCut)

        ChiCSel = Selection(Name+"_ChiCSel", Algorithm = ChiC, RequiredSelections = [inputSelJPsi, inputSelGamma])

        return ChiCSel
        
