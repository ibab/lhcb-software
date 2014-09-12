"""
Stripping options for (pre-)selecting Bd,s -> baryon antibaryon.
For now it only contains options for Bd,s -> p pbar
(ultimately it will also select Bd,s -> p Lambda_bar, Lambda Lambda_bar).

Authors: Eduardo Rodrigues
"""

########################################################################
__author__ = ['Eduardo Rodrigues', 'John Beddow', 'Christoph Hombach', 'Yanxi Zhang']
__date__ = '18/01/2014'
__version__ = '$Revision: 1.6 $'

__all__ = ('B2TwoBaryonLines',
           'makeB2PPbar',
           'makeB2PPSameSign',
           'default_config')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StandardParticles                     import StdLooseProtons
from StandardParticles import StdNoPIDsPions as Pions
from CommonParticles.StdLooseLambda import StdLooseLambdaLL, StdLooseLambdaDD
from PhysSelPython.Wrappers      import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig
from GaudiKernel.SystemOfUnits import MeV, GeV, mm, ns

default_config = { 'NAME'              : 'B2TwoBaryons',
                   'WGs'               : ['Charmless'],
                   'BUILDERTYPE'       : 'B2TwoBaryonLines',
                   'STREAMS'           : [ 'Bhadron' ],
                   'CONFIG'            :  {
                   'PrescaleB2PPbar'   : 1,
                   'MinPTB2PPbar'      : 900,
                   'MinIPChi2B2PPbar'  : 10,
                   #'TrChi2'            : 4,
                   'PIDppi'            : -1,
                   'PIDpk'             : -2,
                   'MaxPTB2PPbar'      : 2100,
                   'MaxDaughtPB2PPbar' : 300000,
                   'MaxIPChi2B2PPbar'  : 25,
                   'CombMassWindow'    : 200,
                   'VertexChi2B2PPbar' : 9,
                   'BPTB2PPbar'        : 1100,
                   'BIPChi2B2PPbar'    : 16,
                   'BDIRA'             : 0.9997,
                   'MaxGhostProb'      : 0.4,
                   'RelatedInfoTools'  : [ { "Type" : "RelInfoConeVariables",
                     	   		       "ConeAngle" : 1.4,
                                               "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                               "RecursionLevel" : 1,
					       "Location"  : 'ConeVar14'	       	 
                                             },		
                                             { "Type" : "RelInfoConeVariables",
				               "ConeAngle" : 1.2,
                                               "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
					       "RecursionLevel" : 1,                                               
					       "Location"  : 'ConeVar12'
                                             },
                                             { "Type" : "RelInfoConeVariables",
					       "ConeAngle" : 1.0,
                                               "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                               "RecursionLevel" : 1,
					       "Location"  : 'ConeVar10'
                                             },
                                             { "Type" : "RelInfoConeVariables",
                                               "ConeAngle" : 0.8,
					       "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
					       "RecursionLevel" : 1,
                                               "Location"  : 'ConeVar08'
			                     },
					     { "Type" : "RelInfoConeVariables",
                                               "ConeAngle" : 0.6,
					       "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
					       "RecursionLevel" : 1,
                                               "Location"  : 'ConeVar06'
			                     },
                                             { "Type" : "RelInfoVertexIsolation",
                                               "Location" : "VtxIsolationVar"
                                             }
                                           ],
                   'ExtraInfoTools'    : [ { "Type" : "ConeVariables"
                                             , "ConeAngle" : 0.6
                                             , "ConeNumber" : 1
                                             , "Variables" : ['angle', 'mult', 'ptasy']},
                                           { "Type" : "ConeVariables"
                                             , "ConeAngle" : 0.8
                                             , "ConeNumber" : 2
                                             , "Variables" : ['angle', 'mult', 'ptasy']},
                                           { "Type" : "ConeVariables"
                                             , "ConeAngle" : 1.0
                                             , "ConeNumber" : 3
                                             , "Variables" : ['angle', 'mult', 'ptasy']},
                                           {'Type' : 'VertexIsolation'}
                                           ] ,
                   'Bs0_APT_Min'                :   2.0*GeV
                   , 'Bs0_AM_Max'                 :   700*MeV
                   , 'Bs0_ADOCAMAX_Long_Max'        : 5*mm
                   , 'Bs0_BPVDIRA_Long_Min'         : 0.9
                   , 'Bs0_BPVIPCHI2_Long_Max'       : 25
                   , 'Bs0_VtxChi2_NDF_Long_Max'     : 16
                   , 'Bs0_BPVVDCHI2_Long_Min'       : 4
                   ,
                   'Trk_Chi2'                 : 3.0,
                   'Lambda_DD_MassWindow'     : 20.0, 
                   'Lambda_DD_VtxChi2'        : 12.0,
                   'Lambda_DD_FDChi2'         : 50.0,
                   'Lambda_DD_FD'             : 300.0,
                   'Lambda_DD_Pmin'           : 8000.0,
                   'Lambda_LL_MassWindow'     : 15.0,
                   'Lambda_LL_VtxChi2'        : 12.0,
                   'Lambda_LL_FDChi2'         : 50.0,
                   'B_Mlow'                  : 500.0,
                   'B_Mhigh'                 : 500.0,
                   'B_2bodyMlow'             : 800.0,
                   'B_2bodyMhigh'            : 800.0,
                   'B_APTmin'                : 1000.0,
                   'B_PTmin'                 : 1500.0,
                   'BDaug_MedPT_PT'          : 800.0,
                   'BDaug_MaxPT_IP'          : 0.05,
                   'BDaug_DD_maxDocaChi2'    : 5.0,
                   'BDaug_LL_maxDocaChi2'    : 5.0,
                   'BDaug_DD_PTsum'          : 4200.0,
                   'BDaug_LL_PTsum'          : 3000.0,
                   'B_DD_PTMin'             : 500.0,
                   'B_LL_PTMin'             : 500.0,
                   'B_VtxChi2'               : 12.0,
                   'B_DD_Dira'               : 0.995,
                   'B_LL_Dira'               : 0.995,
                   'B_DD_IPCHI2wrtPV'        : 8.0,
                   'B_LL_IPCHI2wrtPV'        : 8.0,
                   'B_FDwrtPV'               : 1.0,
                   'B_DD_FDChi2'             : 50.0,
                   'B_LL_FDChi2'             : 50.0,
                   'GEC_MaxTracks'           : 250,
                   'Prescale'                : 1.0,
                   'Postscale'               : 1.0,
                   'MVAResponseLL'           : 0.95,
                   'MVAResponseDD'           : 0.97,
                   }
                   }

class B2TwoBaryonLines( LineBuilder ) :
    """Class defining the Bd,s -> baryon antibaryon stripping line"""
    
    __configuration_keys__ = ( 'PrescaleB2PPbar',
                               'MinPTB2PPbar',
                               'MinIPChi2B2PPbar',
                               #'TrChi2',
                               'PIDppi',
                               'PIDpk',
                               'MaxPTB2PPbar',
                               'MaxDaughtPB2PPbar',	       
                               'MaxIPChi2B2PPbar',
                               'CombMassWindow',
                               'VertexChi2B2PPbar',
                               'BPTB2PPbar',
                               'BIPChi2B2PPbar',
                               'BDIRA',
                               'MaxGhostProb',
                               'RelatedInfoTools',
                               'ExtraInfoTools',
                               'Trk_Chi2',
                               'Bs0_APT_Min'
                               , 'Bs0_AM_Max'
                               , 'Bs0_ADOCAMAX_Long_Max'
                               , 'Bs0_BPVDIRA_Long_Min'
                               , 'Bs0_BPVIPCHI2_Long_Max'
                               , 'Bs0_VtxChi2_NDF_Long_Max'
                               , 'Bs0_BPVVDCHI2_Long_Min' ,
                               'Lambda_DD_MassWindow',
                               'Lambda_DD_VtxChi2',
                               'Lambda_DD_FDChi2',
                               'Lambda_DD_FD',
                               'Lambda_DD_Pmin',
                               'Lambda_LL_MassWindow',
                               'Lambda_LL_VtxChi2',
                               'Lambda_LL_FDChi2',
                               'B_Mlow',
                               'B_Mhigh',
                               'B_2bodyMlow',
                               'B_2bodyMhigh',
                               'B_APTmin',
                               'B_PTmin',
                               'BDaug_MedPT_PT',
                               'BDaug_MaxPT_IP',
                               'BDaug_DD_maxDocaChi2',
                               'BDaug_LL_maxDocaChi2',
                               'BDaug_DD_PTsum',
                               'BDaug_LL_PTsum',
                               'B_LL_PTMin',
                               'B_DD_PTMin',
                               'B_VtxChi2',
                               'B_DD_Dira',
                               'B_LL_Dira',
                               'B_DD_IPCHI2wrtPV',
                               'B_LL_IPCHI2wrtPV',
                               'B_FDwrtPV',
                               'B_DD_FDChi2',
                               'B_LL_FDChi2',
                               'GEC_MaxTracks',
                               'Prescale',
                               'Postscale',
                               'MVAResponseLL',
                               'MVAResponseDD',
                             )
    
    def __init__( self,name,config ) :        
        
        LineBuilder.__init__(self, name, config)
        
        B2PPbarName  = name + "B2PPbar"

        B2PPSameSignName  = name + "B2PPSameSign"

        B2Lp_dd_name    = name+'B2PLambdabarDD'
        B2Lp_ll_name    = name+'B2PLambdabarLL'

        GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
                   'Preambulo' : ["from LoKiTracks.decorators import *"]}
        self.pions   = Pions

        self.selLambda2DD = self.makeLambda2DD( 'Lambda0DDBLines', config )
        self.selLambda2LL = self.makeLambda2LL( 'Lambda0LLBLines', config )

        self.selB2LpDD = self.makeB2LpDD( B2Lp_dd_name, config )
        self.selB2LpLL = self.makeB2LpLL( B2Lp_ll_name, config )

        self.selB2LpLLMVA = self.makeB2LpLLMVA( B2Lp_ll_name+"_MVA", config )
        self.selB2LpDDMVA = self.makeB2LpDDMVA( B2Lp_dd_name+"_MVA", config )

        self.B2PPbar = self.makeB2PPbar( B2PPbarName,
                                    config['MinPTB2PPbar'],
                                    #config['TrChi2'],
                                    config['PIDppi'],
                                    config['PIDpk'],
                                    config['MinIPChi2B2PPbar'],
                                    config['MaxIPChi2B2PPbar'],
                                    config['CombMassWindow'],
                                    config['MaxPTB2PPbar'], 
                                    config['MaxDaughtPB2PPbar'],	            
                                    config['VertexChi2B2PPbar'],
                                    config['BIPChi2B2PPbar'],
                                    config['BPTB2PPbar'],
                                    config['BDIRA'],
                                    config['MaxGhostProb']
                                    )

        """self.B2PPSameSign = self.makeB2PPSameSign( B2PPSameSignName,
                                         config['MinPTB2PPbar'],
                                         #config['TrChi2'],
                                         config['PIDppi'],
                                         config['PIDpk'],
                                         config['MinIPChi2B2PPbar'],
                                         config['MaxIPChi2B2PPbar'],
                                         config['CombMassWindow'],
                                         config['MaxPTB2PPbar'], 
                                         config['MaxDaughtPB2PPbar'],	            
                                         config['VertexChi2B2PPbar'],
                                         config['BIPChi2B2PPbar'],
                                         config['BPTB2PPbar'],
                                         config['BDIRA'],
                                         config['MaxGhostProb']
                                         )
"""
        self.B2Lp_dd_line = StrippingLine(B2Lp_dd_name+"Line",
                                         prescale = config['Prescale'],
                                         postscale = config['Postscale'],
                                         selection = self.selB2LpDD,
                                         FILTER = GECCode,
                                         RelatedInfoTools = config['RelatedInfoTools'],
                                         #ExtraInfoTools = config['ExtraInfoTools'],
                                         #ExtraInfoDaughters = [self.selB2LpDD]
                                         )
        self.B2Lp_ll_line = StrippingLine(B2Lp_ll_name+"Line",
                                         prescale = config['Prescale'],
                                         postscale = config['Postscale'],
                                         selection =  self.selB2LpLL,
                                         FILTER = GECCode,
                                         RelatedInfoTools = config['RelatedInfoTools'],
                                         #ExtraInfoTools = config['ExtraInfoTools'],
                                         #ExtraInfoDaughters = [self.selB2LpLL]
                                         )
        #for alg in config['RelatedInfoTools']:
        #    alg['TopSelection'] = self.selB2LpLLMVA
        self.B2Lp_ll_MVAline = StrippingLine(B2Lp_ll_name+"_MVALine",
                                          prescale = config['Prescale'],
                                          postscale = config['Postscale'],
                                          selection =  self.selB2LpLLMVA,
                                          FILTER = GECCode,
                                          RelatedInfoTools = config['RelatedInfoTools'],
                                          #ExtraInfoTools = config['ExtraInfoTools'],
                                          #ExtraInfoDaughters = [self.selB2LpLLMVA]
                                          )
        self.B2Lp_dd_MVAline = StrippingLine(B2Lp_dd_name+"_MVALine",
                                             prescale = config['Prescale'],
                                             postscale = config['Postscale'],
                                             selection =  self.selB2LpDDMVA,
                                             FILTER = GECCode,
                                             RelatedInfoTools = config['RelatedInfoTools'],
                                             #ExtraInfoTools = config['ExtraInfoTools'],
                                             #ExtraInfoDaughters = [self.selB2LpLLMVA]
                                             )
        


        self.lineB2PPbar = StrippingLine( B2PPbarName+"Line",
                                          prescale = config['PrescaleB2PPbar'],
                                          selection = self.B2PPbar,
                                          RelatedInfoTools = config['RelatedInfoTools'])
         
        #self.lineB2PPSameSign = StrippingLine( B2PPSameSignName+"Line",
        #                                  prescale = config['PrescaleB2PPbar'],
        #                                  selection = self.B2PPSameSign,
        #                                  RelatedInfoTools = config['RelatedInfoTools'])
         

            
        ## Some generic cuts for Bs0.
        ## Vertex chi2 cut depends on number of daughters:
        ##      (2 dau => 1 NDF; 3 dau => 3 NDF; 4 dau => 5 NDF)
        _strCutComb_Long = "(ADAMASS(5.1*GeV)<1.1*%(Bs0_AM_Max)s)" \
                           "& (APT>%(Bs0_APT_Min)s)" \
                           "& (ADOCAMAX('')<%(Bs0_ADOCAMAX_Long_Max)s)" % config
        _strCutMoth_Long  = "(ADMASS(5.1*GeV)<%(Bs0_AM_Max)s)" \
                            "& (BPVVDCHI2 > %(Bs0_BPVVDCHI2_Long_Min)s)" \
                            "& (BPVIPCHI2()< %(Bs0_BPVIPCHI2_Long_Max)s)" \
                            "& (BPVDIRA > %(Bs0_BPVDIRA_Long_Min)s)" % config
        
        _strChi2Moth_Long = "(VFASPF(VCHI2/VDOF)<%(Bs0_VtxChi2_NDF_Long_Max)s)" % config
        
        _strCutMoth_Long = _strChi2Moth_Long + '&' + _strCutMoth_Long
        

        #make Bs0
        """
        Bs0 -> Lambda0 Lambda~0
        """
        self.combineBs04LLDD = self.makeBs0(name+'CombineBsLLDD', [ self.selLambda2LL,self.selLambda2DD] , '[B_s0 -> Lambda0 Lambda~0]cc', _strCutComb_Long, _strCutMoth_Long)
        self.lineBs04LLDD = StrippingLine(name = name+'Bs2LambdabarLambda', prescale = 1.0, postscale = 1.0, selection = self.combineBs04LLDD)

        self.registerLine(self.B2Lp_dd_line)
        self.registerLine(self.B2Lp_ll_line)
        self.registerLine(self.B2Lp_ll_MVAline)
        self.registerLine(self.B2Lp_dd_MVAline)
        self.registerLine(self.lineB2PPbar)
        self.registerLine(self.lineBs04LLDD)

    def makeBs0(self, localName, inputSelections, decay, cutComb, cutMoth) :
           _combineBs0 = CombineParticles( DecayDescriptor = decay, CombinationCut = cutComb, MotherCut = cutMoth )
           return Selection ( localName, Algorithm = _combineBs0, RequiredSelections = inputSelections)

    def makeB2PPbar( self,
                     name,
                     minPT,
                     #trChi2,
                     pidPPi, pidPK, minIPChi2, maxIPChi2,
                     combMassWindow, maxPT, maxP,
                     vertexChi2, bIPChi2, bPT, bDIRA, maxGhostProb ) :

        #_daughters_cuts = "(PT > %(minPT)s * MeV) & (TRCHI2DOF < %(trChi2)s) & ((PIDp-PIDpi) > %(pidPPi)s) & ( (PIDp-PIDK) > %(pidPK)s ) & (MIPCHI2DV(PRIMARY) > %(minIPChi2)s)" %locals()
        _daughters_cuts = "(PT > %(minPT)s * MeV) & (P < %(maxP)s * MeV) & ((PIDp-PIDpi) > %(pidPPi)s) & ( (PIDp-PIDK) > %(pidPK)s ) & (MIPCHI2DV(PRIMARY) > %(minIPChi2)s) & (TRGHP < %(maxGhostProb)s )" %locals()
        _combination_cuts = "( (ADAMASS('B0') < %(combMassWindow)s * MeV) | (ADAMASS('B_s0') < %(combMassWindow)s * MeV) ) & ( AMAXCHILD(MAXTREE('p+'==ABSID,PT)) > %(maxPT)s * MeV ) & ( AMAXCHILD(MAXTREE('p+'==ABSID,MIPCHI2DV(PRIMARY))) > %(maxIPChi2)s )" %locals()
        _mother_cuts = "(PT > %(bPT)s * MeV) & ( VFASPF(VCHI2PDOF) < %(vertexChi2)s ) & ( BPVDIRA > %(bDIRA)s ) & ( BPVIPCHI2() < %(bIPChi2)s )" %locals()

        CombineB2PPbar = CombineParticles(DecayDescriptor = 'B0 -> p+ p~-',
                                       DaughtersCuts = { "p+" : _daughters_cuts },
                                       CombinationCut = _combination_cuts,
                                       MotherCut = _mother_cuts )

        return Selection( name,
                          Algorithm = CombineB2PPbar,
                          RequiredSelections = [ StdLooseProtons ] )

    ########################################################################  
    def makeB2PPSameSign( self,
                          name,
                          minPT,
                          #trChi2,
                          pidPPi, pidPK, minIPChi2, maxIPChi2,
                          combMassWindow, maxPT, maxP,
                          vertexChi2, bIPChi2, bPT, bDIRA, maxGhostProb ) :
        
        #_daughters_cuts = "(PT > %(minPT)s * MeV) & (TRCHI2DOF < %(trChi2)s) & ((PIDp-PIDpi) > %(pidPPi)s) & ( (PIDp-PIDK) > %(pidPK)s ) & (MIPCHI2DV(PRIMARY) > %(minIPChi2)s)" %locals()
        _daughters_cuts = "(PT > %(minPT)s * MeV) & (P < %(maxP)s * MeV) & ((PIDp-PIDpi) > %(pidPPi)s) & ( (PIDp-PIDK) > %(pidPK)s ) & (MIPCHI2DV(PRIMARY) > %(minIPChi2)s) & (TRGHP < %(maxGhostProb)s )" %locals()
        _combination_cuts = "( (ADAMASS('B0') < %(combMassWindow)s * MeV) | (ADAMASS('B_s0') < %(combMassWindow)s * MeV) ) & ( AMAXCHILD(MAXTREE('p+'==ABSID,PT)) > %(maxPT)s * MeV ) & ( AMAXCHILD(MAXTREE('p+'==ABSID,MIPCHI2DV(PRIMARY))) > %(maxIPChi2)s )" %locals()
        _mother_cuts = "(PT > %(bPT)s * MeV) & ( VFASPF(VCHI2PDOF) < %(vertexChi2)s ) & ( BPVDIRA > %(bDIRA)s ) & ( BPVIPCHI2() < %(bIPChi2)s )" %locals()
        
        CombineB2PPSameSign = CombineParticles( DecayDescriptor = ['B0 -> p+ p+', 'B0 -> p~- p~-'], 
                                                DaughtersCuts = { "p+" : _daughters_cuts },
                                                CombinationCut = _combination_cuts,
                                                MotherCut = _mother_cuts )

        return Selection( name,
                          Algorithm = CombineB2PPSameSign,
                          RequiredSelections = [ StdLooseProtons ] )

    ###################################################################################

    def makeLambda2DD( self, name, config ) :
        # define all the cuts
        _massCut   = "(ADMASS('Lambda0')<%s*MeV)"      % config['Lambda_DD_MassWindow']
        _vtxCut    = "(VFASPF(VCHI2)<%s)   "           % config['Lambda_DD_VtxChi2']
        _fdChi2Cut = "(BPVVDCHI2>%s)"                  % config['Lambda_DD_FDChi2']
        _fdCut     = "(VFASPF(VMINVDDV(PRIMARY))>%s)"  % config['Lambda_DD_FD']
        _momCut    = "(P>%s*MeV)"                      % config['Lambda_DD_Pmin']
    
        _allCuts = _momCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdCut+'&'+_fdChi2Cut
    
        # get the Lambda0's to filter
        _stdLambdaDD = DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles")
    
        # make the filter
        _filterLambdaDD = FilterDesktop( Code = _allCuts )
    
        # make and store the Selection object
        return Selection( name, Algorithm = _filterLambdaDD, RequiredSelections = [_stdLambdaDD] )
    
    def makeLambda2LL( self, name, config ) :
        # define all the cuts
        _massCut    = "(ADMASS('Lambda0')<%s*MeV)" % config['Lambda_LL_MassWindow']
        _vtxCut     = "(VFASPF(VCHI2)<%s)"         % config['Lambda_LL_VtxChi2']
        _fdCut      = "(BPVVDCHI2>%s)"             % config['Lambda_LL_FDChi2']
        _trkChi2Cut1 = "(CHILDCUT((TRCHI2DOF<%s),1))" % config['Trk_Chi2']
        _trkChi2Cut2 = "(CHILDCUT((TRCHI2DOF<%s),2))" % config['Trk_Chi2']
        _allCuts = _massCut+'&'+_trkChi2Cut1+'&'+_trkChi2Cut2+'&'+_vtxCut+'&'+_fdCut
    
        # get the Lambda's to filter
        _stdLambdaLL = DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles")

        # make the filter
        _filterLambdaLL = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        return Selection( name, Algorithm = _filterLambdaLL, RequiredSelections = [_stdLambdaLL] )


    def makeB2LpDD( self, name, config ) :
        """
        Create and store a B ->Lambda~0(DD) p+  Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """
        
        _massCutLow     = "(AM>(5620-%s)*MeV)"               % config['B_2bodyMlow']
        _massCutHigh    = "(AM<(5620+%s)*MeV)"               % config['B_2bodyMhigh']
        _aptCut         = "(APT>%s*MeV)"                     % config['B_APTmin']
        _daugMedPtCut   = "(ANUM(PT>%s*MeV)>=2)"             % config['BDaug_MedPT_PT']
        _daugMaxPtIPCut = "(AVAL_MAX(MIPDV(PRIMARY),PT)>%s)" % config['BDaug_MaxPT_IP']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['BDaug_DD_maxDocaChi2']
        _daugPtSumCut   = "(APT1>%s*MeV)"                    % config['B_DD_PTMin']
    
        _combCuts = _aptCut+'&'+_daugPtSumCut+'&'+_daugMedPtCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_daugMaxPtIPCut+'&'+_maxDocaChi2Cut
    
        _ptCut      = "(PT>%s*MeV)"                    % config['B_PTmin']
        _vtxChi2Cut = "(VFASPF(VCHI2)<%s)"             % config['B_VtxChi2']
        _diraCut    = "(BPVDIRA>%s)"                   % config['B_DD_Dira']
        _ipChi2Cut  = "(MIPCHI2DV(PRIMARY)<%s)"        % config['B_DD_IPCHI2wrtPV']
        _fdCut      = "(VFASPF(VMINVDDV(PRIMARY))>%s)" % config['B_FDwrtPV']
        _fdChi2Cut  = "(BPVVDCHI2>%s)"                 % config['B_DD_FDChi2']
        
        _motherCuts = _ptCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdCut+'&'+_fdChi2Cut
    
        _B = CombineParticles()
        _B.DecayDescriptors = [ "B- -> p~- Lambda0", "B+ -> p+ Lambda~0"]
        _B.DaughtersCuts = { "p+" : "TRCHI2DOF<%s"% config['Trk_Chi2'] }
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts
    
        return Selection (name, Algorithm = _B, RequiredSelections = [self.selLambda2DD, StdLooseProtons ])

    def makeB2LpLL( self, name, config ) :
        """
        Create and store a B -> Lambda~0(LL) p+  Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """
    
        _massCutLow     = "(AM>(5279-%s)*MeV)"               % config['B_2bodyMlow']
        _massCutHigh    = "(AM<(5279+%s)*MeV)"               % config['B_2bodyMhigh']
        _aptCut         = "(APT>%s*MeV)"                     % config['B_APTmin']
        _daugMedPtCut   = "(ANUM(PT>%s*MeV)>=2)"             % config['BDaug_MedPT_PT']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['BDaug_LL_maxDocaChi2']
        _daugPtSumCut   = "(APT1>%s*MeV)"                   % config['B_LL_PTMin']
    
        _combCuts = _aptCut+'&'+_daugPtSumCut+'&'+_daugMedPtCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_maxDocaChi2Cut
        
        _ptCut      = "(PT>%s*MeV)"                    % config['B_PTmin']
        _vtxChi2Cut = "(VFASPF(VCHI2)<%s)"             % config['B_VtxChi2']
        _diraCut    = "(BPVDIRA>%s)"                   % config['B_LL_Dira']
        _ipChi2Cut  = "(MIPCHI2DV(PRIMARY)<%s)"        % config['B_LL_IPCHI2wrtPV']
        _fdCut      = "(VFASPF(VMINVDDV(PRIMARY))>%s)" % config['B_FDwrtPV']
        _fdChi2Cut  = "(BPVVDCHI2>%s)"                 % config['B_LL_FDChi2']
    
        _motherCuts = _ptCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdCut+'&'+_fdChi2Cut
    
        _B = CombineParticles()
        _B.DecayDescriptors = [ "B- -> p~- Lambda0", "B+ -> p+ Lambda~0" ]
        _B.DaughtersCuts = { "p+" : "TRCHI2DOF<%s"% config['Trk_Chi2'] }
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts
        
        return Selection (name, Algorithm = _B, RequiredSelections = [self.selLambda2LL, StdLooseProtons  ])
    
                                                                                                                        
    def makeB2LpLLMVA( self,  name, config ):
        """
        Select B -> Lambda~0 p+ event by MVA selcetion
        """
        from Configurables import CombineParticles as CP
        #Selection
        _B = CP("B2LpLLMVA")
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['BDaug_LL_maxDocaChi2']
        _daugPtSumCut   = "(APT1>%s*MeV)"                   % config['B_LL_PTMin']

        #_B.DecayDescriptors = [ "B- -> p~- ( Lambda0 -> p+ pi- )", "B+ -> p+ ( Lambda~0 -> p~- pi+ )" ]
        _B.DecayDescriptors = [ "B- -> p~- Lambda0", "B+ -> p+ Lambda~0" ] 
        _B.CombinationCut = "(ADAMASS('B-')<500*MeV)&"+_maxDocaChi2Cut+'&'+_daugPtSumCut
        _B.MotherCut = "VALUE('LoKi::Hybrid::DictValue/MVAResponse') >  %s" % config['MVAResponseLL']
        #_B.MotherCut = 'ALL'
        # get the Lambda's to filter
        _stdLambdaLL = DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles")

        # make the filter
        _filterLambdaLL = FilterDesktop( Code = "ALL" )

        #Configure tool
        from MVADictHelpers import addTMVAclassifierValue
        xmldir = "$TMVAWEIGHTSROOT/data/"
        #xmldir = "./"

        Vars =    {"log(B_LoKi_VCHI2NDOF)" : "log(VFASPF(VCHI2))",
                   "log(B_LoKi_BPVIPCHI2)"       : "log(MIPCHI2DV(PRIMARY))",
                   "B_LoKi_BPVDIRA"             : "BPVDIRA",
                   "log(B_LoKi_FDwrtPV)"           : "log(VFASPF(VMINVDDV(PRIMARY)))",
                   "log(B_LoKi_FDChi2)"      : "log(BPVVDCHI2)",
                   "log(B_LoKi_PT)"           : "log(PT)",
                   "log(pB_LoKi_PT)"          : "log(CHILD(PT,1))",
                   "log(pL_LoKi_PT)"          : "log(CHILD( CHILD(PT,1) ,2))",
                   "log(L_LoKi_P)"           : "log(CHILD(P,2))",
                   "log(L_LoKi_Mass)"            : "log(ADMASS('Lambda0'))",
                   "log(L_LoKi_VtxChi2)" : "log(CHILD(VFASPF(VCHI2),2))",
                   "log(L_LoKi_FDChi2)"      : "log(CHILD(BPVVDCHI2,2))",
                   "log(pL_LoKi_TrkChi2)"     : "log(CHILD(CHILD(TRCHI2DOF,2),2))",
                   "log(pi_LoKi_TrkChi2)"     : "log(CHILD(CHILD(TRCHI2DOF,1),2))",
                   } 
        addTMVAclassifierValue(Component = _B ,
                              XMLFile = xmldir+"B2pLambda_LL_BDT_v1r4.xml",
                              Variables = Vars,
                              ToolName = "MVAResponse",
                              )
#        return Selection (name, Algorithm = _B, RequiredSelections = [self.selLambda2LL, StdLooseProtons  ])
        return Selection (name, Algorithm = _B, RequiredSelections = [_stdLambdaLL, StdLooseProtons  ])
    def makeB2LpDDMVA( self,  name, config ):
        """
        Select B -> Lambda~0 p+ event by MVA selcetion
        """
        from Configurables import CombineParticles as CP
        #Selection
        _B = CP("B2LpDDMVA")
        _daugPtSumCut   = "(APT1>%s*MeV)"                   % config['B_DD_PTMin']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['BDaug_DD_maxDocaChi2']
        _daugMaxPtIPCut = "(AVAL_MAX(MIPDV(PRIMARY),PT)>%s)" % config['BDaug_MaxPT_IP']
        #_B.DecayDescriptors = [ "B- -> p~- ( Lambda0 -> p+ pi- )", "B+ -> p+ ( Lambda~0 -> p~- pi+ )" ]
        _B.DecayDescriptors = [ "B- -> p~- Lambda0", "B+ -> p+ Lambda~0" ]
        _B.CombinationCut = "(ADAMASS('B-')<500*MeV)&"+_maxDocaChi2Cut+'&'+_daugPtSumCut+'&'+_daugMaxPtIPCut
        _B.MotherCut = "VALUE('LoKi::Hybrid::DictValue/MVAResponse')>  %s" % config['MVAResponseDD']
        #_B.MotherCut = 'ALL'
        # get the Lambda's to filter
        _stdLambdaDD = DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles")
        
        # make the filter
        _filterLambdaDD = FilterDesktop( Code = "ALL" )
        
        #Configure tool
        from MVADictHelpers import addTMVAclassifierValue
        xmldir = "$TMVAWEIGHTSROOT/data/"
        #xmldir = "./"
        
        Vars =    {"log(B_LoKi_VCHI2NDOF)" : "log(VFASPF(VCHI2))",
                   "log(B_LoKi_BPVIPCHI2)"       : "log(MIPCHI2DV(PRIMARY))",
                   "B_LoKi_BPVDIRA"             : "BPVDIRA",
                   "log(B_LoKi_FDwrtPV)"           : "log(VFASPF(VMINVDDV(PRIMARY)))",
                   "log(B_LoKi_FDChi2)"      : "log(BPVVDCHI2)",
                   "log(B_LoKi_PT)"           : "log(PT)",
                   "log(pB_LoKi_PT)"          : "log(CHILD(PT,1))",
                   "log(pL_LoKi_PT)"          : "log(CHILD( CHILD(PT,1) ,2))",
                   "log(L_LoKi_P)"           : "log(CHILD(P,2))",
                   "log(L_LoKi_Mass)"            : "log(ADMASS('Lambda0'))",
                   "log(L_LoKi_VtxChi2)" : "log(CHILD(VFASPF(VCHI2),2))",
                   "log(L_LoKi_FDChi2)"      : "log(CHILD(BPVVDCHI2,2))",
                   "log(pL_LoKi_TrkChi2)"     : "log(CHILD(CHILD(TRCHI2DOF,2),2))",
                   "log(pi_LoKi_TrkChi2)"     : "log(CHILD(CHILD(TRCHI2DOF,1),2))",
                                      }
        
        addTMVAclassifierValue(Component = _B ,
                               XMLFile = xmldir+"B2pLambda_DD_BDT_v1r4.xml",
                               Variables = Vars,
                               ToolName = "MVAResponse",
                               )
        return Selection (name, Algorithm = _B, RequiredSelections = [_stdLambdaDD, StdLooseProtons  ])
