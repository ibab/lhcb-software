__author__ = 'Walter Bonivento'
__date__ = '22/10/2010'
__version__ = '$Revision: 1.0 $'

"""
 Jpsi -> p pbar
"""

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingJpsippForD0MuMuConf(LHCbConfigurableUser):
    __slots__ = {
                  # for D0mumu
                  'Prescale_JpsippForD0mumu' : 0.3
                  ,'JpsiMassWin'           : 50.       # MeV
                  ,'doca'               : 0.1        # mm
                  ,'XminPT'             : 750.       # MeV
                  ,'XmaxPT'             : 1100.      # MeV
                  ,'XminP'              : 5000.      # MeV
                  ,'XTrackChi2'         : 5.        # adimensional
                  ,'XTrackChi2Pi'         : 7.        # adimensional
                  ,'XminIPChi2'         : 3        # adimensional
                  ,'XmaxIPChi2'         : 8        # adimensional
                  ,'DMinFlightChi2'    :  20.
                  ,'DDira'              : 0.9997     # adimensional
                  ,'D0MinPT'            : 1800.      # MeV
                  ,'DMaxIPChi2'        :15.
                  ,'DVChi2'            :10. 
                  }


    def ForD0mumu_Line(self):
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        StripJpsippForD0mumu = self.combineJpsippForD0mumu()
        return StrippingLine('JpsippForD0mumu', prescale = self.getProps()[ "Prescale_JpsippForD0mumu" ] ,  algos = [ StripJpsippForD0mumu ] )
   
        

    def combineJpsippForD0mumu(self):
        from Configurables import CombineParticles
        from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence
        
        #---------------------------
        # for MUMU Selections 
        #---------------------------
        ProtonForD0mumuCut = "(PT> %(XminPT)s *MeV) & (P>%(XminP)s *MeV) & (TRCHI2DOF<%(XTrackChi2)s) & (MIPCHI2DV(PRIMARY)> %(XminIPChi2)s) " % self.getProps()
        MomForD0mumuCut =  "(BPVDIRA> %(DDira)s) & (INGENERATION( (BPVIPCHI2()>%(XmaxIPChi2)s),1 ) ) & (BPVVDCHI2> %(DMinFlightChi2)s) & (MIPCHI2DV(PRIMARY)< %(DMaxIPChi2)s) & (VFASPF(VCHI2/VDOF)< %(DVChi2)s)" % self.getProps()
        CombForD0mumuCut  =       "(AMAXDOCA('')< %(doca)s *mm) & (AMAXCHILD(PT)>%(XmaxPT)s *MeV) & (APT> %(D0MinPT)s)& (ADAMASS('J/psi(1S)')< %(JpsiMassWin)s *MeV)"% self.getProps()
        _TightProtons = DataOnDemand(Location = "Phys/StdNoPIDsProtons/Particles")
        _JpsippForD0mumu = CombineParticles("_JpsippForD0mumu")
#        _JpsippForD0mumu.InputLocations = [ "StdLooseProtons" ]
        _JpsippForD0mumu.DecayDescriptor = "J/psi(1S) -> p+ p~-" 
        _JpsippForD0mumu.DaughtersCuts = { "p+": ProtonForD0mumuCut }
        _JpsippForD0mumu.CombinationCut = CombForD0mumuCut
        _JpsippForD0mumu.MotherCut = MomForD0mumuCut

        SelJpsippForD0mumu = Selection( "SelJpsippForD0mumu",
                                    Algorithm = _JpsippForD0mumu,
                                    RequiredSelections = [ _TightProtons ]
                                    )

        return SelJpsippForD0mumu
#        return SelectionSequence( "SeqJpsippForD0mumu",
#                                  TopSelection = SelJpsippForD0mumu,
##                                  EventPreSelector = [ SpdMultFilter ]
#                                  )
#        return StripCcbar2PpbarForD0mumu  
    
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
