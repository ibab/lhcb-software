#Stripping Lines for Low Multiplicity Processes.
#Electroweak Group (Convenor: Tara Shears)
#Adaptation of lines (to use line builders) originally designed by Dermot Moran by Will Barter

# Accepts events that passed the relevant HLT line.


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


confdict_LowMult={
    'LowMultPrescale'    : 1.0 
    ,  'LowMultPostscale'   : 1.0
    ,  'LowMultPrescale_ps'    : 0.005
    ,  'LowMultNoFilterPrescale':0.1
    }

name = "LowMult"

class LowMultConf(LineBuilder) :

    __configuration_keys__ = ('LowMultPrescale',
                              'LowMultPostscale',
                              'LowMultPrescale_ps',
                              'LowMultNoFilterPrescale'
                              )
    
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self._myname = name


        #MUON

        ExclusiveMuonGEC = {'Code' : "(recSummaryTrack( LHCb.RecSummary.nBackTracks, TrBACKWARD)<1)",
                            'Preambulo' : ["from LoKiTracks.decorators import *"]}
       
        self.LowMultMuon_line = StrippingLine(self._myname+"MuonLine",
                                              prescale = config['LowMultPrescale'],
                                              postscale = config['LowMultPostscale'],
                                              checkPV = False,
                                              FILTER = ExclusiveMuonGEC,
                                              HLT = "HLT_PASS('Hlt2LowMultMuonDecision')"
                                              )
        
        self.registerLine(self.LowMultMuon_line)

        self.LowMultMuon_lineps = StrippingLine(self._myname+"MuonLinePS",
                                                prescale = config['LowMultPrescale_ps'],
                                                postscale = config['LowMultPostscale'],
                                                checkPV = False,
                                                HLT = "HLT_PASS('Hlt2LowMultMuonDecision')"
                                                )
        
        self.registerLine(self.LowMultMuon_lineps)

        self.LowMultPP2PPMuMu_line = StrippingLine(self._myname+"PP2PPMuMuLine",
                                                   prescale = config['LowMultPrescale'],
                                                   postscale = config['LowMultPostscale'],
                                                   checkPV = False,
                                                   FILTER = ExclusiveMuonGEC,
                                                   HLT = "HLT_PASS('Hlt2diPhotonDiMuonDecision')"
                                                   )
        
        self.registerLine(self.LowMultPP2PPMuMu_line)

        self.LowMultPP2PPMuMu_lineps = StrippingLine(self._myname+"PP2PPMuMuLinePS",
                                                     prescale = config['LowMultPrescale_ps'],
                                                     postscale = config['LowMultPostscale'],
                                                     checkPV = False,
                                                     HLT = "HLT_PASS('Hlt2diPhotonDiMuonDecision')"
                                                     )
        
        self.registerLine(self.LowMultPP2PPMuMu_lineps)

        #ELECTRON

        ExclusiveElectronGEC = {'Code' : "(recSummaryTrack( LHCb.RecSummary.nBackTracks, TrBACKWARD)<1) &   ( CONTAINS ( 'Rec/Track/Best'  ) <  6 ) ",
                                'Preambulo' : ["from LoKiTracks.decorators import *"]}                               
         
        self.LowMultElectron_line = StrippingLine(self._myname+"ElectronLine",
                                                  prescale = config['LowMultPrescale'],
                                                  postscale = config['LowMultPostscale'],
                                                  checkPV = False,
                                                  FILTER = ExclusiveElectronGEC,
                                                  HLT = "HLT_PASS('Hlt2LowMultElectronDecision')"
                                                  )
        
        self.registerLine(self.LowMultElectron_line)

        self.LowMultElectron_lineps = StrippingLine(self._myname+"ElectronLinePS",
                                                    prescale = config['LowMultPrescale_ps'],
                                                    postscale = config['LowMultPostscale'],
                                                    checkPV = False,
                                                    HLT = "HLT_PASS('Hlt2LowMultElectronDecision')"
                                                    )
        
        self.registerLine(self.LowMultElectron_lineps)

        self.LowMultElectron_nofilter_line = StrippingLine(self._myname+"ElectronLineNoFilter",
                                                    prescale = config['LowMultNoFilterPrescale'],
                                                    postscale = config['LowMultPostscale'],
                                                    checkPV = False,
                                                    HLT = "HLT_PASS('Hlt2LowMultElectron_nofilterDecision')"
                                                    )
        
        self.registerLine(self.LowMultElectron_nofilter_line)

        
        #HADRON

        ExclusiveHadronGEC = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) > 1) & (recSummaryTrack( LHCb.RecSummary.nBackTracks, TrBACKWARD)<1) &   ( CONTAINS ( 'Rec/Track/Best'  ) <  6 ) ",
                              'Preambulo' : ["from LoKiTracks.decorators import *"]}
        
        self.LowMultHadron_line = StrippingLine(self._myname+"HadronLine",
                                                prescale = config['LowMultPrescale'],
                                                postscale = config['LowMultPostscale'],
                                                checkPV = False,
                                                FILTER = ExclusiveHadronGEC,
                                                HLT = "HLT_PASS('Hlt2LowMultHadronDecision')"
                                                )
        
        self.registerLine(self.LowMultHadron_line)


        self.LowMultHadron_lineps = StrippingLine(self._myname+"HadronLinePS",
                                                  prescale = config['LowMultPrescale_ps'],
                                                  postscale = config['LowMultPostscale'],
                                                  checkPV = False,
                                                  HLT = "HLT_PASS('Hlt2LowMultHadronDecision')"
                                                  )
        
        self.registerLine(self.LowMultHadron_lineps)

        self.LowMultHadron_nofilter_line = StrippingLine(self._myname+"HadronLineNoFilter",
                                                  prescale = config['LowMultNoFilterPrescale'],
                                                  postscale = config['LowMultPostscale'],
                                                  checkPV = False,
                                                  HLT = "HLT_PASS('Hlt2LowMultHadron_nofilterDecision')"
                                                  )
        
        self.registerLine(self.LowMultHadron_nofilter_line)
        

        #PHOTON

        self.LowMultPhoton_line = StrippingLine(self._myname+"PhotonLine",
                                          prescale = config['LowMultPrescale'],
                                          postscale = config['LowMultPostscale'],
                                          checkPV = False,
                                          HLT = "HLT_PASS('Hlt2LowMultPhotonDecision')"
                                          )
        
        self.registerLine(self.LowMultPhoton_line)



        
      
