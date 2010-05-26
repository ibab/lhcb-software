__author__ = 'Jibo He'
__date__ = '21/01/2009'
__version__ = '$Revison: 1.0 $'

"""
Bc->Jpsi(MuMu)H selections,
following StrippingBu2LLK and StrippingBs2JpsiPhi
"""

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
      
class StrippingBc2JpsiHConf(LHCbConfigurableUser):
    """
    Definition of Bc->Jpsi(MuMu)H stripping 
    """
    __slots__ = {
        'BcMassWindow'          : 400   # MeV, Mass window for Bc 
        , 'BcVtxCHI2'           : 9     # adimentional
        , 'BcPT'                : 0     # MeV, May incrase up to 5000 MeV if needed
        , 'JpsiMassWindow'      : 100   # MeV, 10 sigma, may decreased to 150
        , 'JpsiVtxCHI2'         : 9     # adimentional
        , 'MuonJpsiPT'          : 1200  # MeV 
        , 'PionPT'              : 2000  # MeV
        , 'TrackCHI2DOF'        : 9     # adimentional 
        }

    def MuMu(self):
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        StripBc2JpsiH = self.combineBc()
        return StrippingLine('Bc2JpsiH', prescale = 1, algos = [ StripBc2JpsiH ] )        
    
    def combineBc(self):
        """
        Recontruct Bc from Jpsi and H
        """

        name = "Bc2JpsiH"
        
        from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
        from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

        # Jpsi Cut
        # Which is faster, INTREE or CHILDCUT?
        # JpsiCut = "(MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(TrackCHI2DOF)s) & (MINTREE('mu+'==ABSID,PT) > %(MuonJpsiPT)s *MeV) & (ADMASS('J/psi(1S)') < %(JpsiMassWindow)s *MeV) & (VFASPF(VCHI2/VDOF) < %(JpsiVtxCHI2)s)" % self.getProps()
        JpsiCut = "(CHILDCUT((TRCHI2DOF < %(TrackCHI2DOF)s),1)) & (CHILDCUT((TRCHI2DOF < %(TrackCHI2DOF)s),2)) & (CHILDCUT((PT > %(MuonJpsiPT)s *MeV),1))  & (CHILDCUT((PT > %(MuonJpsiPT)s *MeV),2)) & (ADMASS('J/psi(1S)') < %(JpsiMassWindow)s *MeV) & (VFASPF(VCHI2/VDOF) < %(JpsiVtxCHI2)s)" % self.getProps()
        
        # MuBc Cut
        PionCut = "(PT > %(PionPT)s *MeV) & (TRCHI2DOF < %(TrackCHI2DOF)s)" % self.getProps()
        
        # Comb cut
        combCut = "(ADAMASS('B_c+') < %(BcMassWindow)s *MeV)" % self.getProps()
        
        # Bc Cut
        BcCut = "(VFASPF(VCHI2/VDOF)< %(BcVtxCHI2)s ) & (PT > %(BcPT)s *MeV)" % self.getProps()

        #---------------------------
        # J/psi -> Mu Mu
        #---------------------------
        #_Jpsi2MuMu = DataOnDemand(Location = "Phys/StdLooseDiMuon")
        _Jpsi2MuMu = DataOnDemand(Location = "Phys/StdVeryLooseJpsi2MuMu")
        
        _JpsiFilter = FilterDesktop("_JpsiFilter")
        _JpsiFilter.Code = JpsiCut
        
        SelJpsi2MuMu = Selection("SelJpsi2MuMu_"+name,
                                 Algorithm = _JpsiFilter,
                                 RequiredSelections = [ _Jpsi2MuMu ])
        
        #---------------------------
        # Pion or Kaon
        #---------------------------        
        _StdLoosePions = DataOnDemand(Location = "Phys/StdNoPIDsPions")
        
        _PionFilter = FilterDesktop("_PionFilter")
        _PionFilter.Code = PionCut
        
        SelPion = Selection("SelPion_"+name,
                            Algorithm = _PionFilter,
                            RequiredSelections = [ _StdLoosePions ])
        
        #---------------------------
        # Bc -> J/psi(MuMu) H
        #---------------------------
        _Bc2JpsiH = CombineParticles( "_"+name,
                                      DecayDescriptor = "[ B_c+ -> J/psi(1S) pi+ ]cc",
                                      CombinationCut = combCut,
                                      MotherCut = BcCut,
                                      ReFitPVs = True )

        # Set the OfflineVertexFitter to keep the 3 tracks and not the J/Psi Pi 
        _Bc2JpsiH.addTool( OfflineVertexFitter() )
        _Bc2JpsiH.VertexFitters.update( { "" : "OfflineVertexFitter"} )
        _Bc2JpsiH.OfflineVertexFitter.useResonanceVertex = False
        
        SelBc2JpsiH = Selection ( "Sel"+name,
                                  Algorithm = _Bc2JpsiH,
                                  RequiredSelections = [ SelJpsi2MuMu, SelPion ] )
        
        SeqBc2JpsiH = SelectionSequence("Seq"+name, TopSelection = SelBc2JpsiH )
        return SeqBc2JpsiH

    def getProps(self):
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        Copied from StrippingBu2LLK
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
