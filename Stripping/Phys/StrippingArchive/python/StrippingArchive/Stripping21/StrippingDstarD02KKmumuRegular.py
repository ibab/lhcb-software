# $Id: StrippingDstarD02KKmumuRegular.py,v 1.2 2010-09-06 01:04:05 jhe Exp $
'''
Module for construction of D*+->D0pi+, D0->K+K-mu+mu- stripping Selections and StrippingLines.
Use REGULAR cuts.
Provides functions to build the D*+ and the D0 selections.
Provides class StrippingDstarD02KKmumuRegularConf, which constructs the Selections and
StrippingLines given a configuration dictionary.
Exported symbols (use python help!):
   - StrippingDstarD02KKmumuRegularConf
   - makeDstar2D0pi
   - makeD02KKmumu
'''

__author__ = 'Benoit VIAUD'
__date__ = '19/02/2011'
__version__ = '$Revision: 1.3 $'

__all__ = ('StrippingDstarD02KKmumuRegularConf',
           'makeDstar2D0pi',
           'makeD02KKmumu',
           'default_config')


from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from LHCbKernel.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from StrippingUtils.Utils import LineBuilder


class StrippingDstarD02KKmumuRegularConf(LineBuilder) :
    """
    Builder of   D*+->D0pi+, D0->K+K-mu+mu- stripping Selection and StrippingLine.
    Constructs  the  D*+->D0pi+, D0->K+K-mu+mu- Selection and StrippingLine from a configuration dictionary.
    Use REGULAR cuts.
    Usage:
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selD02KKmumu           : nominal D0->K+K-mu+mu- Selection object
    selDstar2D0pi         : nominal D*+->D0 pi+ Selection object
    DstarD02KKmumuRegularLine : StrippingLine made out of selDstar2D0pi
    lines()                  : List of lines, [line]

    """

    __configuration_keys__ = ('TrackChi2max',
                              'DDauKPTmin',
                              'DDauKPmin',
                              'DDauMuPTmin',
                              'DDauMuPmin',
                              'DDauKDelPIDmin',
                              'DDauMuDelPIDmin',
                              'DCombMassWind',
                              'DCombAMAXDOCAmax',
                              'DMothPTmin',
                              'DMothBPVDIRAmin',
                              'DMothMIPDVmax',
                              'DMothMIPCHI2DVmax',
                              'DMothBPVVDmin',
                              'DMothBPVDCHI2min',
                              'DMothBPVVDZmin',
                              'DMothVFASPFmax',
                              'DstarCombMassWind',
                              'DstarCombAMAXDOCAmax',
                              'DstarMothVFASPFmax',
                              'DstarMothPTmin',
                              'DstarMothDMmin',
                              'DstarMothDMmax',
                              'DstarMothSlpiMIPDVmax',
                              'DstarMothSlpiMIPCHI2DVmax',
                              'DstarMothSlpiPTmin',
                              'NTracksLim',
                              'LinePrescale',
                              'LinePostscale'
                              )



    def __init__(self, name, config) : # {

        LineBuilder.__init__(self, name, config)


        from Configurables import LoKi__VoidFilter as VoidFilter
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        modules = CoreFactory('CoreFactory').Modules
        for i in ['LoKiTracks.decorators']:
            if i not in modules : modules.append(i)


        d02KKmumu_name = 'D0For'+name
        dstar_name  = name

        self.inPions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")
        self.inMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
        self.inKaons = DataOnDemand(Location = "Phys/StdLooseKaons/Particles")

        self.selD02KKmumu  = makeD02KKmumu (d02KKmumu_name,
                                   inputSel = [ self.inMuons, self.inKaons ],                                            TrackChi2max = config['TrackChi2max'],
                                            DDauKPTmin = config['DDauKPTmin'],
                                            DDauKPmin = config['DDauKPmin'],
                                            DDauMuPTmin = config['DDauMuPTmin'],
                                            DDauMuPmin = config['DDauMuPmin'],
                                            DDauKDelPIDmin = config['DDauKDelPIDmin'],
                                            DDauMuDelPIDmin = config['DDauMuDelPIDmin'],
                                            DCombMassWind = config['DCombMassWind'],
                                            DCombAMAXDOCAmax = config['DCombAMAXDOCAmax'],
                                            DMothPTmin = config['DMothPTmin'],
                                            DMothBPVDIRAmin = config['DMothBPVDIRAmin'],
                                            DMothMIPDVmax = config['DMothMIPDVmax'],
                                            DMothMIPCHI2DVmax = config['DMothMIPCHI2DVmax'],
                                            DMothBPVVDmin = config['DMothBPVVDmin'],
                                            DMothBPVDCHI2min = config['DMothBPVDCHI2min'],
                                            DMothBPVVDZmin = config['DMothBPVVDZmin'],
                                            DMothVFASPFmax = config['DMothVFASPFmax'] )

        self.selDstar2D0Pi_D02KKmumu = makeDstar2D0Pi(dstar_name,
                   inputSel = [ self.inPions, self.selD02KKmumu ],
                                            DstarCombMassWind = config['DstarCombMassWind'],
                                            DstarCombAMAXDOCAmax = config['DstarCombAMAXDOCAmax'],
                                            DstarMothVFASPFmax = config['DstarMothVFASPFmax'],
                                            DstarMothPTmin = config['DstarMothPTmin'],
                                            DstarMothDMmin = config['DstarMothDMmin'],
                                            DstarMothDMmax = config['DstarMothDMmax'],
                                            DstarMothSlpiMIPDVmax = config['DstarMothSlpiMIPDVmax'],
                                            DstarMothSlpiMIPCHI2DVmax = config['DstarMothSlpiMIPCHI2DVmax'],
                                            DstarMothSlpiPTmin = config['DstarMothSlpiPTmin'])

        self.line_Dstar2D0Pi_D02KKmumu = StrippingLine(dstar_name+"Line",
                                  prescale = config['LinePrescale'],
                                  postscale = config['LinePostscale'],
                                  FILTER = {"Code":"(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s )"% config['NTracksLim'],
                                            "Preambulo":["from LoKiTracks.decorators import *"]},
                                  algos = [ self.selDstar2D0Pi_D02KKmumu ]
                                        )
        self.registerLine(self.line_Dstar2D0Pi_D02KKmumu)

def makeD02KKmumu(name,
                  inputSel,
                  TrackChi2max,
                  DDauKPTmin,
                  DDauKPmin,
                  DDauMuPTmin,
                  DDauMuPmin,
                  DDauKDelPIDmin,
                  DDauMuDelPIDmin,
                  DCombMassWind,
                  DCombAMAXDOCAmax,
                  DMothPTmin,
                  DMothBPVDIRAmin,
                  DMothMIPDVmax,
                  DMothMIPCHI2DVmax,
                  DMothBPVVDmin,
                  DMothBPVDCHI2min,
                  DMothBPVVDZmin,
                  DMothVFASPFmax,
                  decDescriptors = [ "[D0 -> K+ K- mu+ mu-]cc"]
                  ) :
    """
    Create and return a D0->K+K-mu+mu- Selection object.
    Starts from DataOnDemand 'Phys/StdLooseKaons' and 'Phys/StdLooseMuons'.
    Arguments:
    name                  : name of the Selection.
    TrackChi2max                         : Maximum track-fit chi2 for D0's daughters
    DDauKPTmin                          : Minimum PT for D0's Kaon daughters
    DDauKPmin                            : Minimum P for D0's Kaon daughters
    DDauMuPTmin                          : Minimum PT for D0's Muon daughters
    DDauMuPmin                            : Minimum P for D0's Muon daughters
    DDauKDelPIDmin                    : Minimum (PIDK-PIDpi) for D0's Kaon daughters
    DDauMuDelPIDmin                    : Minimum (PIDmu-PIDpi) for D0's Muon daughters
    DCombMassWind                    : Mass window selecting KKmumu combinations
    DCombAMAXDOCAmax,       : Max value of the highest of the 6 DOCAs of the 6 pairs one can form out of the 4 f-state particles.
    DMothPTmin                           : Minimum D0 PT
    DMothBPVDIRAmin               : Min value of the cosine of the angle between the D momentum and reconstructed line of flight
    DMothMIPDVmax                   : Max value of the D0 impact parameter wrt to the PV to which it is associated.
    DMothMIPCHI2DVmax           : Max value of the Chi2 of this IP
    DMothBPVVDmin                   : Min value of the Chi2 of the D0 flying distance.
    DMothBPVDCHI2min              : Max value of the Chi2 of this flying distance.
    DMothBPVVDZmin                 : D0 has to fly toward the detector
    DMothVFASPFmax                 : Maximum value of the D0 vertex chi2 value


    """

    _DDauKCut = "(PT> %(DDauKPTmin)s *MeV) & (P> %(DDauKPmin)s *MeV) & (PIDK-PIDpi>%(DDauKDelPIDmin)s) & (TRCHI2DOF<%(TrackChi2max)s) " % locals()
    _DDauMuCut = "(PT> %(DDauMuPTmin)s *MeV) & (P> %(DDauMuPmin)s *MeV) & (PIDmu-PIDpi>%(DDauMuDelPIDmin)s) & (TRCHI2DOF<%(TrackChi2max)s) " % locals()

    _DCombCut = "( ADAMASS('D0')<%(DCombMassWind)s*MeV) & (AMAXDOCA('')<%(DCombAMAXDOCAmax)s*mm) "%locals()

    _DMothCut = "(PT > %(DMothPTmin)s*MeV ) & (BPVDIRA > %(DMothBPVDIRAmin)s) & (MIPDV(PRIMARY) < %(DMothMIPDVmax)s*mm ) & (MIPCHI2DV(PRIMARY) < %(DMothMIPCHI2DVmax)s ) & (  BPVVDCHI2 > %(DMothBPVDCHI2min)s ) & (BPVVD > %(DMothBPVVDmin)s*mm) & (VFASPF(VCHI2/VDOF) < %(DMothVFASPFmax)s) & (  BPVVDZ > %(DMothBPVVDZmin)s )"%locals()


    _D0 = CombineParticles(
        DecayDescriptors = decDescriptors
        , DaughtersCuts = { "mu+" : _DDauMuCut, "K+" : _DDauKCut }
        , CombinationCut =  _DCombCut
        , MotherCut = _DMothCut
    )


    return Selection( name,
                      Algorithm = _D0,
                      RequiredSelections = inputSel
                    )


def makeDstar2D0Pi(name,
                   inputSel,
                   DstarCombMassWind,
                   DstarCombAMAXDOCAmax,
                   DstarMothVFASPFmax,
                   DstarMothPTmin,
                   DstarMothDMmin,
                   DstarMothDMmax,
                   DstarMothSlpiMIPDVmax,
                   DstarMothSlpiMIPCHI2DVmax,
                   DstarMothSlpiPTmin,
                   decDescriptors= [ "[D*(2010)+ -> D0 pi+]cc" ]
                   ) :
    """
    Create and return a D*+ ->D0 pi+ Selection object.
    Arguments:
    name            : name of the Selection.
     d0Sel           : D0 -> K+K- mu+ mu- Selection object.
    DstarCombMassWind                   :   Mass Window selecting the D0+Slow pion combinations
    DstarCombAMAXDOCAmax       :   Maximum value of the DOCA between the D0 and the pion
    DstarMothVFASPFmax                :   Max value of the Chi2 of the Dstar vertex
    DstarMothPTmin                          :   Minimum Dstar PT
    DstarMothDMmin                         :   Lower bound of the (M_Dstar-M_D0) window
    DstarMothDMmax                        :   Upper bound of the (M_Dstar-M_D0) window
    DstarMothSlpiMIPDVmax            :   Max value of the IP of the slow pion wrt PV
    DstarMothSlpiMIPCHI2DVmax    :   Max value of the chi2 on this IP
    DstarMothSlpiPTmin                    :   Min value of the slow pion PT

    """

    _DstarCombCut = "(ADAMASS('D*(2010)+')<%(DstarCombMassWind)s*MeV) & ( AMAXDOCA('')<%(DstarCombAMAXDOCAmax)s*mm )"%locals()
    _DstarMotherCut_VCHI2 = "( VFASPF(VCHI2/VDOF) < %(DstarMothVFASPFmax)s )"%locals()
    _DstarMotherCut_PT = "(PT > %(DstarMothPTmin)s*MeV)"% locals()
    _DstarMotherCut_DM = "(MM - CHILD(MM,1) > %(DstarMothDMmin)s*MeV) & (MM - CHILD(MM,1) < %(DstarMothDMmax)s*MeV)"% locals()
    _DstarMotherCut_SlpiIPandPT = "( CHILD(MIPDV(PRIMARY),2) < %(DstarMothSlpiMIPDVmax)s*mm) & (CHILD(MIPCHI2DV(PRIMARY),2) < %(DstarMothSlpiMIPCHI2DVmax)s ) & ( CHILD(PT,2) > %(DstarMothSlpiPTmin)s*MeV)"% locals()

    _DstarMotherCut =  "( " + _DstarMotherCut_PT + " & " + _DstarMotherCut_DM  +" & "+ _DstarMotherCut_SlpiIPandPT +" & " +  _DstarMotherCut_VCHI2 +" )"


    _Dstar = CombineParticles(
        DecayDescriptors = decDescriptors
        , CombinationCut = _DstarCombCut
        , MotherCut = _DstarMotherCut
    )

    return Selection( name,
                      Algorithm = _Dstar,
                      RequiredSelections = inputSel
                    )



default_config = {'TrackChi2max' : 5.
                  ,'DDauKPTmin'          : 200.
                  , 'DDauKPmin'     : 1000.
                  , 'DDauMuPTmin'         : 200.
                  , 'DDauMuPmin'        : 1000.
                  , 'DDauKDelPIDmin'        : -5.
                  , 'DDauMuDelPIDmin'            : -10.
                  , 'DCombMassWind'             : 60.
                  , 'DCombAMAXDOCAmax'            : 0.4
                  , 'DMothPTmin'            : 1500.
                  , 'DMothBPVDIRAmin'             : 0.999
                  , 'DMothMIPDVmax'          :  0.2
                  , 'DMothMIPCHI2DVmax'  :  36.
                  , 'DMothBPVVDmin'           : 1.0
                  , 'DMothBPVDCHI2min'           : 4.
                  , 'DMothBPVVDZmin'              : 0.
                  , 'DMothVFASPFmax'               :  12.
                  , 'DstarCombMassWind'               : 60.
                  , 'DstarCombAMAXDOCAmax'                 :  0.4
                  , 'DstarMothVFASPFmax'            :  12.
                  , 'DstarMothPTmin'             : 1500.
                  , 'DstarMothDMmin'               : 138.
                  , 'DstarMothDMmax'           : 156.
                  , 'DstarMothSlpiMIPDVmax'           : 0.400
                  , 'DstarMothSlpiMIPCHI2DVmax'   : 36.
                  , 'DstarMothSlpiPTmin'       : 250.
                  , 'NTracksLim'               : 150
                  , 'LinePrescale'   : 1.
                  , 'LinePostscale'  : 1.}
