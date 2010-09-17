def __update_conf__( thresholds, conf, d ) :
        if conf not in thresholds : thresholds.update( { conf : {} } )
        for (i,j) in d.iteritems() :
            if i not in thresholds[conf] : thresholds[conf].update({ i : {} })
            thresholds[conf][i].update( j )


from  Physics_HighNu_3000Vis_200L0_20Hlt1_ExpressHlt2_Sep10 import  Physics_HighNu_3000Vis_200L0_20Hlt1_ExpressHlt2_Sep10
class Physics_HighNu_3000Vis_200L0_20Hlt1_CoreHlt2VDM_Sep10           ( Physics_HighNu_3000Vis_200L0_20Hlt1_ExpressHlt2_Sep10 ):
    """
    Threshold settings for Hlt1 for 2010 conditions
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven, J. Albrecht
    @date 2010-06-23
    """
    
    __all__ = ( 'HltType', 'ActiveHlt2Lines', 'Thresholds' )


    def __init__(self) :
        from HltTracking.HltReco import MinimalVelo 
        velo = MinimalVelo.outputSelection()
        self.Hlt2DefaultVoidFilter = " CONTAINS( '%s') < 350" %  (velo) 

    def HltType(self) :
        self.verifyType( Physics_HighNu_3000Vis_200L0_20Hlt1_CoreHlt2VDM_Sep10 )
        return          'Physics_HighNu_3000Vis_200L0_20Hlt1_CoreHlt2VDM_Sep10'
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        thresholds = Physics_HighNu_3000Vis_200L0_20Hlt1_ExpressHlt2_Sep10.Thresholds(self)
        # Hlt2
    
        from Muons_Retention50 import Muons_Retention50         
        thresholds.update( Muons_Retention50().Thresholds() )

        from Exclusive_Leptonic import Exclusive_Leptonic
        thresholds.update( Exclusive_Leptonic().Thresholds() )

        from Electrons_Minimal_TrackFitted import Electrons_Minimal_TrackFitted
        thresholds.update( Electrons_Minimal_TrackFitted().Thresholds() )

        from Hadrons_September10 import Hadrons_Minimal
        thresholds.update( Hadrons_Minimal().Thresholds() )
    
        from Hlt2Lines.Hlt2CommissioningLines  import Hlt2CommissioningLinesConf
        thresholds.update( { Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'    : 0.01
                                                                           , 'Hlt2Transparent'  : 1. }
                                                          }
                             }
                           )

	from Hlt1Lines.Hlt1MBLines             import Hlt1MBLinesConf
        from Hlt1Lines.Hlt1L0Lines             import Hlt1L0LinesConf
        from Hlt1Lines.Hlt1LumiLines           import Hlt1LumiLinesConf
	from Hlt1Lines.Hlt1BeamGasLines        import Hlt1BeamGasLinesConf


	thresholds.update( {  Hlt1L0LinesConf :{ 'Prescale'  : { 'Hlt1L0CALO'            : 1.}
						 , 'Postscale' : {'Hlt1L0CALO'            : 'RATE(1000)'}
						 }
			      
			      , Hlt1LumiLinesConf :     {  'Prescale'  : { 'Hlt1LumiLow.*'   : 0  
									   , 'Hlt1LumiMid.*' : 0
									   						   
                                                                  }
                                                    
							   }
			      , Hlt1BeamGasLinesConf : { 'Postscale' :  { 'Hlt1BeamGasBeam1' :            'RATE(100)'
									  , 'Hlt1BeamGasBeam2' :          'RATE(100)'
									  }
							 }
			      , Hlt1MBLinesConf : { 'Prescale' : {  'Hlt1MBMicroBias.*' : 0. 
								    }
                                             }
			      })
        return thresholds


    def ActiveHlt1Lines(self) :
	    """
	    Returns a list of active lines
	    """
	    
	    # for now, assume this configuration has the same Hlt2 setup as below
	    list = Physics_HighNu_3000Vis_200L0_20Hlt1_ExpressHlt2_Sep10.ActiveHlt1Lines(self)

	    

	    list += ['Hlt1L0CALO']

	    return list

    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        # for now, assume this configuration has the same Hlt2 setup as below
        list = Physics_HighNu_3000Vis_200L0_20Hlt1_ExpressHlt2_Sep10.ActiveHlt2Lines(self)


        ##
        ## muons
        ##
        from Muons_Retention50 import Muons_Retention50 
        list.extend( Muons_Retention50().ActiveHlt2Lines() )


        from Exclusive_Leptonic import Exclusive_Leptonic
        list.extend( Exclusive_Leptonic().ActiveHlt2Lines() )

        ##
        ## electrons
        ##
        from Electrons_Minimal_TrackFitted import Electrons_Minimal_TrackFitted
        list.extend( Electrons_Minimal_TrackFitted().ActiveHlt2Lines() )


        ##
        ## hadrons
        ##
        from Hadrons_September10 import Hadrons_Minimal
        list.extend( Hadrons_Minimal().ActiveHlt2Lines() )
        
        return list
