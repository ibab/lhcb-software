from  Physics_HighNu_40Vis_15L0_2Hlt1_ExpressHlt2_Jun10 import  Physics_HighNu_40Vis_15L0_2Hlt1_ExpressHlt2_Jun10
class Physics_HighNu_40Vis_15L0_2Hlt1_AllHlt2_Jun10           ( Physics_HighNu_40Vis_15L0_2Hlt1_ExpressHlt2_Jun10 ):
    """
    Threshold settings for Hlt1 for 2010 conditions
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven, J. Albrecht
    @date 2010-06-23
    """
    
    __all__ = ( 'HltType', 'ActiveHlt2Lines', 'Thresholds' )

    def HltType(self) :
        self.verifyType( Physics_HighNu_40Vis_15L0_2Hlt1_AllHlt2_Jun10 )
        return          'Physics_HighNu_40Vis_15L0_2Hlt1_AllHlt2_Jun10'
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        thresholds = Physics_HighNu_40Vis_15L0_2Hlt1_ExpressHlt2_Jun10.Thresholds(self)
        # Hlt2
        from Muons_Retention15 import Muons_Retention15         
        thresholds.update( Muons_Retention15().Thresholds() )

        from CharmLines import CharmLines
        thresholds.update( CharmLines().Thresholds() )
        
        from TopoLines import TopoLines
        thresholds.update( TopoLines().Thresholds())

        from Phi_Reduction5 import PhiCharmLepton_Reduction5
        thresholds.update(PhiCharmLepton_Reduction5().Thresholds())
      
        from Hlt2Lines.Hlt2CommissioningLines  import Hlt2CommissioningLinesConf
        thresholds.update( { Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'    : 1.
                                                                           , 'Hlt2Transparent'  : 1. }
                                                          }
                             }
                           )
        ##prescale topo monitoring cuts
        from Hlt2Lines.Hlt2TopologicalLines  import Hlt2TopologicalLinesConf
        thresholds.update( { Hlt2TopologicalLinesConf : {
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
        
        return thresholds

    

    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        list = Physics_HighNu_40Vis_15L0_2Hlt1_ExpressHlt2_Jun10.ActiveHlt2Lines(self)

        from Muons_Retention15 import Muons_Retention15 
        list.extend( Muons_Retention15().ActiveHlt2Lines() )

        ##
        ## Topo : whole list
        ##
        from TopoLines import TopoLines
        list.extend( TopoLines().ActiveHlt2Lines() )
        ##
        ## Charm : whole list
        ##
        from CharmLines import CharmLines
        list.extend( CharmLines().ActiveHlt2Lines() )

        ##
        ## phi : whole list
        ##
        from Phi_Reduction5 import PhiCharmLepton_Reduction5
        list.extend( PhiCharmLepton_Reduction5().ActiveHlt2Lines() )
        ##
        ## B 2DX
        ##
        from B2DX_320Vis import B2DX_320Vis
        list.extend( B2DX_320Vis().ActiveHlt2Lines() )
        
        ##
        ## exclusive : whole list
        ##
        from Exclusive_Nominal import Exclusive_Nominal
        list.extend( Exclusive_Nominal().ActiveHlt2Lines() )
               
        return list
