def __update_conf__( current, extra ) :
        for (conf,d) in extra.iteritems() :
            if conf not in current : 
                current[conf] = d
                continue
            cur = current[conf]
            for (k,v) in d.iteritems() :
                if k not in cur : 
                    cur[k] = v
                    continue
                print 'Warning: potential collision detected: %s -- %s' % (conf,k)
                print 'current: %s' % cur[k]
                print 'request: %s' % v
                if type(cur[k])==dict :
                    cur[k].update( v )
                else :
                    cur[k] = v
                print 'result: %s' % cur[k]


from  Physics_3000Vis_200L0_20Hlt1_ExpressHlt2_Oct10 import  Physics_3000Vis_200L0_20Hlt1_ExpressHlt2_Oct10
class Physics_3000Vis_200L0_20Hlt1_CoreHlt2_Oct10           ( Physics_3000Vis_200L0_20Hlt1_ExpressHlt2_Oct10 ):
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
        self.Hlt2DefaultVoidFilter = "CONTAINS( '%s') < 350" %  (velo) 

    def HltType(self) :
        self.verifyType( Physics_3000Vis_200L0_20Hlt1_CoreHlt2_Oct10 )
        return          'Physics_3000Vis_200L0_20Hlt1_CoreHlt2_Oct10'
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        thresholds = Physics_3000Vis_200L0_20Hlt1_ExpressHlt2_Oct10.Thresholds(self)
        # Hlt2
    
        from Muons_Retention50_Oct10 import Muons_Retention50_Oct10
        __update_conf__(thresholds,  Muons_Retention50_Oct10().Thresholds() )

        from Exclusive_Leptonic import Exclusive_Leptonic
        __update_conf__(thresholds,  Exclusive_Leptonic().Thresholds() )

        from Electrons_Minimal_TrackFitted import Electrons_Minimal_TrackFitted
        __update_conf__(thresholds,  Electrons_Minimal_TrackFitted().Thresholds() )

        from Hadrons_September10 import Hadrons_Minimal
        __update_conf__(thresholds,  Hadrons_Minimal().Thresholds() )
    
        from Hlt2Lines.Hlt2CommissioningLines  import Hlt2CommissioningLinesConf
        thresholds.update( { Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'    : 0.01
                                                                           , 'Hlt2Transparent'  : 1. }
                                                          }
                             }
                           )

        return thresholds

    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        # for now, assume this configuration has the same Hlt2 setup as below
        list = Physics_3000Vis_200L0_20Hlt1_ExpressHlt2_Oct10.ActiveHlt2Lines(self)


        ##
        ## muons
        ##
        from Muons_Retention50_Oct10 import Muons_Retention50_Oct10
        list.extend( Muons_Retention50_Oct10().ActiveHlt2Lines() )


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
