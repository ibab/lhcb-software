# Stripping Line for BMuNu (tight muons with high pT and IP)
# RD Group
#
# Xabier Cid Vidal
#
# Signal:  Isolated StdTightANNMuons in low mult events, pT > 5GeV/c, IPS>20, NTracks<150
__author__ = ['Xabier Cid Vidal']
__all__ = ('Bu2MuNuConf',
           'create_stdjets',           
           'relInfoJetsVars',
           'relInfoConeVars',
           'relInfoCylVars',
           'relInfoPFVars',
           'default_name',
           'default_config')

from Gaudi.Configuration import *
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import  StdTightMuons, StdTightANNMuons
from CommonParticles.Utils import *
from Configurables import FilterDesktop, CombineParticles
from JetAccessories.JetMaker_Config import JetMakerConf

default_name = 'Bu2MuNu'

default_config = {
    'NAME'  : default_name,
    'WGs'   : ['RD'],
    'BUILDERTYPE' : 'Bu2MuNuConf',
    'CONFIG': {'PrescaleSignal'  : 1.0,
               'PostscaleSignal' : 1.0,
               'PrescaleControl'  : 0.03,
               'PostscaleControl' : 1.0,
               'pTminMu'     : 5.,
               'pTmaxMu'     : 40.,
               'pmaxMu'      : 500.,
               'IPchi2Mu'    : 400.,
               'TrChi2Mu'    : 3,
               'useNN'        : True,
               'maxNTracks'  : 150,
               'pTminMuControl': 2.5,
               'IPchi2MuControl': 100.
               },
    'STREAMS': [ 'Leptonic' ]
    }



# the muons banning is line dependent (need to provide the location of the muons!)
def create_stdjets(strobj,line_location,stdjets_name_ban):
    
    myconf1 = JetMakerConf(stdjets_name_ban,
                           R = 0.7 ,
                           PtMin = 500.,
                           listOfParticlesToBan = [line_location],
                           JetIDCut = False).algorithms[0]
    
    if "StdJetsNoJetIDBan" in dir(strobj): strobj.StdJetsNoJetIDBan.append(myconf1)
    else: strobj.StdJetsNoJetIDBan= [myconf1]
    
    ## configure Data-On-Demand service                                                                     
    locations = updateDoD ( myconf1 )


relInfoJetsVars = ["JETNOMU1PX","JETNOMU1PY", "JETNOMU1PZ", "JETNOMU1PT", "JETNOMU1JETWIDTH", "JETNOMU1NNTAG", "JETNOMU1MNF", "JETNOMU2PX", "JETNOMU2PY", "JETNOMU2PZ", "JETNOMU2PT", "JETNOMU2JETWIDTH", "JETNOMU2NNTAG", "JETNOMU2MNF", "JETNOMU3PX", "JETNOMU3PY", "JETNOMU3PZ", "JETNOMU3PT", "JETNOMU3JETWIDTH", "JETNOMU3NNTAG", "JETNOMU3MNF", "JETMU1PX", "JETMU1PY", "JETMU1PZ", "JETMU1PT", "JETMU1JETWIDTH", "JETMU1NNTAG", "JETMU1MNF"]

relInfoConeVars = ["CONEANGLE","CONEMULT","CONEP","CONEPT","CONEPASYM","CONEPTASYM","CONEDELTAETA","CONEDELTAPHI"]

relInfoCylVars = ["CYLBESTD0","CYLBESTKS","CYLTRACKSNCYL","CYLMINPT","CYLMAXPT","CYLAVPT","CYLMAXPTTRIPCHI2","CYLMAXPTTRDOCACHI2","CYLMAXPTTRBMASS","CYLMINIPCHI2","CYLMAXIPCHI2","CYLAVIPCHI2","CYLMAXIPCHI2TRPT","CYLMAXIPCHI2TRDOCACHI2","CYLMAXIPCHI2TRBMASS","CYLMINDOCACHI2","CYLMAXDOCACHI2","CYLAVDOCACHI2","CYLMINDOCATRPT","CYLMINDOCATRIPCHI2","CYLMINDOCATRBMASS","CYLBESTBMASS","CYLBESTBMASSTRPT","CYLBESTBMASSTRIPCHI2","CYLBESTBMASSTRDOCACHI2"]

relInfoPFVars = ["PFCHARGEDMASS","PFALLMASS","PFCHARGEDTOTPT","PFALLTOTPT","PFCHARGEDMISSPT","PFALLMISSPT"]


class Bu2MuNuConf( LineBuilder ) :

    __configuration_keys__ = ( 'PrescaleSignal',
                               'PostscaleSignal',
                               'PrescaleControl',
                               'PostscaleControl',
                               'pTminMu',
                               'pTmaxMu',
                               'pmaxMu',
                               'IPchi2Mu',
                               'TrChi2Mu',
                               'useNN',
                               'maxNTracks',
                               'pTminMuControl',
                               'IPchi2MuControl'
                               )
    
    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name
        self._config = config
        self.useNN = config["useNN"]        
        
        #################################################################################
        # Define the initial selection for the signal muons
        _cutMu = "(MIPCHI2DV(PRIMARY)> %(IPchi2Mu)s)&(TRCHI2DOF < %(TrChi2Mu)s ) &"\
                 " (PT < %(pTmaxMu)s*GeV) & (P < %(pmaxMu)s*GeV) & (PT>%(pTminMu)s*GeV)" %config

        _cutMuControl = "(MIPCHI2DV(PRIMARY)> %(IPchi2MuControl)s) &"\
                        "(PT>%(pTminMuControl)s*GeV)" %config
        
        if self.useNN:
            self.selMuonsControl = FilterDesktop(self._myname + '_MuForBu2MuNuControl',
                                                 Preambulo = [ "from LoKiTracks.decorators import *"],
                                                 Code      = _cutMuControl,
                                                 Inputs = ["Phys/StdTightANNMuons/Particles"]
                                                 )
            self.selMuons = FilterDesktop(self._myname + '_MuForBu2MuNu',
                                          Preambulo = [ "from LoKiTracks.decorators import *"],
                                          Code      = _cutMu,
                                          Inputs = ["Phys/StdTightANNMuons/Particles"]
                                          )

        else:
            self.selMuonsControl = FilterDesktop(self._myname + '_MuForBu2MuNuControl',
                                                 Preambulo = [ "from LoKiTracks.decorators import *"],
                                                 Code      = _cutMuControl,
                                                 Inputs = ["Phys/StdTightANNMuons/Particles"]
                                                 )

            self.selMuons = FilterDesktop(self._myname + '_MuForBu2MuNu',
                                          Preambulo = [ "from LoKiTracks.decorators import *"],
                                          Code      = _cutMu,
                                          Inputs = ["Phys/StdTightMuons/Particles"]
                                          )

            
        self.selMuonsLocationControl = updateDoD(self.selMuonsControl)
        self.selMuonsLocationControl = self.selMuonsLocationControl.keys()[0]
        self.selMuonsLocation = updateDoD(self.selMuons)
        self.selMuonsLocation = self.selMuonsLocation.keys()[0]
        #################################################################################
        # filter high multiplicity events        
        self.filterTracks = " ( CONTAINS('Rec/Track/Best')< %(maxNTracks)s ) " %self._config


        #################################################################################
        # configure related infos!
        stdjets_name_noban = "StdJetsNoJetIDNoBan"
        # jet variables for control
        stdjets_name_ban_control = "StdJetsNoJetIDBanControl"
        RelatedInfoToolsControl = [{'Type' : 'RelInfoJetsVariables',
                                    'Location': 'JetVariables',
                                    'Variables': relInfoJetsVars,
                                    'UseVarsJetsWithB' : False,
                                    'LocationJetsNoMu' : "Phys/"+stdjets_name_ban_control+"/Particles",
                                    'LocationJetsNoRemove' : "Phys/"+stdjets_name_noban+"/Particles"}]
        
        # jet variables for signal
        stdjets_name_ban_signal = "StdJetsNoJetIDBanSignal"
        RelatedInfoToolsSignal = [{'Type' : 'RelInfoJetsVariables',
                                   'Location': 'JetVariables',
                                   'Variables': relInfoJetsVars,
                                   'UseVarsJetsWithB' : False,
                                   'LocationJetsNoMu' : "Phys/"+stdjets_name_ban_signal+"/Particles",
                                   'LocationJetsNoRemove' : "Phys/"+stdjets_name_noban+"/Particles"}]
        
        ## these are common for both lines!
        otherRelatedInfos = [
            # cone variables
            {'Type' : 'RelInfoConeVariables',
             'Variables':relInfoConeVars,
             'Location': 'ConeVariables1',
             'ConeAngle' : 1.0},
            {'Type' : 'RelInfoConeVariables',
             'Variables':relInfoConeVars,
             'Location': 'ConeVariables2',
             'ConeAngle' : 1.5},
            {'Type' : 'RelInfoConeVariables',
             'Variables':relInfoConeVars,
             'Location': 'ConeVariables3',
             'ConeAngle' : 2.0},

            # cylinder variables
            {'Type' : 'RelInfoCylVariables',
             'Variables':relInfoCylVars,
             'Location': "CylVariables1",
             'cylSize': 7.5},
            {'Type' : 'RelInfoCylVariables',
             'Variables':relInfoCylVars,
             'Location': "CylVariables2",
             'cylSize': 15},
            {'Type' : 'RelInfoCylVariables',
             'Variables':relInfoCylVars,
             'Location': "CylVariables3",
             'trackQuality': False,
             'samePV': False,
             'cylSize': 15},
            {'Type' : 'RelInfoCylVariables',
             'Variables':relInfoCylVars,
             'Location': "CylVariables4",
             'trackQuality': False,
             'samePV': True,
             'cylSize': 15},
            {'Type' : 'RelInfoCylVariables',
             'Variables':relInfoCylVars,
             'Location': "CylVariables5",
             'cylSize': 22.5},

            # PF variables
            {'Type' : 'RelInfoPFVariables',
             'Variables':relInfoPFVars,
             'Location': 'PFVariables'}]

        
        RelatedInfoToolsControl.extend(otherRelatedInfos)
        RelatedInfoToolsSignal.extend(otherRelatedInfos)
                               
        
        #################################################################################
        #control line (prescaled!)
        self.ControlLine = StrippingLine( self._myname + 'ControlLine',
                                          prescale  = config[ 'PrescaleControl' ],
                                          postscale = config[ 'PostscaleControl' ],
                                          selection = DataOnDemand( self.selMuonsLocationControl ),
                                          RelatedInfoTools = RelatedInfoToolsControl,
                                          RequiredRawEvents = ["Calo"],
                                          MDSTFlag = True                                         
                                          )
        
        # signal line (no prescale)
        self.SignalLine = StrippingLine( self._myname + 'SignalLine',
                                         prescale  = config[ 'PrescaleSignal'],
                                         postscale = config[ 'PostscaleSignal'],                                    
                                         FILTER = self.filterTracks,
                                         selection = DataOnDemand( self.selMuonsLocation ),
                                         RelatedInfoTools = RelatedInfoToolsSignal,
                                         RequiredRawEvents = ["Calo"],
                                         MDSTFlag = True
                                         )
        
        
        self.registerLine( self.ControlLine)
        self.registerLine( self.SignalLine)
        

        #################################################################################
        # This is for the relatedinfos based on jets
        StdJetsNoJetIDNoBan = JetMakerConf(stdjets_name_noban,
                                           R = 0.7 ,
                                           PtMin = 500.,
                                           JetIDCut = False).algorithms[0]
        
        ## configure Data-On-Demand service                                                               
        locations = updateDoD ( StdJetsNoJetIDNoBan )
        
        ## add the jets with the specific banning
        create_stdjets(self,self.ControlLine.outputLocation(),stdjets_name_ban_control)
        create_stdjets(self,self.SignalLine.outputLocation(),stdjets_name_ban_signal)

        

 
