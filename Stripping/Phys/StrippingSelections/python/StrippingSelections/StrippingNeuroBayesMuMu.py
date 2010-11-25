__author__ = 'Ulrich Kerzel'
__date__ = '03/06/2010'
__version__ = '$Revision: 1.3 $'

"""
Mu Mu stripping with NeuroBayes
"""
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import FilterDesktop, CombineParticles, LoKi__VoidFilter
import GaudiKernel.SystemOfUnits as Units
from GaudiKernel.PhysicalConstants import c_light
from PhysSelPython.Wrappers import SelectionSequence
from PhysSelPython.Wrappers import Selection, DataOnDemand
from Configurables import StrippingNBMuMu



class StrippingNeuroBayesMuMuConf(LHCbConfigurableUser):
    """
    Stripping selection for resonances decaying to two muons based
    on the NeuroBayes neural network package.
    
    N.B. The cut is placed on the transformed network output which
         lies between 0...1
         The original network output is between -1...1, i.e
         transform via (-1+cut)*0.5
    """
    
    __slots__ = { 
        'TrackChi2'               :    5.0
        , 'SpdMult'               :99999.0
        , 'JPsiMassMin'           :    3.04 # GeV
        , 'JPsiMassMax'           :    3.14 # GeV
        , 'VertexChi2'            :   25.0
        , 'NBCutJPsi'             :    0.7
        , 'ExpertiseJPsi'         : 'Muon/mumu_net_noip.nb'
        , 'NBCutMuMu'             :    0.95
        , 'ExpertiseMuMu'         : 'Muon/mumu_net_full.nb'        
        , 'NetworkVersion'        : "TuneSep2010"
        }


    def JPsi (self ):
        jPsiCand = CombineParticles("jPsiCand")
        jPsiCand.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
        jPsiCand.DaughtersCuts   = {"mu+" : "(TRCHI2DOF< %(TrackChi2)s ) & (ISMUON)" % self.getProps() }
        jPsiCand.CombinationCut  = "(AM> %(JPsiMassMin)s *GeV) & (AM< %(JPsiMassMax)s*GeV)" % self.getProps() 
        jPsiCand.MotherCut       = "(VFASPF(VCHI2/VDOF)< %(VertexChi2)s) &(MM > %(JPsiMassMin)s *GeV) & (MM < %(JPsiMassMax)s*GeV)" % self.getProps()

        muons =  DataOnDemand(Location = 'Phys/StdLooseMuons' )

        jPsiSel = Selection("jPsiSel" ,
                            Algorithm = jPsiCand,
                            RequiredSelections = [ muons ]
                            )

        return jPsiSel

    def MuMu (self ):
        mumuCand = CombineParticles("mumuCand")
        mumuCand.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
        mumuCand.DaughtersCuts   = {"mu+" : "(TRCHI2DOF< %(TrackChi2)s ) & (ISMUON)" % self.getProps() }
        mumuCand.CombinationCut  = "AM>0"  
        mumuCand.MotherCut       = "(VFASPF(VCHI2/VDOF)< %(VertexChi2)s)" % self.getProps()

        muons =  DataOnDemand(Location = 'Phys/StdLooseMuons' )

        mumuSel = Selection("mumuSel" ,
                            Algorithm = mumuCand,
                            RequiredSelections = [ muons ]
                            )

        return mumuSel


    def NeuroBayesStrip( self, id, RequiredSel, Expertise, NeuroBayesCut, min, max, nBins):

        mumuNB                    = StrippingNBMuMu("mumuNB"+id)
        #mumuNB.OutputLevel        = 2
        mumuNB.PlotHisto          = False
        mumuNB.PlotMassMin        = min
        mumuNB.PlotMassMax        = max
        mumuNB.PlotNBins          = nBins
        mumuNB.Expertise          = Expertise
        mumuNB.NetworkVersion     = self.getProp ( 'NetworkVersion')
        mumuNB.NetworkCut         = NeuroBayesCut

        #print "NeuroBayesStrip %s settings, Expertise %s, cut %lf, mass min %lf, max %lf" %(id, Expertise, NeuroBayesCut, min, max )

        nbSel = Selection("NeuroBayesSel"+id ,
                          Algorithm = mumuNB,
                          RequiredSelections = RequiredSel
                          )

        return nbSel


    def lines( self ) :

        jPsiSel       = self.JPsi()
        mumuSel       = self.MuMu()

        expertiseJPsi = self.getProp ( 'ExpertiseJPsi' )
        nbCutJPsi     = self.getProp ( 'NBCutJPsi')
        expertiseMuMu = self.getProp ( 'ExpertiseMuMu' )
        nbCutMuMu     = self.getProp ( 'NBCutMuMu')         
        jPsiMin       = self.getProp ( 'JPsiMassMin')
        jPsiMax       = self.getProp ( 'JPsiMassMax')
 
        nbJPsiSel = self.NeuroBayesStrip( "JPsi", [ jPsiSel ] , expertiseJPsi, nbCutJPsi, jPsiMin, jPsiMax ,  120)
        nbMuMuSel = self.NeuroBayesStrip( "MuMu", [ mumuSel ] , expertiseMuMu, nbCutMuMu,     0.0,     12.0, 1000)


        jPsiLine = StrippingLine('NeuroBayesJPsiLine',
                                 prescale = 1.0,  postscale = 1.0,
                                 algos = [ nbJPsiSel ])

        mumuLine = StrippingLine('NeuroBayesMuMuLine',
                                 prescale = 1.0,  postscale = 1.0,
                                 algos = [ nbMuMuSel ])
        
                              
                              

        return [ jPsiLine, mumuLine ]
        
        

    def SpdMultFilter(self):
        return LoKi__VoidFilter("SpdMultFilter",
                                Code = "( CONTAINS('Raw/Spd/Digits')<%(SpdMult)s )" % self.getProps()
                                ) 
        

    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
