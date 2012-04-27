__author__ = 'Ulrich Kerzel'
__date__ = '03/06/2010'
__version__ = '$Revision: 1.3 $'

"""
Mu Mu stripping with NeuroBayes
"""

__author__ = ['Ulrich Kerzel']
__date__ = '04/02/2011'
__version__ = '$Revision: 1.1 $'

__all__ = ('StrippingNeuroBayesMuMuConf',
           'makeMuMu')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


from Configurables import StrippingNBMuMu



class StrippingNeuroBayesMuMuConf(LineBuilder):
    """
    Stripping selection for resonances decaying to two muons based
    on the NeuroBayes neural network package.
    
    N.B. The cut is placed on the transformed network output which
         lies between 0...1
         The original network output is between -1...1, i.e
         transform via (-1+cut)*0.5
    """
    
    __configuration_keys__ = ( 
        'trackChi2'           #    :    5.0
        , 'MuMuMassMin'
        , 'MuMuMassMax'
        , 'MuMuMassHighMin'
        , 'MuMuMassHighMax'        
        , 'JPsiMassMin'       #    :    3.04 # GeV
        , 'JPsiMassMax'       #    :    3.14 # GeV
        , 'vertexChi2'        #    :   25.0
        , 'NBCutJPsi'         #    :    0.7
        , 'NBCutMuMu'         #    :    0.95
        , 'NBCutMuMuHigh'     #    :    
        , 'ExpertiseJPsi'     #    : 'Muon/mumu_net_noip.nb'
        , 'ExpertiseMuMu'     #    : 'Muon/mumu_net_full.nb'
        , 'ExpertiseMuMuHigh'
        , 'NBVersionJPsi'     #    : "TuneSep2010"
        , 'NBVersionMuMu'
        , 'NBVersionMuMuHigh'
        , 'PrescaleMuMu'
        , 'PostscaleMuMu'
        , 'PrescaleMuMuHigh'
        , 'PostscaleMuMuHigh'
        , 'PrescaleJPsi'
        , 'PostscaleJPsi'
        )



    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        self.SelMuMu           = self.MuMu(name+"MuMu",
                                           massMin    = config['MuMuMassMin'],
                                           massMax    = config['MuMuMassMax'],
                                           trackChi2  = config['trackChi2'],
                                           vertexChi2 = config['vertexChi2']
                                           )
        self.SelMuMuHigh       = self.MuMu(name+"MuMuHigh",
                                           massMin    = config['MuMuMassHighMin'],
                                           massMax    = config['MuMuMassHighMax'],
                                           trackChi2  = config['trackChi2'],
                                           vertexChi2 = config['vertexChi2']
                                           )
        
        self.SelJPsi           = self.MuMu(name+"JPsi",
                                           massMin = config['JPsiMassMin'],
                                           massMax = config['JPsiMassMax'],
                                           trackChi2  = config['trackChi2'],
                                           vertexChi2 = config['vertexChi2']
                                           )

        self.SelMuMuNB         = self.MuMuNB(name+"MuMuNB",
                                             self.SelMuMu  ,
                                             Expertise     = config['ExpertiseMuMu'],
                                             NBVersion     = config['NBVersionMuMu'],
                                             NBCut         = config['NBCutMuMu'],
                                             massMin       = config['MuMuMassMin'],
                                             massMax       = config['MuMuMassMax'],
                                             nBins         = 120,
                                             doPlot        = False)
        
        self.SelMuMuHighNB     = self.MuMuNB(name+"MuMuHighNB",
                                             self.SelMuMuHigh  ,
                                             Expertise     = config['ExpertiseMuMuHigh'],
                                             NBVersion     = config['NBVersionMuMuHigh'],
                                             NBCut         = config['NBCutMuMuHigh'],
                                             massMin       = config['MuMuMassHighMin'],
                                             massMax       = config['MuMuMassHighMax'],
                                             nBins         = 120,
                                             doPlot        = False)

        self.SelJPsiNB         = self.MuMuNB(name+"JPsiNB",
                                             self.SelJPsi  ,
                                             Expertise     = config['ExpertiseJPsi'],
                                             NBVersion     = config['NBVersionJPsi'],
                                             NBCut         = config['NBCutJPsi'],
                                             massMin       = config['JPsiMassMin'],
                                             massMax       = config['JPsiMassMax'],
                                             nBins         = 120,
                                             doPlot        = False)

        self.mumuLine          = StrippingLine(name+"MuMuNBLine",
                                               prescale  = config['PrescaleMuMu'],
                                               postscale = config['PostscaleMuMu'],
                                               selection = self.SelMuMuNB
                                               )
        self.mumuHighLine      = StrippingLine(name+"MuMuHighNBLine",
                                               prescale  = config['PrescaleMuMuHigh'],
                                               postscale = config['PostscaleMuMuHigh'],
                                               selection = self.SelMuMuHighNB
                                               )        
        
        self.jPsiLine          = StrippingLine(name+"JPsiNBLine",
                                               prescale  = config['PrescaleJPsi'],
                                               postscale = config['PostscaleJPsi'],
                                               selection = self.SelJPsiNB
                                               )
                
        self.registerLine(self.mumuLine)
        self.registerLine(self.mumuHighLine)
        self.registerLine(self.jPsiLine)


    #
    # create candidates
    #
    def MuMu( self, Name, massMin, massMax, trackChi2, vertexChi2):
        from GaudiKernel.SystemOfUnits import GeV

        muonCut   = "(TRCHI2DOF< %(trackChi2)s ) & (ISMUON)"                                                 % locals()
        combCut   = "(AM> %(massMin)s *GeV) & (AM< %(massMax)s*GeV)"                                         % locals()
        motherCut = "(VFASPF(VCHI2/VDOF)< %(vertexChi2)s) &(MM > %(massMin)s *GeV) & (MM < %(massMax)s*GeV)" % locals()
        
        from StandardParticles import StdAllLooseMuons
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        muonFilter = FilterDesktop(Code = muonCut)
        myMuons    = Selection(Name+'_MuonSel', Algorithm = muonFilter, RequiredSelections = [StdAllLooseMuons])
        
        _DiMu = CombineParticles (DecayDescriptor = "J/psi(1S) -> mu+ mu-" ,
                                  CombinationCut  = combCut,
                                  MotherCut       = motherCut)
        DiMuSel = Selection(Name+"_DiMuonSel", Algorithm = _DiMu, RequiredSelections = [myMuons])
        
        return DiMuSel

    #
    # NeuroBayes 
    #
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
            
    
