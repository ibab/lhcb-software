class Physics_MinBiasL0_PassThroughHlt_Jan10 :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (list of commissioning)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, G. Raven
    @date 2009-08-11
    """
    
    
    def __init__(self) :
        self.StripEndSequence = False

    def HltType(self) :
        return 'Physics_MinBiasL0_PassThroughHlt_Jan10'

    def L0TCK(self) :
        return "0x1409"

    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        return  [ ]
   
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        return [ 'Hlt1ODINPhysics','Hlt1ODINTechnical','Hlt1Tell1Error'
               , 'Hlt1L0Any'
               , 'Hlt1L0SPD' , 'Hlt1L0CALO' , 'Hlt1L0MUON' , 'Hlt1L0PU' , 'Hlt1L0SPD40' , 'Hlt1L0PU40' 
               , 'Hlt1L0Electron' , 'Hlt1L0Photon' , 'Hlt1L0Hadron' , 'Hlt1L0Muon' , 'Hlt1L0DiMuon' , 'Hlt1L0Muon,lowMult' , 'Hlt1L0DiMuon,lowMult'
               , 'Hlt1L0B1gas' , 'Hlt1L0B2gas'
               , 'Hlt1LumiNoBeam','Hlt1LumiBeamCrossing','Hlt1LumiBeam1','Hlt1LumiBeam2'
               #, 'Hlt1LumiLowNoBeam','Hlt1LumiLowBeamCrossing','Hlt1LumiLowBeam1','Hlt1LumiLowBeam2'
               , 'Hlt1MBMiniBiasNoBeam','Hlt1MBMiniBiasBeamCrossing','Hlt1MBMiniBiasBeam1','Hlt1MBMiniBiasBeam2'
               , 'Hlt1MBNoBiasNoBeam','Hlt1MBNoBiasBeamCrossing','Hlt1MBNoBiasBeam1','Hlt1MBNoBiasBeam2'
               , 'Hlt1VeloASide','Hlt1VeloCSide'
               ]

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt1Lines.Hlt1CommissioningLines  import Hlt1CommissioningLinesConf
        from Hlt1Lines.Hlt1LumiLines  import Hlt1LumiLinesConf
        from Hlt1Lines.Hlt1MBLines  import Hlt1MBLinesConf
        from Hlt1Lines.Hlt1VeloLines  import Hlt1VeloLinesConf
        from Hlt1Lines.Hlt1L0Lines  import Hlt1L0LinesConf
        return { Hlt1CommissioningLinesConf : { 'Prescale' : { 'Hlt1ODINPhysics'    : 1.
                                                             , 'Hlt1ODINTechnical'  : 0. # @OnlineEnv.AcceptRate
                                                             , 'Hlt1Tell1Error'     : 0.
                                                             } 
                                              }
               , Hlt1LumiLinesConf          : { 'EnableReco' : True  }
               , Hlt1L0LinesConf            : { 'Prescale' : { '.*' : 1 } }
               , Hlt1VeloLinesConf          : { 'Prescale' : { '.*' : 0 } }
               , Hlt1MBLinesConf            : { 'Prescale' : { 'Hlt1MBNoBiasNoBeam'        : 0.01
                                                             , 'Hlt1MBNoBiasBeam1'         : 0.05
                                                             , 'Hlt1MBNoBiasBeam2'         : 0.01
                                                             , 'Hlt1MBNoBiasBeamCrossing'  : 0.1
                                                             , 'Hlt1MBMiniBias.*'          : 0.5
                                                             } 
                                              }
               }
