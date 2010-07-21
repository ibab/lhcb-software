def __update_conf__( thresholds, conf, d ) :
        if conf not in thresholds : thresholds.update( { conf : {} } )
        for (i,j) in d.iteritems() :
            if i not in thresholds[conf] : thresholds[conf].update({ i : {} })
            thresholds[conf][i].update( j )


from  Physics_HighNu_80Vis_25L0_2Hlt1_ExpressHlt2_Jun10 import Physics_HighNu_80Vis_25L0_2Hlt1_ExpressHlt2_Jun10 
class Physics_HighNu_80Vis_25L0_2Hlt1_CoreHlt2_Jun10         ( Physics_HighNu_80Vis_25L0_2Hlt1_ExpressHlt2_Jun10 ) :
    """
    Threshold settings for Hlt1 for 2010 conditions
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven
    @date 2009-08-26
    """
    
    __all__ = ( 'HltType', 'ActiveHlt2Lines', 'Thresholds' )


    def __init__(self) :
        from HltTracking.HltReco import MinimalVelo 
        velo = MinimalVelo.outputSelection()
        self.Hlt2DefaultVoidFilter = " CONTAINS( '%s') < 350" %  (velo) 

    def HltType(self) :
        self.verifyType( Physics_HighNu_80Vis_25L0_2Hlt1_CoreHlt2_Jun10 )
        return          'Physics_HighNu_80Vis_25L0_2Hlt1_CoreHlt2_Jun10'

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        thresholds = Physics_HighNu_80Vis_25L0_2Hlt1_ExpressHlt2_Jun10.Thresholds(self)
        # Hlt2
        from Muons_Retention15 import Muons_Retention15         
        thresholds.update( Muons_Retention15().Thresholds() )

	from Exclusive_Leptonic import Exclusive_Leptonic
	thresholds.update( Exclusive_Leptonic().Thresholds() )
	
	from Electrons_Minimal_TrackFitted import Electrons_Minimal_TrackFitted
        thresholds.update( Electrons_Minimal_TrackFitted().Thresholds() )

        from Hadrons_Minimal import Hadrons_Minimal
        thresholds.update( Hadrons_Minimal().Thresholds() )
    
        from Hlt2Lines.Hlt2CommissioningLines  import Hlt2CommissioningLinesConf
        thresholds.update( { Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'    : 0.01
                                                                           , 'Hlt2Transparent'  : 1. }
                                                          }
                             }
                           )
        ##prescale topo monitoring cuts
        from Hlt2Lines.Hlt2TopologicalLines  import Hlt2TopologicalLinesConf
	from Hlt2Lines.Hlt2InclusivePhiLines import Hlt2InclusivePhiLinesConf
        thresholds.update( {
		Hlt2InclusivePhiLinesConf : {
		'TFKaonPT' : 800 },
		Hlt2TopologicalLinesConf : {
		'Prescale'   : {
		'Hlt2Topo2BodySA' : 0.1
		, 'Hlt2TopoRobTF2Body' : 0.1
		, 'Hlt2TopoRobTF3Body' : 0.1
		, 'Hlt2TopoRobTF4Body' : 0.1
		, 'Hlt2CharmTF2BodyWideMass' : 0.1
		, 'Hlt2CharmTF3BodyWideMass' : 0.1
		, 'Hlt2CharmTF4BodyWideMass' : 0.1
		, 'Hlt2CharmTF2BodyWideMassSA' : 0.1
		, 'Hlt2CharmTF3BodyWideMassSA' : 0.1
		, 'Hlt2CharmTF4BodyWideMassSA' : 0.1
		, 'Hlt2CharmOSTF2BodyWideMass' : 0.1
		, 'Hlt2CharmOSTF3BodyWideMass' : 0.1
		, 'Hlt2CharmOSTF4BodyWideMass' : 0.1
		}
            ,'Postscale'   : { 'Hlt2Topo3BodySA'    : 0.1
                               , 'Hlt2Topo4BodySA'  : 0.1
                               , 'Hlt2Charm2BodySA' : 0.1
                               , 'Hlt2Charm3BodySA' : 0.1
                               , 'Hlt2Charm4BodySA' : 0.1 }
		}
		}
                           )
        

        ### extra prescales on 
        # Hlt2B2D2hhhBachelorSignal
        # Hlt2B2D2hhBachelorSignal
        #  see http://lblogbook.cern.ch/HLT+Trigger/172 for why...

        #from Hlt2Lines.Hlt2B2DXLines import Hlt2B2DXLinesConf
        #__update_conf__(thresholds, Hlt2B2DXLinesConf, { 'Prescale' : { 'Hlt2B2D2hhhBachelorSignal' : 0.1
        #                                                              , 'Hlt2B2D2hhBachelorSignal'  : 0.1 } } )
        return thresholds

    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
      
        list = Physics_HighNu_80Vis_25L0_2Hlt1_ExpressHlt2_Jun10.ActiveHlt2Lines(self)

        from Muons_Retention15 import Muons_Retention15 
        list.extend( Muons_Retention15().ActiveHlt2Lines() )


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
        from Hadrons_Minimal import Hadrons_Minimal
        list.extend( Hadrons_Minimal().ActiveHlt2Lines() )
               
        return list
