'''
Stripping selections or Minimum Bias physics.
Based on those by Patrick K.
'''

__author__ = 'Yanxi ZHANG'
__date__ = '30/11/2015'
__version__ = '$Revision: 0 $'


__all__ = (
    'HeavyIonMiniBiasConf',
    'default_config'
    )

default_config =  {
    'NAME'            : 'HeavyIonMiniBias',
    'WGs'             : ['ALL'],
    'STREAMS'         : ['MiniBias'],
    'BUILDERTYPE'     : 'HeavyIonMiniBiasConf',
    'CONFIG'          : {
        "odin": ["NoBeam","Beam1","Beam2","BeamCrossing"],
        'CheckPV'    :  False,
        

        'MicroBiasPrescale'            :  1.0,
        'MicroBiasPostscale'           :  1.0, 
        "MicroBiasHlt1Filter"         : "(HLT_PASS('Hlt1MBMicroBiasVeloDecision'))", #(HLT_PASS('Hlt1MB.*Decision')) #Hlt1MBMicroBiasVelo
        "MicroBiasHlt2Filter"         : "(HLT_PASS('Hlt2PassThroughDecision'))",

        'MicroBiasLowMultPrescale'            :  0.1,
        'MicroBiasLowMultPostscale'           :  1.0, 
        "MicroBiasLowMultHlt1Filter"         : "(HLT_PASS('Hlt1MBMicroBiasLowMultVeloDecision'))", #(HLT_PASS('Hlt1MB.*Decision')) #Hlt1MBMicroBiasVelo
        "MicroBiasLowMultHlt2Filter"         : "(HLT_PASS('Hlt2PassThroughDecision'))",

        'NoBiasPrescale'            :  0.2,
        'NoBiasPostscale'           :  1.0, 
        "NoBiasHlt1Filter"         : "(HLT_PASS('Hlt1MBNoBiasLeadingCrossingDecision'))", #(HLT_PASS('Hlt1MB.*Decision')) #Hlt1MBMicroBiasVelo
        "NoBiasHlt2Filter"         : "(HLT_PASS('Hlt2PassThroughDecision'))",

      }
    }

from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from PhysSelPython.Wrappers import Selection, DataOnDemand

class HeavyIonMiniBiasConf(LineBuilder):
    
    __configuration_keys__ = default_config['CONFIG'].keys()

    
    def __init__(self, name, config ): 
        
        LineBuilder.__init__(self, name, config)
        self.name = name 
        self.config = config
        
        odin = "|".join( ["(ODIN_BXTYP == LHCb.ODIN.%s)"%(odin_type,) for odin_type in config['odin'] if odin_type in ["NoBeam","Beam1","Beam2","BeamCrossing"]])

        self.MicroBiasLine = StrippingLine( 
	      name = 'MBMicroBias',
              prescale  = self.config['MicroBiasPrescale'],
              postscale  = self.config['MicroBiasPostscale'],                                                                      
              HLT1       =self.config['MicroBiasHlt1Filter'],
              HLT2       =self.config['MicroBiasHlt2Filter'],
              checkPV   = self.config['CheckPV'],
	          RequiredRawEvents = ["Trigger", "Muon", "Calo", "Rich", "Velo", "Tracker"],
              #algos     = [ self.makeJpsi],
              ODIN      = odin
              )
        self.registerLine( self.MicroBiasLine )

        self.MicroBiasLowMultLine = StrippingLine( 
	      name = 'MBMicroBiasLowMult',
              prescale  = self.config['MicroBiasLowMultPrescale'],
              postscale  = self.config['MicroBiasLowMultPostscale'],                                                                      
              HLT1       =self.config['MicroBiasLowMultHlt1Filter'],
              HLT2       =self.config['MicroBiasLowMultHlt2Filter'],
              checkPV   = self.config['CheckPV'],
	          RequiredRawEvents = ["Trigger", "Muon", "Calo", "Rich", "Velo", "Tracker"],
              #algos     = [ self.makeJpsi],
              ODIN      = odin
              )
        self.registerLine( self.MicroBiasLowMultLine )

        self.NoBiasLine = StrippingLine( 
	      name = 'MBNoBias',
              prescale  = self.config['NoBiasPrescale'],
              postscale  = self.config['NoBiasPostscale'],                                                                      
              HLT1       =self.config['NoBiasHlt1Filter'],
              HLT2       =self.config['NoBiasHlt2Filter'],
              checkPV   = self.config['CheckPV'],
	          RequiredRawEvents = ["Trigger", "Muon", "Calo", "Rich", "Velo", "Tracker"],
              #algos     = [ self.makeJpsi],
              ODIN      = odin
              )
        self.registerLine( self.NoBiasLine )

