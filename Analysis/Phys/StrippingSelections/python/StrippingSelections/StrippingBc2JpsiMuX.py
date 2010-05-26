__author__ = 'Jibo He'
__date__ = '21/01/2009'
__version__ = '$Revison: 1.0 $'

"""
Bc->Jpsi(MuMu)MuX selections,
following StrippingBu2LLK and StrippingBs2JpsiPhi
"""

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
      
class StrippingBc2JpsiMuXConf(LHCbConfigurableUser):
    """
    Definition of Bc->Jpsi(MuMu)MuX stripping 
    """
    __slots__ = {
          'BcUpperMass'         : 7000  # MeV, Upper limit for partial rec. 
        , 'BcLowerMass'         : 3000  # MeV, Lower limit for partial rec.
        , 'BcVtxCHI2'           : 9     # adimentional
        , 'BcPT'                : 0  # MeV, May incrase up to 5000 MeV if needed
        , 'JpsiMassWindow'      : 200   # MeV, 10 sigma, may decreased to 150
        , 'JpsiVtxCHI2'         : 9     # adimentional
        , 'MuonJpsiPT'          : 1200  # MeV 
        , 'MuonBcPT'            : 2000  # MeV
        , 'TrackCHI2DOF'        : 9     # adimentional 
        }

    def MuMu(self):
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        StripBc2JpsiMuX = self.combineBc()
        return StrippingLine('Bc2JpsiMuX', prescale = 1, algos = [ StripBc2JpsiMuX ] )        
    
    def combineBc(self):
        """
        Recontruct Bc from Jpsi and Mu
        """

        name = "Bc2JpsiMuX"

        from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
        from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

        # Jpsi Cut
        # Which is faster, INTREE or CHILDCUT?
        # JpsiCut = "(MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(TrackCHI2DOF)s) & (MINTREE('mu+'==ABSID,PT) > %(MuonJpsiPT)s *MeV) & (ADMASS('J/psi(1S)') < %(JpsiMassWindow)s *MeV) & (VFASPF(VCHI2/VDOF) < %(JpsiVtxCHI2)s)" % self.getProps()
        JpsiCut = "(CHILDCUT((TRCHI2DOF < %(TrackCHI2DOF)s),1)) & (CHILDCUT((TRCHI2DOF < %(TrackCHI2DOF)s),2)) & (CHILDCUT((PT > %(MuonJpsiPT)s *MeV),1))  & (CHILDCUT((PT > %(MuonJpsiPT)s *MeV),2)) & (ADMASS('J/psi(1S)') < %(JpsiMassWindow)s *MeV) & (VFASPF(VCHI2/VDOF) < %(JpsiVtxCHI2)s)" % self.getProps()

        # MuBc Cut
        MuonBcCut = "(PT > %(MuonBcPT)s *MeV) & (TRCHI2DOF < %(TrackCHI2DOF)s)" % self.getProps()
        
        # Comb cut
        combCut = "(%(BcLowerMass)s *MeV < AM) & (AM < %(BcUpperMass)s *MeV)" % self.getProps()
        
        # Bc Cut
        BcCut = "(VFASPF(VCHI2/VDOF)< %(BcVtxCHI2)s ) & (PT > %(BcPT)s *MeV)" % self.getProps()

        #---------------------------
        # J/psi -> Mu Mu
        #---------------------------
        _Jpsi2MuMu = DataOnDemand(Location = "Phys/StdLooseDiMuon")
        
        _JpsiFilter = FilterDesktop("_JpsiFilter")
        _JpsiFilter.Code = JpsiCut
        
        SelJpsi2MuMu = Selection("SelJpsi2MuMu_"+name,
                                 Algorithm = _JpsiFilter,
                                 RequiredSelections = [ _Jpsi2MuMu ])
        
        #---------------------------
        # Muon
        #---------------------------        
        #_StdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons")
        _StdLooseMuons = DataOnDemand(Location = "Phys/StdNoPIDsMuons")
        
        _MuonBcFilter = FilterDesktop("_MuonBcFilter")
        _MuonBcFilter.Code = MuonBcCut
        
        SelMuonBc = Selection("SelMuonBc_"+name,
                              Algorithm = _MuonBcFilter,
                              RequiredSelections = [ _StdLooseMuons ])
        
        #---------------------------
        # Bc -> J/psi(MuMu) Mu X
        #---------------------------
        _Bc2JpsiMuX = CombineParticles( "_"+name,
                                        DecayDescriptor = "[ B_c+ -> J/psi(1S) mu+ ]cc",
                                        CombinationCut = combCut,
                                        MotherCut = BcCut,
                                        ReFitPVs = True )

        # Set the OfflineVertexFitter to keep the 3 tracks and not the J/Psi Pi 
        _Bc2JpsiMuX.addTool( OfflineVertexFitter() )
        _Bc2JpsiMuX.VertexFitters.update( { "" : "OfflineVertexFitter"} )
        _Bc2JpsiMuX.OfflineVertexFitter.useResonanceVertex = False
        
        SelBc2JpsiMuX = Selection ( "Sel"+name,
                                    Algorithm = _Bc2JpsiMuX,
                                    RequiredSelections = [ SelJpsi2MuMu, SelMuonBc ] )
        
        SeqBc2JpsiMuX = SelectionSequence("Seq"+name, TopSelection = SelBc2JpsiMuX )
        return SeqBc2JpsiMuX

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
