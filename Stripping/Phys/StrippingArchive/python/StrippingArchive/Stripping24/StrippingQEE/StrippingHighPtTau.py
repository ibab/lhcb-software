# Stripping Lines for Single High PT Tau EWK/Exotica studies
# QEE Group
#
# S. Farry
#

__all__ = (
  'HighPtTauConf',
  'default_config'
)


from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsPions, StdAllLoosePions, StdLooseResolvedPi0, StdLooseMergedPi0
from GaudiKernel.SystemOfUnits import GeV
from Configurables import FilterDesktop


default_config = {
    'NAME'        : 'HighPtTau',
    'BUILDERTYPE' : 'HighPtTauConf',
    'WGs'         : [ 'QEE'],
    'STREAMS'     : [ 'EW' ],
    'CONFIG'      : { 
        'HighPtTau2Prong_Prescale'  : 1.0,
        'HighPtTau2Prong_Postscale' : 1.0,
        'HighPtTau3Prong_Prescale'  : 1.0,
        'HighPtTau3Prong_Postscale' : 1.0,
        'HighPtTau2ProngLoose_Prescale'  : 0.1,
        'HighPtTau2ProngLoose_Postscale' : 1.0,
        'HighPtTau3ProngLoose_Prescale'  : 0.1,
        'HighPtTau3ProngLoose_Postscale' : 1.0,
        '3p_pi_PT'  : 2.  * GeV,
        '2p_pi_PT'  : 5.  * GeV,
        'pi0_PT'    : 5 * GeV,
        'RHO_M_MIN' : 0. * GeV,
        'RHO_M_MAX' : 2 * GeV,
        'RHO_PT_MIN' : 0 * GeV,
        '3p_sumPT'   : 8 * GeV,
        '2p_sumPT'   : 8 * GeV,
        'DOCA_MAX'   : 0.1,
        'maxchildPT' : 5 * GeV,
        'PT_3Prong'  : 15 * GeV,
        'PT_2Prong'  : 15 * GeV,
        'FDCHI2_MIN' : 10,
        'FDT_MIN'    : 0.5,
        'VCHI2_NDOF_MAX' : 20,
        'CORRM_MIN' : 1.2*GeV,
        'CORRM_MAX' : 2.0*GeV,
        'M_MIN'     : 0 * GeV,
        'M_MAX'     : 2.0 * GeV,
        'HLT2_2Prong' : "HLT_PASS_RE('Hlt2EWSingleTauHighPt2ProngDecision')",
        'HLT2_3Prong' : "HLT_PASS_RE('Hlt2EWSingleTauHighPt3ProngDecision')",
        'ISO_MAX'   : 4 * GeV,
        'nJets_MAX'  : -1 # don't apply
    },
}

class HighPtTauConf( LineBuilder ) :

    __configuration_keys__ = default_config['CONFIG'].keys()

    def __init__( self, name, config ) :

        relinfo = [{"Type" : "RelInfoConeVariables",
             "ConeAngle" : 0.5,
             "Variables" : ['CONEPT', 'CONEANGLE','CONEMULT','CONEPTASYM'],
             "Location"  : "Iso"}
            ]

        LineBuilder.__init__( self, name, config )
        
        #make rhos
        relinfofilter2 = FilterDesktop(name+"2ProngIsoLine", Code="RELINFO('/Event/Phys/HighPtTau2ProngLine/Iso', 'CONEPT', 100000.) < %(ISO_MAX)s"%config, Inputs=['Phys/'+name+'2ProngLine/Particles'])
        relinfofilter3 = FilterDesktop(name+"3ProngIsoLine", Code="RELINFO('/Event/Phys/HighPtTau3ProngLine/Iso', 'CONEPT', 100000.) < %(ISO_MAX)s"%config, Inputs=['Phys/'+name+'3ProngLine/Particles'])
        sel_rho = RhoCombiner("Rho", StdAllLoosePions, config)
        # 2 prong signal + loose line with no iso cut
        sel_Tau2Prong      = TauMaker( name + '2Prong'     , 1, [StdAllLoosePions, StdLooseMergedPi0, StdLooseResolvedPi0], config)
        sel_Tau2ProngLoose = TauMaker( name + '2ProngLoose', 1, [StdAllLoosePions, StdLooseMergedPi0, StdLooseResolvedPi0], config)
        # 3 prong signal + loose line with no iso cut
        sel_Tau3Prong      = TauMaker( name + '3Prong'     , 0, [StdAllLoosePions, sel_rho], config)
        sel_Tau3ProngLoose = TauMaker( name + '3ProngLoose', 0, [StdAllLoosePions, sel_rho], config)

        line_Tau2Prong = StrippingLine( name + '2ProngLine',
                                        prescale  = config[ name+'2Prong_Prescale'  ],
                                        postscale = config[ name+'2Prong_Postscale' ],
                                        RequiredRawEvents = ["Muon","Calo","Velo","Tracker"],
                                        checkPV   = True,
                                        RelatedInfoTools = relinfo,
                                        RelatedInfoFilter = relinfofilter2,
                                        selection = sel_Tau2Prong,
                                        HLT2 = "%(HLT2_2Prong)s"%config
                                        )

        line_Tau2ProngLoose = StrippingLine( name + '2ProngLooseLine',
                                             prescale  = config[ name+'2ProngLoose_Prescale' ],
                                             postscale = config[ name+'2ProngLoose_Postscale' ],
                                             RequiredRawEvents = ["Muon","Calo","Velo","Tracker"],
                                             checkPV   = True,
                                             RelatedInfoTools = relinfo,
                                             selection = sel_Tau2ProngLoose,
                                             HLT2 = "%(HLT2_2Prong)s"%config
                                             )

        # apply max jet requirement if specified in config ( != -1 )
        kwargs = {}
        if config['nJets_MAX'] > -1: kwargs['FILTER'] = "( CONTAINS('Phys/StdJets/Particles') <= %(nJets_MAX)s )"%config

        line_Tau3Prong = StrippingLine( name + '3ProngLine',
                                        prescale  = config[ name+'3Prong_Prescale' ],
                                        postscale = config[ name+'3Prong_Postscale' ],
                                        RequiredRawEvents = ["Muon","Calo","Velo","Tracker"],
                                        checkPV   = True,
                                        RelatedInfoTools = relinfo,
                                        RelatedInfoFilter = relinfofilter3,
                                        selection = sel_Tau3Prong,
                                        HLT2 = "%(HLT2_3Prong)s"%config,
                                        **kwargs
                                        )

        line_Tau3ProngLoose = StrippingLine( name + '3ProngLooseLine',
                                             prescale  = config[ name+'3ProngLoose_Prescale' ],
                                             postscale = config[ name+'3ProngLoose_Postscale' ],
                                             RequiredRawEvents = ["Muon","Calo","Velo","Tracker"],
                                             checkPV   = True,
                                             RelatedInfoTools = relinfo,
                                             selection = sel_Tau3ProngLoose,
                                             HLT2 = "%(HLT2_3Prong)s"%config
                                           )

        self.registerLine( line_Tau2Prong )
        self.registerLine( line_Tau3Prong )
        self.registerLine( line_Tau2ProngLoose )
        self.registerLine( line_Tau3ProngLoose )

def RhoCombiner(name, _input, config):
    """
    Combiner to make rhos for use in tau
    """
    cc = '(AMINCHILD(PT,ISBASIC)>%(3p_pi_PT)s)'%config
    mc = '(M>%(RHO_M_MIN)s) & (M<%(RHO_M_MAX)s) & (PT>%(RHO_PT_MIN)s)'%config
    _combination = CombineParticles(
        DecayDescriptor    = 'rho(770)0 -> pi+ pi-',
        DaughtersCuts      = { 'pi+' : "ALL", 
                               'pi-' : "ALL" },
        CombinationCut     = cc,
        MotherCut          = mc
        )
    return Selection ( name,
                       Algorithm          = _combination,
                       RequiredSelections = [ _input ]
                       )

def TauMaker(name, n, _inputs, config):
    """
    Tau Maker
    n = 0 - 3prong
    n = 1 - 2prong
    """
    decays = [
        "[tau- -> rho(770)0 pi-]cc",
        "[tau- -> pi- pi0]cc"
        ]

    cc = ["(APT>%(3p_sumPT)s) & (AMAXDOCA('')<%(DOCA_MAX)s) & (AMAXCHILD(PT,ISBASIC)>%(maxchildPT)s)"%config,
        "(APT>%(2p_sumPT)s)"%config]
    mc = ['(PT>%(PT_3Prong)s) & (BPVVDCHI2 > %(FDCHI2_MIN)s) & (VFASPF(VCHI2/VDOF)<%(VCHI2_NDOF_MAX)s) & (BPVVDR > %(FDT_MIN)s) & (BPVCORRM>%(CORRM_MIN)s) & (BPVCORRM<%(CORRM_MAX)s)'%config,
        '(PT>%(PT_3Prong)s) & (M>%(M_MIN)s) & (M<%(M_MAX)s)'%config]
        
    dc = [{'pi+' : 'PT > %(3p_pi_PT)s'%config, 'pi-' : 'PT > %(3p_pi_PT)s'%config },
          {'pi+' : 'PT > %(2p_pi_PT)s'%config, 'pi-' : 'PT > %(2p_pi_PT)s'%config, 'pi0' :'PT > %(pi0_PT)s'%config }]
    
    kwargs = {}
    #set combiner to be momentum combiner for pipi0
    if n == 1: kwargs['ParticleCombiners'] = {"" : "MomentumCombiner"}
    
    _combination = CombineParticles( 
        DecayDescriptor    = decays[n],
        DaughtersCuts      = dc[n],
        CombinationCut     = cc[n],
        MotherCut          = mc[n],
        WriteP2PVRelations = False,
        **kwargs
        )

    
    return Selection ( name, Algorithm = _combination, RequiredSelections = _inputs)
