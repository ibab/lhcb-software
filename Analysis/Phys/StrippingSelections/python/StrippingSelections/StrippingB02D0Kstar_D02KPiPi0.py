__author__ = 'Aurelien.Martens@cern.ch'
__date__ = '23/06/2010'
__version__ = '$Revision: 1.1 $'

"""
 Bd,s -> D0 Kstar0 with D0 -> K Pi Pi0 and Kstar -> K Pi
"""

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import CombineParticles, FilterDesktop #, OfflineVertexFitter, 
from PhysSelPython.Wrappers import MultiSelectionSequence, SelectionSequence, Selection, DataOnDemand


class StrippingB02D0Kstar_D02KPiPi0Conf(LHCbConfigurableUser):
    __slots__ = {
        # Robust cuts
        'DauIPMin'                 : 0.05    # mm
        , 'KstarIPMin'             : 0.05    # mm
        , 'DVDMin'                 : 1.      # mm
        , 'DVDRMin'                : 0.2     # mm
        , 'BIPMax'                 : 0.07    # mm
        , 'BVDMin'                 : 1.      # mm
        # Chi2 cuts
        , 'DauTrackChi2Max'        : 10.     # dimensionless, per DOF
        , 'KstarDauMIPChi2Min'     : 6.25    # dimensionless, to PV
        , 'DDauMIPChi2Min'         : 4.      # dimensionless, to PV
        , 'KstarVtxChi2Max'        : 12.     # dimensionless, per DOF
        , 'KstarMIPChi2Min'        : 4.      # dimensionless, to PV
        , 'DVtxChi2Max'            : 12.     # dimensionless, per DOF
        , 'DVDChi2Min'             : 36.     # dimensionless, to PV
        , 'BVtxChi2Max'            : 10.     # dimensionless, per DOF
        , 'BMIPChi2Max'            : 12.25   # dimensionless, to PV
        , 'BVDChi2Min'             : 100.    # dimensionless, to PV
        # Kinematics
        , 'KstarDauPTMin'          : 300.    # MeV
        , 'KstarDauPMin'           : 2000.   # MeV
        , 'DDauPTMin'              : 250.    # MeV
        , 'DDauPMin'               : 2000.   # MeV
        , 'Pi0PTMin'               : 250.    # MeV
        , 'Pi0PMin'                : 2000.   # MeV
        , 'DPTMin'                 : 1000.   # MeV
        , 'KstarPTMin'             : 1000.   # MeV
        #direction angle
        , 'BDIRAMin'               : 0.9997   # dimensionless
        # Mass windows
        , 'KstarMassMin'           : 745.    # MeV
        , 'KstarMassMax'           : 1046.   # MeV
        , 'KstarCombMassMin'       : 645.    # MeV
        , 'KstarCombMassMax'       : 1146.   # MeV
        , 'DMassMin'               : 1768.   # MeV
        , 'DMassMax'               : 2068.   # MeV
        , 'DCombMassMin'           : 1668.   # MeV
        , 'DCombMassMax'           : 2168.   # MeV
        , 'BMassMin'               : 4800.   # MeV
        , 'BMassMax'               : 5900.   # MeV
        , 'BCombMassMin'           : 4700.   # MeV
        , 'BCombMassMax'           : 6000.   # MeV
        }

    def NominalResolved_Line(self):
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        B02D0Kstar_D02KPiPi0ResolvedSel = self.B02D0Kstar_Resolved()
       
        B02D0Kstar_D02KPiPi0ResolvedSeq = SelectionSequence("SeqB02D0Kstar_D02KPiPi0Resolved"
                                                            , TopSelection = B02D0Kstar_D02KPiPi0ResolvedSel
                                                            )
        return StrippingLine('B02D0Kstar_D02KPiPi0Resolved', prescale = 1,  algos = [ B02D0Kstar_D02KPiPi0ResolvedSeq  ] )
    
    def NominalMerged_Line(self):
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        B02D0Kstar_D02KPiPi0MergedSel = self.B02D0Kstar_Merged()
       
        B02D0Kstar_D02KPiPi0MergedSeq = SelectionSequence("SeqB02D0Kstar_D02KPiPi0Merged"
                                                          , TopSelection = B02D0Kstar_D02KPiPi0MergedSel
                                                          )
        return StrippingLine('B02D0Kstar_D02KPiPi0Merged', prescale = 1,  algos = [ B02D0Kstar_D02KPiPi0MergedSeq  ] )

    def NoPIDsD02KPiPi0Resolved(self):
        Kaon = DataOnDemand(Location = "Phys/StdNoPIDsKaons")
        Pion = DataOnDemand(Location = "Phys/StdNoPIDsPions")
        Pi0Resolved = DataOnDemand(Location = "Phys/StdLooseResolvedPi0")
        #D0 daughters
        DDauKineCut = "(PT> %(DDauPTMin)s *MeV) & (P> %(DDauPMin)s *MeV)" % self.getProps()
        DDauChi2Cut = "(TRCHI2DOF< %(DauTrackChi2Max)s ) & (MIPCHI2DV(PRIMARY)> %(DDauMIPChi2Min)s )" % self.getProps()
        DDauRobustCut = "(MIPDV(PRIMARY)> %(DauIPMin)s *mm)" % self.getProps()
        DDauCut = "( " + DDauKineCut + " & " + DDauChi2Cut + " & " + DDauRobustCut + " )"
        Pi0Cut = "(PT> %(Pi0PTMin)s *MeV) & (P> %(Pi0PMin)s *MeV)" % self.getProps()
        #D0
        DCombCut = "(AM> %(DCombMassMin)s *MeV) & (AM< %(DCombMassMax)s *MeV) & (APT> %(DPTMin)s *MeV)" % self.getProps()
        DCut = "(M> %(DMassMin)s *MeV) & (M< %(DMassMax)s *MeV) & (PT> %(DPTMin)s *MeV) & (VFASPF(VCHI2/VDOF)< %(DVtxChi2Max)s ) & (BPVVDCHI2> %(DVDChi2Min)s ) & (BPVVD> %(DVDMin)s *mm) & (BPVVDR> %(DVDRMin)s *mm)" % self.getProps()
        # combine particles
        _D0 = CombineParticles("NoPIDsD0ForB02D0Kstar_D02KPiPi0Resolved")
        _D0.DecayDescriptors = ["[D0 -> K- pi+ pi0]cc" ]
        _D0.DaughtersCuts = { "K-" : DDauCut, "pi+": DDauCut, "pi0": Pi0Cut }
        _D0.CombinationCut = DCombCut 
        _D0.MotherCut = DCut
        # selection
        D0 = Selection("SelNoPIDsD0ForB02D0Kstar_D02KPiPi0Resolved"
                       ,Algorithm = _D0
                       ,RequiredSelections = [ Kaon, Pion, Pi0Resolved ]
                       )
        return D0 

    def NoPIDsD02KPiPi0Merged(self):
        Kaon = DataOnDemand(Location = "Phys/StdNoPIDsKaons")
        Pion = DataOnDemand(Location = "Phys/StdNoPIDsPions")
        Pi0Merged = DataOnDemand(Location = "Phys/StdLooseMergedPi0")
        #D0 daughters
        DDauKineCut = "(PT> %(DDauPTMin)s *MeV) & (P> %(DDauPMin)s *MeV)" % self.getProps()
        DDauChi2Cut = "(TRCHI2DOF< %(DauTrackChi2Max)s ) & (MIPCHI2DV(PRIMARY)> %(DDauMIPChi2Min)s )" % self.getProps()
        DDauRobustCut = "(MIPDV(PRIMARY)> %(DauIPMin)s *mm)" % self.getProps()
        DDauCut = "( " + DDauKineCut + " & " + DDauChi2Cut + " & " + DDauRobustCut + " )"
        Pi0Cut = "(PT> %(Pi0PTMin)s *MeV) & (P> %(Pi0PMin)s *MeV)" % self.getProps()
        #D0
        DCombCut = "(AM> %(DCombMassMin)s *MeV) & (AM< %(DCombMassMax)s *MeV) & (APT> %(DPTMin)s *MeV)" % self.getProps()
        DCut = "(M> %(DMassMin)s *MeV) & (M< %(DMassMax)s *MeV) & (PT> %(DPTMin)s *MeV) & (VFASPF(VCHI2/VDOF)< %(DVtxChi2Max)s ) & (BPVVDCHI2> %(DVDChi2Min)s ) & (BPVVD> %(DVDMin)s *mm) & (BPVVDR> %(DVDRMin)s *mm)" % self.getProps()
        # combine particles
        _D0 = CombineParticles("NoPIDsD0ForB02D0Kstar_D02KPiPi0Merged")
        _D0.DecayDescriptors = ["[D0 -> K- pi+ pi0]cc" ]
        _D0.DaughtersCuts = { "K-" : DDauCut, "pi+": DDauCut, "pi0": Pi0Cut }
        _D0.CombinationCut = DCombCut 
        _D0.MotherCut = DCut
        # selection
        D0 = Selection("SelNoPIDsD0ForB02D0Kstar_D02KPiPi0Merged"
                       ,Algorithm = _D0
                       ,RequiredSelections = [ Kaon, Pion, Pi0Merged ]
                       )
        return D0

    def NoPIDsKstar2KPi(self, name = "MakeKstar2KPi"):
        _Kaon = DataOnDemand(Location = "Phys/StdNoPIDsKaons")
        _Pion = DataOnDemand(Location = "Phys/StdNoPIDsPions")
        #Kstar daughters
        KstarDauKineCut = "(PT> %(KstarDauPTMin)s *MeV) & (P> %(KstarDauPMin)s *MeV)" % self.getProps()
        KstarDauChi2Cut = "(TRCHI2DOF< %(DauTrackChi2Max)s ) & (MIPCHI2DV(PRIMARY)> %(KstarDauMIPChi2Min)s )" % self.getProps()
        KstarDauRobustCut = "(MIPDV(PRIMARY)> %(DauIPMin)s *mm)" % self.getProps()
        KstarDauCut = "( " + KstarDauKineCut + " & " + KstarDauChi2Cut + " & " + KstarDauRobustCut + " )"
        #Kstar
        KstarCombCut = "(AM> %(KstarCombMassMin)s *MeV) & (AM< %(KstarCombMassMax)s *MeV) & (APT> %(KstarPTMin)s *MeV)" % self.getProps()
        KstarCut = "(M> %(KstarMassMin)s *MeV) & (M< %(KstarMassMax)s *MeV) & (PT> %(KstarPTMin)s ) & ((VFASPF(VCHI2/VDOF)< %(KstarVtxChi2Max)s ) & (MIPCHI2DV(PRIMARY)> %(KstarMIPChi2Min)s )) & (MIPDV(PRIMARY)> %(KstarIPMin)s *mm)" % self.getProps()
        # combine particles
        Kstar2KPi = CombineParticles("NoPIDsKstar2KPi")
        Kstar2KPi.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
        Kstar2KPi.DaughtersCuts =  { "K+" : KstarDauCut,"pi+": KstarDauCut }
        Kstar2KPi.CombinationCut = KstarCombCut
        Kstar2KPi.MotherCut = KstarCut
        # selection
        Kstar = Selection("KstarFor"+name
                          ,Algorithm = Kstar2KPi
                          ,RequiredSelections = [ _Kaon, _Pion ]
                          )
        return Kstar

    def B02D0Kstar_Resolved(self):
        D0Resolved = self.NoPIDsD02KPiPi0Resolved()
        mySelection = self.NoPIDsKstar2KPi("B0Resolved")
        Kstar = mySelection.clone("SelKstarForResolved")
        #B0
        BCombCut = "(AM > %(BCombMassMin)s *MeV) & (AM < %(BCombMassMax)s *MeV)" % self.getProps()
        BCut = "(M > %(BCombMassMin)s *MeV) & (M < %(BCombMassMax)s *MeV) & (BPVDIRA > %(BDIRAMin)s ) & (VFASPF(VCHI2/VDOF)< %(BVtxChi2Max)s )  & (BPVVDCHI2> %(BVDChi2Min)s ) & (MIPCHI2DV(PRIMARY)< %(BMIPChi2Max)s ) & (BPVIP()< %(BIPMax)s *mm) & (BPVVD> %(BVDMin)s *mm)" % self.getProps()
        # combine particles
        _B0 = CombineParticles("B0ForB02D0Kstar_D02KPiPi0Resolved")
        _B0.DecayDescriptors  = ["[B0 -> D~0 K*(892)0]cc","[B0 -> D0 K*(892)0]cc" ] 
        _B0.CombinationCut = BCombCut
        _B0.MotherCut = BCut
        #selection
        B0 = Selection("SelB0ForB02D0Kstar_D02KPiPi0Resolved"
                       ,Algorithm = _B0
                       ,RequiredSelections = [D0Resolved,Kstar]
                       )
        return B0

    def B02D0Kstar_Merged(self):
        D0Merged = self.NoPIDsD02KPiPi0Merged()
        mySelection = self.NoPIDsKstar2KPi("B0Merged")
        Kstar = mySelection.clone("SelKstarForMerged")
        #B0
        BCombCut = "(AM > %(BCombMassMin)s *MeV) & (AM < %(BCombMassMax)s *MeV)" % self.getProps()
        BCut = "(M > %(BCombMassMin)s *MeV) & (M < %(BCombMassMax)s *MeV) & (BPVDIRA > %(BDIRAMin)s ) & (VFASPF(VCHI2/VDOF)< %(BVtxChi2Max)s )  & (BPVVDCHI2> %(BVDChi2Min)s ) & (MIPCHI2DV(PRIMARY)< %(BMIPChi2Max)s ) & (BPVIP()< %(BIPMax)s *mm) & (BPVVD> %(BVDMin)s *mm)" % self.getProps()
        # combine particles
        _B0 = CombineParticles("B0ForB02D0Kstar_D02KPiPi0Merged")
        _B0.DecayDescriptors  = ["[B0 -> D~0 K*(892)0]cc","[B0 -> D0 K*(892)0]cc" ] 
        _B0.CombinationCut = BCombCut
        _B0.MotherCut = BCut
        #selection
        B0 = Selection("SelB0ForB02D0Kstar_D02KPiPi0Merged"
                       ,Algorithm = _B0
                       ,RequiredSelections = [D0Merged,Kstar]
                       )
        return B0
        
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
