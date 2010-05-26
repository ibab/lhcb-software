__author__ = 'Jibo He'
__date__ = '21/01/2009'
__version__ = '$Revison: 1.0 $'

"""
Bc->Jpsi(MuMu)H selections,
following StrippingBu2LLK and StrippingBs2JpsiPhi
"""

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
      
class StrippingBc2JpsiHDetachedConf(LHCbConfigurableUser):
    """
    Definition of Bc->Jpsi(MuMu)H Detached stripping 
    """
    __slots__ = {
        'BcMassWindow'          : 400   # MeV, Mass window for Bc 
        , 'BcVtxCHI2'           : 9     # adimentional
        , 'BcPT'                : 0     # MeV, May incrase up to 5000 MeV if needed
        , 'JpsiMassWindow'      : 100   # MeV, 10 sigma, may decreased to 150
        , 'JpsiVtxCHI2'         : 9     # adimentional
        , 'MuonJpsiPT'          : 800   # MeV 
        , 'PionPT'              : 1200  # MeV
        , 'PionIPCHI2'          : 4     # adimentional
        , 'JpsiIPCHI2'          : -1    # adimentional, choose one  
        , 'MuonIPCHI2'          : -1    # adimentional, choose one 
        , 'BcIPCHI2'            : 25    # adimentional
        , 'TrackCHI2DOF'        : 9     # adimentional 
        }

    def MuMu(self):
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        StripBc2JpsiHDetached = self.combineBc()
        return StrippingLine('Bc2JpsiHDetached', prescale = 1, algos = [ StripBc2JpsiHDetached ] )        
    
    def combineBc(self):
        """
        Recontruct Bc from Jpsi and H
        """

        name = "Bc2JpsiHDetached"
        
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

        # Lifetime biased cut
        # Here INTREE almost the same as CHILDCU   
        LifetimeBiasedCut = "(MINTREE('pi+'==ABSID,BPVIPCHI2()) > %(PionIPCHI2)s) & (MINTREE('mu+'==ABSID,BPVIPCHI2()) > %(MuonIPCHI2)s) & (MINTREE('J/psi(1S)'==ABSID,BPVIPCHI2()) > %(JpsiIPCHI2)s) & (BPVIPCHI2() < %(BcIPCHI2)s)" % self.getProps()
        #LifetimeBiasedCut = "(CHILDCUT((BPVIPCHI2() > %(PionIPCHI2)s), 2)) & (CHILDCUT((BPVIPCHI2() > %(MuonIPCHI2)s),1,1)) & (CHILDCUT((BPVIPCHI2() > %(MuonIPCHI2)s),1,2)) & (CHILDCUT((BPVIPCHI2() > %(JpsiIPCHI2)s), 1)) & (BPVIPCHI2() < %(BcIPCHI2)s)" % self.getProps()

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
        _Bc2JpsiHDetached = CombineParticles( "_"+name,
                                              DecayDescriptor = "[ B_c+ -> J/psi(1S) pi+ ]cc",
                                              CombinationCut = combCut,
                                              MotherCut = BcCut + ' & ' + LifetimeBiasedCut,
                                              ReFitPVs = True )
        
        # Set the OfflineVertexFitter to keep the 3 tracks and not the J/Psi Pi 
        _Bc2JpsiHDetached.addTool( OfflineVertexFitter() )
        _Bc2JpsiHDetached.VertexFitters.update( { "" : "OfflineVertexFitter"} )
        _Bc2JpsiHDetached.OfflineVertexFitter.useResonanceVertex = False

        SelBc2JpsiHDetached = Selection ( "Sel"+name,
                                          Algorithm = _Bc2JpsiHDetached,
                                          RequiredSelections = [ SelJpsi2MuMu, SelPion ] )
        
        SeqBc2JpsiHDetached = SelectionSequence("Seq"+name, TopSelection = SelBc2JpsiHDetached )
        return SeqBc2JpsiHDetached

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
