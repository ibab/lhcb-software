# $Id$
'''
Stripping Lines for the search of Xi_bc states.
The following channels are used:
 Xi_bc0 -> Xi_c0 J/psi(1S)
 Xi_bc+ -> Lambdac+ J/psi(1S)

where the daughter charmed baryon is reconstructed via:
  Lambdac+ -> p K- pi+ 
  Xic0 -> Xi- pi+
and where the Xi- is reconstructed via:
  Xi- -> Lambda pi-, Lambda -> p pi- 
'''




__author__ = ['Lucio Andelini']
__date__ = '1/Mar/2012'
__version__ = '$Revision: 0 $'


__all__ = ( 'XibcBuilder'              ## LineBuilder class specialization
            , 'default_name'           ## Default name for XibcBuilder object
            , 'default_config'         ## Default config for XibcBuilder object
            ## Common cut
            , "filterJpsi"             ## Takes preconstructed Jpsi and filter them according to muon requirements
            ## Charged Xibc
            , "makeLambdac"            ## Lambdac filter
            , "makeXibc"               ## Xibc+ CombineParticles Selection
            ## Neutral Xibc
            ,	"makeLambda0"            ## Lambda0 CombineParticles
            ,	"makeXi"                 ## Ximinus CombineParticles Selection (has to be called for Up and Long)
            ,	"makeXic"                ## Xic0 CombineParticles
            , "makeXibc0"              ## Xibc0 CombineParticles Selection
            ## Inclusive Line
            , "makeX"                  ## CombineParticle Selection for the High Mass Baryon State (assigned ID=Xibc+)
)


from Gaudi.Configuration import *

from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, AutomaticData, MergedSelection 
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from CommonParticles.StdLooseLambda import StdLooseLambdaLL, StdLooseLambdaDD
from GaudiKernel.SystemOfUnits import MeV, GeV, mm
from Configurables import TisTosParticleTagger


default_name = "Xibc"

default_config = {
    'NAME'                                  : 'Xibc',
    'WGs'                                   : ['BandQ'],
    'BUILDERTYPE'                           : 'XibcBuilder',
    'STREAMS'                               : {'Dimuon' : ['StrippingXibcXibc2LcJpsi',
                                                           'StrippingXibcX2JpsiProton'
                                                           ]
                                               },
    'CONFIG'                                : {
    'Pions4Lambdac_MINIPCHI2'             : 0.0
    , 'Pion4Lambdac_TRPCHI2'              : 0#.015
    , 'Pion4Lambdac_ProbNNpi'             : 0.2
    , 'Pion4Lambdac_PT'                   : '250*MeV'
    , 'Protons4Lambdac_TRPCHI2'           : 0#.05
    , 'Protons4Lambdac_minP'              : '0*GeV'
    , 'Kaon4Lambdac_TRPCHI2'              : 0#.05
    , 'Protons4Lambdac_ProbNNp'           : 0.05
    , 'Protons4Lambdac_PT'                : '450*MeV'
    , 'Kaon4Lambdac_PT'                   : '450*MeV'
    , 'Kaon4Lambdac_ProbNNk'              : 0.02
    , 'Kaon4Lambdac_minP'                 : '0*GeV'
    , 'Muon4Jpsi_PIDmu'                   : 0
    , 'Muon4Jpsi_TRPCHI2'                 : 0#.005
    , 'Lambdac_MassWindowLarge'           : '120*MeV'
    , 'Lambdac_MassWindowTight'           : '30*MeV'		
    , 'Lambdac_MinAPT'                    : '1500*MeV' 
    , 'Lambdac_MaxADOCA'                  : '0.5*mm'
    , 'Lambdac_BPVDIRA'                   : 0.98
    , 'Lambdac_minTAU'                    : -0.0001 	
    , 'Lambdac_ENDVERTEXCHI2'             : 5						
    , 'Jpsi_MassWindowLarge'              : '150*MeV'
    , 'Jpsi_MassWindowTight'              : '50*MeV'
    , 'Muon4Jpsi_PT'                      : '650*MeV'
    , 'Jpsi_ENDVERTEXCHI2'                : 10
    , 'Xibc_ENDVERTEXCHI2'                : 7
    , 'Xibc_MassWindow'                   : '2*GeV'
    , 'Xibc_MINPVIP'                      : 1e3			#DISABLED FOR INCLUSIVITY
    , 'Xibc2LcJpsiPrescale'               : 1.0
    ####### #
    , 'Protons4Lambda0_ProbNNp'           : 0.02
    , 'LongProtons4Lambda0_MINIPCHI2'     : 2
    , 'Protons4Lambda0_PT'                : '600*MeV'
    , 'Pions4Lambda0_ProbNNpi'            : 0.2
    , 'Pions4Lambda0_PT'                  : '0*MeV'
    , 'Pions4Lambda0_MINIPCHI2'           : 0.
    , 'Lambda0_MassWindowLarge'           : '180*MeV'
    , 'Lambda0_MassWindowTight'           : '30*MeV'
    , 'Lambda0_APT'                       : '700*MeV'
    , 'Lambda0_ENDVERTEXCHI2'             : 10	
    , 'Lambda0_DownProtonTrackPvalue'     : 0
    , 'Lambda0_DownPionTrackPvalue'       : 0
    , 'Lambda0_minFD'                     : '1*mm/GeV'
    , 'Pions4Ximinus_PT'                  : '0*MeV'
    , 'Pions4Ximinus_ProbNNpi'            : 0.2
    , 'Pions4Ximinus_TRPCHI2'             : 0
    , 'Pions4Ximinus_MINIPCHI2'           : 0
    , 'Ximinus_MassWindowLarge'           : '120*MeV'
    , 'Ximinus_MassWindowTight'           : '40*MeV'
    , 'Ximinus_APT'                       : '800*MeV'
    , 'Ximinus_FlightDistance'            : '0.1*mm/GeV'
    , 'Ximinus_ENDVERTEXCHI2'             : 20
    , 'Xic0_MassWindowLarge'              : '600*MeV'
    , 'Xic0_MassWindowTight'              : '100*MeV'
    , 'Pions4Xic0_PT'                     : '200*MeV'
    , 'Pions4Xic0_ProbNNpi'               : 0.2
    , 'Pions4Xic0_TRPCHI2'                : 0
    , 'Xic0_APT'                          : '0*MeV'
    , 'Xic0_ENDVERTEXCHI2'                : 20
    , 'Xibc0_MassWindow'                  : '1.5*GeV'
    , 'Xibc0_ENDVERTEXCHI2'               : 20
    ####### 
    , 'HighMassBaryon_MassLowEdge'        : '4.5*GeV'
    , 'HighMassBaryon_MinAPT'             : '1*GeV'
    , 'ProtonsForHighMassBaryon_TRPCHI2'  : 0.#1
    , 'ProtonsForHighMassBaryon_PT'       : '1.5*GeV'
    , 'ProtonsForHighMassBaryon_P'        : '5*GeV'
    , 'ProtonsForHighMassBaryon_ProbNNp'  : 0.1
    , 'JpsiForHighMassBaryon_PT'          : '1.5*GeV'
    , 'JpsiForHighMassBaryon_MassWin'     : '40*MeV'
    , 'JpsiForHighMassBaryon_MuonPIDmu'   : 0
    , 'JpsiProtonForHighMassBaryonCosth'  : 1 #disabled
    #######
    , "JpsiKp_CtrlLine_Prescale"          : 1.0
    , "JpsiKp_MinTAU"                     : '0.10*ps'
    , "JpsiKp_MassMin"                    : 5200 #MeV/c2
    , "JpsiKp_MassLbThreshold"            : 5750 #MeV/c2
    , "JpsiKp_MassMax"                    : 8000 #MeV/c2
    , "JpsiKp_MaxVertexChi2"              : 10
    , "JpsiKp_Jpsi_MinPT"                 : 700 #MeV/c
    , "JpsiKp_Jpsi_MassWin"               : 40  #MeV/cc
    , "JpsiKp_mu_MaxTrackGhostProb"       : 0.4
    , "JpsiKp_mu_MinPIDmu"                : 0
    , "JpsiKp_p_MinPt"                    : 300 #MeV
    , "JpsiKp_p_MinProbNNp"               : 0.05
    , "JpsiKp_p_MaxTrackGhostProb"        : 0.4
    , "JpsiKp_p_MinTrackPvalue"           : 0.1
    , "JpsiKp_p_MinP"                     : 2000 #MeV/c
    , "JpsiKp_K_MinPT"                    : 300 #MeV
    , "JpsiKp_K_MinProbNNk"               : 0.02
    , "JpsiKp_K_MaxTrackGhostProb"        : 0.4
    , "JpsiKp_K_MinTrackPvalue"           : 0.1
    , "JpsiKp_K_MinP"                     : 2000 #MeV/c
    ########
    , 'GlobalGhostProb_Max'               : 0.4
    , 'LongTrackGEC'                      : 150 
    }
    }

## These configuration parameters should not be changed outside of this file.
_my_immutable_config = {
				'PionsForLambdacLocation'         : 'Phys/StdAllNoPIDsPions/Particles'
				, 'KaonsForLambdacLocation'       : 'Phys/StdAllNoPIDsKaons/Particles'
				, 'ProtonsForLambdacLocation'     : 'Phys/StdAllNoPIDsProtons/Particles'
				, 'MuonsForJpsiLocation'          : 'Phys/StdAllNoPIDsMuons/Particles'
				, 'LongProtonsForLambda'          : 'Phys/StdLooseProtons/Particles'
				, 'LongPionsForLambda'            : 'Phys/StdLoosePions/Particles'
				, 'DownProtonsForLambda'          : 'Phys/StdNoPIDsDownProtons/Particles'
				, 'DownPionsForLambda'            : 'Phys/StdNoPIDsDownPions/Particles'
				, 'UpPionsForLambda'              : 'Phys/StdNoPIDsUpPions/Particles'
				, 'LongPionsForXi'                : 'Phys/StdLoosePions/Particles'
				, 'DownPionsForXi'                : 'Phys/StdNoPIDsDownPions/Particles'
				, 'JpsiForExclusiveLinesLocation' : 'Phys/StdLooseJpsi2MuMu/Particles'
				, 'PionsForXic'                   : 'Phys/StdAllNoPIDsPions/Particles'
        , 'Protons4JpsiKp'                : 'Phys/StdAllNoPIDsProtons/Particles'
        , 'Kaons4JpsiKp'                  : 'Phys/StdAllNoPIDsKaons/Particles'
				, 'Protons4HighMassBaryon'        : 'Phys/StdAllNoPIDsProtons/Particles'
				, 'Jpsi4HighMassBaryon'           : 'Phys/StdLooseJpsi2MuMu/Particles'
        } 


class XibcBuilder(LineBuilder) :
    """
    Search for Xibc
    """

    __configuration_keys__ = ( 
				'Pions4Lambdac_MINIPCHI2'					
				, 'Pion4Lambdac_TRPCHI2'					
				, 'Protons4Lambdac_TRPCHI2'				
				, 'Protons4Lambdac_minP'				
				, 'Pion4Lambdac_ProbNNpi'
				, 'Pion4Lambdac_PT'
				, 'Protons4Lambdac_ProbNNp'					
				, 'Protons4Lambdac_PT'					
				, 'Kaon4Lambdac_TRPCHI2'					
				, 'Kaon4Lambdac_ProbNNk'							
				, 'Kaon4Lambdac_PT'							
				, 'Kaon4Lambdac_minP'							
				, 'Muon4Jpsi_PIDmu'								
				, 'Muon4Jpsi_TRPCHI2'	
				, 'Lambdac_MassWindowLarge'						
				, 'Lambdac_MassWindowTight'						
				, 'Lambdac_MinAPT'						
				, 'Lambdac_MaxADOCA'
				, 'Lambdac_BPVDIRA'
				, 'Lambdac_minTAU'						
				, 'Lambdac_ENDVERTEXCHI2'					
				, 'Jpsi_MassWindowLarge'								
				, 'Jpsi_MassWindowTight'								
				, 'Muon4Jpsi_PT'									
				, 'Jpsi_ENDVERTEXCHI2'						
				, 'Xibc_ENDVERTEXCHI2'						
				, 'Xibc_MassWindow'								
				, 'Xibc_MINPVIP'			
				, 'Xibc2LcJpsiPrescale'						
	####### #
				, 'Protons4Lambda0_ProbNNp'
				, 'LongProtons4Lambda0_MINIPCHI2'
				, 'Pions4Lambda0_ProbNNpi'
				, 'Protons4Lambda0_PT'  
				, 'Pions4Lambda0_PT'    
				, 'Pions4Lambda0_MINIPCHI2'      
				, 'Lambda0_MassWindowLarge'						
				, 'Lambda0_MassWindowTight'						
				, 'Lambda0_APT'						
				, 'Lambda0_ENDVERTEXCHI2'					
				, 'Lambda0_DownProtonTrackPvalue'	
				, 'Lambda0_DownPionTrackPvalue'  	
				, 'Lambda0_minFD'
				, 'Pions4Ximinus_PT'        
				, 'Pions4Ximinus_ProbNNpi'        
      	, 'Pions4Ximinus_TRPCHI2'   
      	, 'Pions4Ximinus_MINIPCHI2'  
				, 'Ximinus_MassWindowLarge'						
				, 'Ximinus_MassWindowTight'						
				, 'Ximinus_APT'						
				, 'Ximinus_FlightDistance'
				, 'Ximinus_ENDVERTEXCHI2'					
				, 'Xic0_MassWindowLarge'								
				, 'Xic0_MassWindowTight'								
				, 'Pions4Xic0_PT'                
				, 'Pions4Xic0_ProbNNpi'              
				, 'Pions4Xic0_TRPCHI2'
				, 'Xic0_APT'                     
				, 'Xic0_ENDVERTEXCHI2'						
				, 'Xibc0_MassWindow'							
				, 'Xibc0_ENDVERTEXCHI2'						
	####### #
				, 'HighMassBaryon_MassLowEdge'		
				, 'ProtonsForHighMassBaryon_TRPCHI2'
				, 'HighMassBaryon_MinAPT'		
				, 'ProtonsForHighMassBaryon_PT'		
				, 'ProtonsForHighMassBaryon_P'		
				, 'ProtonsForHighMassBaryon_ProbNNp'
				, 'JpsiForHighMassBaryon_PT'			
				, 'JpsiForHighMassBaryon_MassWin'			
				, 'JpsiForHighMassBaryon_MuonPIDmu'			
				, 'JpsiProtonForHighMassBaryonCosth'
  #######
        , "JpsiKp_CtrlLine_Prescale"         
        , "JpsiKp_MinTAU"                   
        , "JpsiKp_MassMin"                   
        , "JpsiKp_MassLbThreshold"           
        , "JpsiKp_MassMax"                   
        , "JpsiKp_MaxVertexChi2"             
        , "JpsiKp_Jpsi_MinPT"                
        , "JpsiKp_Jpsi_MassWin"              
        , "JpsiKp_mu_MaxTrackGhostProb"      
        , "JpsiKp_mu_MinPIDmu"               
        , "JpsiKp_p_MinPt"                   
        , "JpsiKp_p_MinProbNNp"                 
        , "JpsiKp_p_MaxTrackGhostProb"       
        , "JpsiKp_p_MinTrackPvalue"          
        , "JpsiKp_p_MinP"                    
        , "JpsiKp_K_MinPT"                   
        , "JpsiKp_K_MinProbNNk"                 
        , "JpsiKp_K_MaxTrackGhostProb"       
        , "JpsiKp_K_MinTrackPvalue"          
        , "JpsiKp_K_MinP"                    
  	####### #
        , 'GlobalGhostProb_Max'  
				, 'LongTrackGEC')								
 

################################################################################################
# LineBuilder definition
################################################################################################

    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)
        # Set up global event cuts.
        # Conceptually these come first, although the place where they're
        # inserted into the line is at the bottom of the code.
        _globalEventCuts = {
					'Code' :
          ("(recSummary (LHCb.RecSummary.nLongTracks, 'Rec/Track/Long') < %(LongTrackGEC)s )"
                                                                                  ) % config,
					'Preambulo' : ["from LoKiTracks.decorators import *"]
				}


        ####################################### Common Jpsi  #########################################


        self._muon4Jpsi = AutomaticData(_my_immutable_config['MuonsForJpsiLocation'])
        self._Jpsi = filterJpsi(name, config=config)


        ######################################## Xibc+ Exclusive Line ##################################

        self._pions4Lambdac = AutomaticData(_my_immutable_config ['PionsForLambdacLocation'])
        self._protons4Lambdac = AutomaticData(_my_immutable_config['ProtonsForLambdacLocation']) 
        self._kaons4Lambdac = AutomaticData(_my_immutable_config['KaonsForLambdacLocation'])


        self._Lambdac = makeLambdac(name, 
                                    [self._protons4Lambdac, self._kaons4Lambdac, self._pions4Lambdac], 
                                    config);

        self._Xibc = makeXibc(name, [self._Jpsi, self._Lambdac], config);


        #declare the line:
        self.lineXibc2LcJpsi = StrippingLine(name+'Xibc2LcJpsi',
                                      prescale = config['Xibc2LcJpsiPrescale'],
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self._Xibc)

        self.registerLine(self.lineXibc2LcJpsi)

        ##################################### Xibc0 ###################################################

        self._Lambda0 = makeLambda0(name, [] , config)
        #Note: the Jpsi requirement is added to quickly skip events without a Jpsi saving CPU time
        self._XiminusL = makeXi(name+"LongPi", 
                      [self._Lambda0, AutomaticData(_my_immutable_config['LongPionsForXi']), self._Jpsi]
                      , config )

        self._XiminusD = makeXi(name+"DownPi", 
                      [self._Lambda0, AutomaticData(_my_immutable_config['DownPionsForXi']), self._Jpsi]
                      , config )

        self._Ximinus = MergedSelection(name+"MergedXiminus", 
                                        RequiredSelections = [self._XiminusL, self._XiminusD]);

        self._Xic0 = makeXic(name, 
                            [self._Ximinus, AutomaticData(_my_immutable_config['PionsForXic'])], 
                            config);

        self._Xibc0 = makeXibc0(name, [self._Jpsi, self._Xic0], config);

        #declare the line:
        self.lineXibc2XicJpsi = StrippingLine(name+'Xibc2XicJpsi',
                                      prescale = 1,
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self._Xibc0)

        self.registerLine(self.lineXibc2XicJpsi)


        ########################################## Xibc -> J/psi K p  Line #######################

        self._Xibc2JpsiKpi = makeXibc2JpsiKp(name, 
            [ self._Jpsi, 
              AutomaticData( _my_immutable_config['Protons4JpsiKp']),
              AutomaticData( _my_immutable_config['Kaons4JpsiKp'])
            ],
            config)
      
        self.lineXibc2JpsiKp = StrippingLine((name+'Xibc2JpsiKp'),
                                               prescale = 1.0,
                                               postscale = 1.0,
                                               FILTER = _globalEventCuts,
                                               selection = self._Xibc2JpsiKpi)

        self.registerLine (self.lineXibc2JpsiKp);

        ########################################## Xibc -> J/psi K p Control Line #######################

        self._Xibc2JpsiKpiCtrl = makeXibc2JpsiKp(name+"Lb", 
            [ self._Jpsi, 
              AutomaticData( _my_immutable_config['Protons4JpsiKp']),
              AutomaticData( _my_immutable_config['Kaons4JpsiKp'])
            ],
            config, controlLine = True)
      
        self.lineXibc2JpsiKp = StrippingLine((name+'Lb2JpsiKp'),
                                               prescale = config['JpsiKp_CtrlLine_Prescale'],
                                               postscale = 1.0,
                                               FILTER = _globalEventCuts,
                                               selection = self._Xibc2JpsiKpiCtrl)

        self.registerLine (self.lineXibc2JpsiKp);


        ########################################## Inclusive X -> p J/psi Line #####################

        self._X = makeX (name, 
              [	#self._Jpsi, 
                AutomaticData (_my_immutable_config['Jpsi4HighMassBaryon']), 
                AutomaticData( _my_immutable_config['Protons4HighMassBaryon'])]
                                                        , config)

        self.lineX2JpsiProton = StrippingLine(name+'X2JpsiProton',
                                      prescale = 1.0,
                                      postscale = 1.0,
                                      FILTER = _globalEventCuts,
                                      selection = self._X)

        self.registerLine(self.lineX2JpsiProton)

        ########################################## Inclusive X -> p J/psi Line Good Pointing #####
        # Complementary line reverting the Pointing requirement for Stripping20r1p1.
        # Previous Stripping version was tuned on decays towards heavy daughters
        # partially reconstructed, so that the requirement of bad pointing did make sense
        # however, it forbids studies on Xibc -> J/psi p K -like decays.
        #
        #  2014-06-04: Line "JpsiProtonGoodPointing" merged to JpsiProton 
        #              for Stripping21 (Lucio.Anderlini@fi.infn.it)
        # 

#####################
# Functions
#####################

def filterJpsi (localname, 
                _InputLocation=_my_immutable_config['JpsiForExclusiveLinesLocation'], 
                config=default_config['CONFIG']):
  _filterAlgorithm = FilterDesktop(localname+"JpsiMuonFilter");
  _myCutOnMuons = ("(PIDmu > %(Muon4Jpsi_PIDmu)s) &"+
                  " (TRPCHI2 > %(Muon4Jpsi_TRPCHI2)s) &"+
                  " (TRGHP < %(GlobalGhostProb_Max)s) &" +
                  " (PT > %(Muon4Jpsi_PT)s) ")%config
  _filterAlgorithm.Code = ("(CHILDCUT(("+_myCutOnMuons+"),1) & CHILDCUT(("+_myCutOnMuons+"),2)) &"+
                  " (VFASPF(VCHI2/VDOF)<%(Jpsi_ENDVERTEXCHI2)s) &"+
                  " (ADMASS('J/psi(1S)') < %(Jpsi_MassWindowTight)s)") % config
	

  return Selection (name = localname + 'JpsiMuonFilterSelection', 
                  Algorithm = _filterAlgorithm, 
                  RequiredSelections = [ AutomaticData(_InputLocation) ])



def makeLambdac (localname, _RequiredSelections, config=default_config['CONFIG']):
  _lambdacCP = CombineParticles(localname+"lambdacAlgorithm")
  _lambdacCP.DecayDescriptor = "[Lambda_c+ -> p+ pi+ K-]cc"
  _lambdacCP.DaughtersCuts = {
        "p+" 	: ("(TRPCHI2 > %(Protons4Lambdac_TRPCHI2)s) & " + 
                  " (PROBNNp > %(Protons4Lambdac_ProbNNp)s) & " +
                  " (TRGHP < %(GlobalGhostProb_Max)s) &" +
                  " (PT > %(Protons4Lambdac_PT)s) & " +
                  " (P > %(Protons4Lambdac_minP)s) ") % config,
        "K-"	:	("(TRPCHI2 > %(Kaon4Lambdac_TRPCHI2)s) &"+
                  "(PROBNNk > %(Kaon4Lambdac_ProbNNk)s) & "+
                  "(TRGHP < %(GlobalGhostProb_Max)s) &" +
                  "(P > %(Kaon4Lambdac_minP)s) & "+
                  "(PT > %(Kaon4Lambdac_PT)s)" ) % config,
        "pi+"	:	("(MIPCHI2DV(PRIMARY)> %(Pions4Lambdac_MINIPCHI2)s) &"+
                  " (TRPCHI2 > %(Pion4Lambdac_TRPCHI2)s) &"+
                  " (TRGHP < %(GlobalGhostProb_Max)s) &" +
                  " (PROBNNpi > %(Pion4Lambdac_ProbNNpi)s) &"+
                  "(PT > %(Pion4Lambdac_PT)s)") % config 
		}
  _lambdacCP.CombinationCut = ("(ADAMASS('Lambda_c+') < %(Lambdac_MassWindowLarge)s) & " +
                                "(APT > %(Lambdac_MinAPT)s) & " +
                                "(ADOCAMAX('') < %(Lambdac_MaxADOCA)s)"  
                                            ) % config

  _lambdacCP.MotherCut = ("(VFASPF(VCHI2/VDOF) < %(Lambdac_ENDVERTEXCHI2)s) & "+
                          "(ADMASS('Lambda_c+') < %(Lambdac_MassWindowTight)s) & "+
                          "(BPVDIRA > %(Lambdac_BPVDIRA)s) & "+
                          "(BPVLTIME()>%(Lambdac_minTAU)s)"
                          ) % config

	
  return Selection (name = localname + 'LcSelection', Algorithm=_lambdacCP, 
                          RequiredSelections = _RequiredSelections);

###

def makeXibc (localname, _RequiredSelections ,config=default_config['CONFIG']):
  _xibcLc = CombineParticles(localname + "Xibc2LcJpsiAlgorithm")
  _xibcLc.DecayDescriptor = "[Xi_bc+ -> J/psi(1S) Lambda_c+]cc"
  _xibcLc.CombinationCut = ("(ADAMASS('Xi_bc+') < %(Xibc_MassWindow)s) " 
#                          "(AMAXCHILD(TRGHP, ISBASIC) < %(GlobalGhostProb_Max)s)"
                        ) % config
  _xibcLc.MotherCut = ("(VFASPF(VCHI2/VDOF)<%(Xibc_ENDVERTEXCHI2)s)" +
                        " & (MIPDV(PRIMARY) < %(Xibc_MINPVIP)s)") % config #MINIPCUT disabled
    
  return Selection (name = localname+'XibcLcSelection', Algorithm = _xibcLc,
                      RequiredSelections = _RequiredSelections)
  ###############################################################


def makeLambda0 (localname, additionalRequiredSelections=[], config=default_config['CONFIG']):
	#Lambda combine particles
  _protonCommonCut = ('(PROBNNp > %(Protons4Lambda0_ProbNNp)s) & ' + 
                      '(PT > %(Protons4Lambda0_PT)s)'
                                                    ) % config
  _pionCommonCut = ('(PROBNNpi > %(Pions4Lambda0_ProbNNpi)s) & ' +
                    '(PT > %(Pions4Lambda0_PT)s) & ' +
                    '(MIPCHI2DV(PRIMARY) > %(Pions4Lambda0_MINIPCHI2)s)') % config

  _lambdaCommonCut = ("((VFASPF(VCHI2/VDOF)<%(Lambda0_ENDVERTEXCHI2)s) &" + 
                      "(ADMASS('Lambda0') < %(Lambda0_MassWindowTight)s) & " +
                      "(BPVVDZ/P > %(Lambda0_minFD)s))") % config

  _lambda = CombineParticles(localname+"lambda2ppi")
  _lambda.DecayDescriptors = ["[Lambda0 -> p+ pi-]cc", "[Lambda0 -> p+ pi+]cc"]
  _lambda.DaughtersCuts = { "p+" : _protonCommonCut + 
                            " & (MIPCHI2DV(PRIMARY) > %(LongProtons4Lambda0_MINIPCHI2)s)" % config,
                            "pi-" : _pionCommonCut,
                            "pi+" : _pionCommonCut
                            }
  _lambda.CombinationCut = ("(ADAMASS('Lambda0') < %(Lambda0_MassWindowLarge)s) & "+			
                            "(APT > %(Lambda0_APT)s)"		  
                                                      ) % config
  _lambda.MotherCut = _lambdaCommonCut

	#Selection
  LooseLambdaSelection = Selection(localname+"SelLooseLambda",
            Algorithm = _lambda,
            RequiredSelections =  additionalRequiredSelections + [
                                AutomaticData(_my_immutable_config['LongProtonsForLambda']),
                                AutomaticData(_my_immutable_config['LongPionsForLambda'])]);

	#Lambda combine particles WITH DOWNSTREAM TRACKS
  _lambdaDD = CombineParticles(localname+"lambda2ppidown")
  _lambdaDD.DecayDescriptors = _lambda.DecayDescriptors;
  _lambdaDD.CombinationCut = ("(ADAMASS('Lambda0') < %(Lambda0_MassWindowLarge)s) & "+
                              "(APT > %(Lambda0_APT)s)" 
                                                                        ) % config
  _lambdaDD.DaughtersCuts = {
             "p+" 		: (	_protonCommonCut +
                        "& ( TRPCHI2 > %(Lambda0_DownProtonTrackPvalue)s )") % config,
             "pi-"	:  ( _pionCommonCut + 
                        "& ( TRPCHI2 > %(Lambda0_DownPionTrackPvalue)s)" ) % config,
             "pi+"	:  ( _pionCommonCut + 
                        "& ( TRPCHI2 > %(Lambda0_DownPionTrackPvalue)s)" ) % config
            }
  _lambdaDD.MotherCut = _lambdaCommonCut

	#Selection
  LooseLambdaDDSelection = Selection(localname+"SelLooseLambdaDown",
            Algorithm = _lambdaDD,
                      RequiredSelections = additionalRequiredSelections + [
                                    AutomaticData(_my_immutable_config['DownProtonsForLambda']), 
                                    AutomaticData(_my_immutable_config['DownPionsForLambda'])]);

	#Lambda combine particles WITH UPSTREAM TRACKS
  _lambdaUL = CombineParticles(localname+"lambda2ppiup")
  _lambdaUL.DecayDescriptors = _lambda.DecayDescriptors;
  _lambdaUL.DaughtersCuts = {
                            "p+" : _protonCommonCut,
                            "pi-": _pionCommonCut,
                            "pi+": _pionCommonCut
                            }
  _lambdaUL.CombinationCut = ("(ADAMASS('Lambda0') <  %(Lambda0_MassWindowLarge)s) & "+
                              "(APT > %(Lambda0_APT)s)"
                                                       ) % config
  _lambdaUL.MotherCut = _lambdaCommonCut

	##Selection
  LooseLambdaULSelection = Selection(localname+"SelLooseLambdaUp",
                Algorithm = _lambdaUL,
                RequiredSelections = additionalRequiredSelections + [
                              AutomaticData(_my_immutable_config['LongProtonsForLambda']),
                              AutomaticData(_my_immutable_config['UpPionsForLambda'])]);

  return MergedSelection(localname+'AllLooseLambda', 
                        RequiredSelections = [LooseLambdaSelection, 
                                              LooseLambdaDDSelection, 
                                              LooseLambdaULSelection ] );
                  

def makeXi (localname, _RequiredSelections, config=default_config['CONFIG']):

  _xi = CombineParticles(localname+"xi2lambda")
  _xi.DecayDescriptor = "[Xi- -> Lambda0 pi-]cc"
  _xi.DaughtersCuts = 	{ "pi-" : (	"(PT > %(Pions4Ximinus_PT)s) & " + 
                                  "(PROBNNpi > %(Pions4Ximinus_ProbNNpi)s) & " +
                                  "(TRPCHI2 > %(Pions4Ximinus_TRPCHI2)s) & " +
                                  "(MIPCHI2DV(PRIMARY) > %(Pions4Ximinus_MINIPCHI2)s)"
                                                                                ) % config 
                          }
  _xi.CombinationCut = ("(ADAMASS('Xi-') < %(Ximinus_MassWindowLarge)s) & "+
                      "(APT > %(Ximinus_APT)s)"
                                                ) % config
  _xi.MotherCut = ("((VFASPF(VCHI2/VDOF) < %(Ximinus_ENDVERTEXCHI2)s) "
                    "& (ADMASS('Xi-') < %(Ximinus_MassWindowTight)s)" 
                    "& (BPVVDZ/P > %(Ximinus_FlightDistance)s))" 
                                                                      )% config

	#Selection
  return Selection(localname+"SelLooseXi",
            Algorithm = _xi,
            RequiredSelections = _RequiredSelections);
					


def makeXic (localname, _RequiredSelections, config=default_name):

  _xic = CombineParticles(localname+"xic2xipi")
  _xic.DecayDescriptor = "[Xi_c0 -> Xi- pi+]cc"
  _xic.CombinationCut = ("(ADAMASS('Xi_c0') < %(Xic0_MassWindowLarge)s) &  "+ 
                         "(APT > %(Xic0_APT)s)") % config
  _xic.DaughtersCuts = { "pi+": ( 
                          "(PT > %(Pions4Xic0_PT)s) & " +
                          "(TRPCHI2 > %(Pions4Xic0_TRPCHI2)s) & " +
                          "(PROBNNpi > %(Pions4Xic0_ProbNNpi)s) ") % config			
                      }

  _xic.MotherCut = ("((VFASPF(VCHI2/VDOF)< %(Xic0_ENDVERTEXCHI2)s) & "+
                          "(ADMASS('Xi_c0') < %(Xic0_MassWindowTight)s))  " 
                          )% config

  return Selection(localname+"SelLooseXic0",
            Algorithm = _xic,
            RequiredSelections = _RequiredSelections);

def makeXibc0 (localname, _RequiredSelections, config=default_name):
	
  _xibc = CombineParticles(localname + "xibc2xijpsi")
  _xibc.DecayDescriptor = "[Xi_bc0 -> Xi_c0 J/psi(1S)]cc"
  _xibc.CombinationCut = ("(ADAMASS('Xi_bc0') < %(Xibc0_MassWindow)s) & " +
                          "(AMAXCHILD(TRGHP, ISBASIC) < %(GlobalGhostProb_Max)s)"
                          ) % config
  _xibc.MotherCut = ("(VFASPF(VCHI2/VDOF)< %(Xibc0_ENDVERTEXCHI2)s)" 
#                    " & (MAXTREE(TRGHP, ISBASIC) < %(GlobalGhostProb_Max)s)"
                                                                  )% config
                  

  #Selection
  return Selection(localname + "SelLooseXibc0",
          Algorithm = _xibc,
          RequiredSelections = _RequiredSelections);


#################################################################################

def makeX (localname, _RequiredSelections, config=default_config['CONFIG']):

  _combinex = CombineParticles(localname + "HighMassState");
  _combinex.DecayDescriptor = "[Xi_bc+ -> J/psi(1S) p+]cc"
  _combinex.CombinationCut = ("(AM > %(HighMassBaryon_MassLowEdge)s ) &" +
                              " (APT > %(HighMassBaryon_MinAPT)s ) " ) % config

  _combinex.DaughtersCuts = { "p+" : ("(PT > %(ProtonsForHighMassBaryon_PT)s) &"+
                                    " (PROBNNp > %(ProtonsForHighMassBaryon_ProbNNp)s) &"+
                                    " (TRGHP < %(GlobalGhostProb_Max)s) &" +
                                    " (TRPCHI2 > %(ProtonsForHighMassBaryon_TRPCHI2)s) & "+
                                    " ( P > %(ProtonsForHighMassBaryon_P)s)") % config
                          , "J/psi(1S)" : ("(PT > %(JpsiForHighMassBaryon_PT)s) & "+
                                    "(ADMASS('J/psi(1S)') < %(JpsiForHighMassBaryon_MassWin)s) &"+
                                    "(MAXTREE(TRGHP, ISBASIC) < %(GlobalGhostProb_Max)s) & "
                                    "(MINTREE('mu+'==ABSID,PIDmu) > %(JpsiForHighMassBaryon_MuonPIDmu)s)"
                                                                            ) % config
     }
  _combinex.MotherCut = "(ALL)" #No cut
	
  return Selection(localname + "SelHighMassBaryonicStates", Algorithm =_combinex, 
                        RequiredSelections = _RequiredSelections);


###################################################################################
def makeXibc2JpsiKp (localname, _RequiredSelections, config=default_config['CONFIG'], controlLine=False):
  myXibc = CombineParticles(localname+"Xibc2JpsipK");
  myXibc.DecayDescriptor = "[Xi_bc0 -> J/psi(1S) p+ K-]cc"
  if (controlLine == True):
    myXibc.CombinationCut = ("(AM > %(JpsiKp_MassMin)s ) & " 
                             "(AM < %(JpsiKp_MassLbThreshold)s)  " )%config
  else:
    myXibc.CombinationCut = ("(AM > %(JpsiKp_MassLbThreshold)s ) & " 
                             "(AM < %(JpsiKp_MassMax)s  )  " )%config
                                                                          
  myXibc.MotherCut = ("(VFASPF(VCHI2/VDOF)< %(JpsiKp_MaxVertexChi2)s) & "
                      "(BPVLTIME()> %(JpsiKp_MinTAU)s)"
                      ) %config
  myXibc.DaughtersCuts = {"J/psi(1S)" : ("(PT > %(JpsiKp_Jpsi_MinPT)s) & "+
                                         "(ADMASS('J/psi(1S)') < %(JpsiKp_Jpsi_MassWin)s) &"+
                                         "(MAXTREE(TRGHP, ISBASIC) < %(JpsiKp_mu_MaxTrackGhostProb)s) &"+
                                         "(MINTREE('mu+'==ABSID,PIDmu) > %(JpsiKp_mu_MinPIDmu)s )" ) %config,
                          "p+" : ("(PT > %(JpsiKp_p_MinPt)s) &"+
                                  " (PROBNNp > %(JpsiKp_p_MinProbNNp)s) &"+
                                  " (TRGHP < %(JpsiKp_p_MaxTrackGhostProb)s) &" +
                                  " (TRPCHI2 > %(JpsiKp_p_MinTrackPvalue)s) & "+
                                  " (P > %(JpsiKp_p_MinP)s )" ) %config,
                          "K-" : ("(PT > %(JpsiKp_K_MinPT)s) &"+
                                  "(PROBNNk > %(JpsiKp_K_MinProbNNk)s) &"+
                                  "(TRGHP < %(JpsiKp_K_MaxTrackGhostProb)s )&" +
                                  "(TRPCHI2 > %(JpsiKp_K_MinTrackPvalue)s) & "+
                                  "(P > %(JpsiKp_K_MinP)s)") %config}


  return Selection(localname + "Xibc2JpsipKSel", Algorithm=myXibc, 
                      RequiredSelections = _RequiredSelections)
                                


###################################################################################
