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
            if cur[k] == v : continue
            print 'Warning: potential collision detected: %s -- %s' % (conf,k)
            print 'current: %s' % cur[k]
            print 'request: %s' % v
            if type(cur[k])==dict :
                cur[k].update( v )
            else :
                cur[k] = v
            print 'result: %s' % cur[k]

class Hlt2_Physics_September2012( object ):
    """
    Settings for 2012 running. Only the Hlt2 part.

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author J. Albrecht, V. Gligorov (cloned by Rob Lambert)
    @date 2012-04-17
    """
    
    __all__ = ( 'ActiveHlt1Lines', 'ActiveHlt2Lines', 'Thresholds', 'L0TCK' )

    def verifyType(self,ref) :
        # verify self.ActiveLines is still consistent with
        # our types self.ActiveHlt2Lines and self.ActiveHlt1Lines...
        # so we can force that classes which inherit from us
        # and overrule either ActiveHlt.Lines also overrule
        # HltType...
        if ( self.ActiveHlt1Lines() != ref.ActiveHlt1Lines(self)  or
             self.ActiveHlt2Lines() != ref.ActiveHlt2Lines(self) ) :
            raise RuntimeError( 'Must update HltType when modifying ActiveHlt.Lines()' )
        
    def L0TCK(self) :
        return '0x0044'

    def HltType(self) :
        self.verifyType( Hlt1_Physics_September2012 ) 
        return          'Hlt1_Physics_September2012'
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt2Lines.Hlt2CommissioningLines  import Hlt2CommissioningLinesConf
        from Hlt2Lines.Hlt2diphotonDiMuonLines import Hlt2diphotonDiMuonLinesConf
        from Hlt2Lines.Hlt2InclusiveDiProtonLines import Hlt2InclusiveDiProtonLinesConf
        from Hlt2Lines.Hlt2DisplVerticesLines  import Hlt2DisplVerticesLinesConf

        thresholds = { Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'  : 0.0001 
                                                                       , 'Hlt2Forward'      : 0.00001
                                                                       , 'Hlt2DebugEvent'   : 0.000001  }
                                                        , 'Postscale' : { 'Hlt2ErrorEvent'   : 'RATE(0.01)' } }
                       # micro bias lines switched off for high mu physics running              
                       , Hlt2diphotonDiMuonLinesConf : { 'Prescale' : { 'Hlt2LowMultHadron'     :  1.0 # for 0x0035, this is already done in L0
                                                                      , 'Hlt2LowMultPhoton'     : 0.01
                                                                      } } 

                       #, Hlt2InclusiveDiProtonLinesConf: { 'Prescale' : { 'Hlt2DiProton'           :       0.001
                       #                                                   , 'Hlt2DiProtonLowMult'  :       0.001
                       #                                               } } 

                       , Hlt2DisplVerticesLinesConf : {  'Prescale' : 
                                                            { 'Hlt2DisplVerticesHighMassSingle'               : 1
                                                              , 'Hlt2DisplVerticesSingle'                     : 1
                                                              , 'Hlt2DisplVerticesDouble'                     : 1
                                                              , 'Hlt2DisplVerticesHighMassSingle'             : 1 
                                                              , 'Hlt2DisplVerticesHighFDSingle'               : 1
                                                              , 'Hlt2DisplVerticesSinglePostScaled'           : 1
                                                              , 'Hlt2DisplVerticesSingleDown'                 : 1
                                                              , 'Hlt2DisplVerticesDoublePostScaled'           : 1
                                                              , 'Hlt2DisplVerticesSingleHighMassPostScaled'   : 1
                                                              , 'Hlt2DisplVerticesSingleHighFDPostScaled'     : 1
                                                              , 'Hlt2DisplVerticesSingleMVPostScaled'         : 1    }               
                                                         }

                       }

        from Express_Hlt2_draft2012 import Express_Hlt2_draft2012
        __update_conf__(thresholds, Express_Hlt2_draft2012().Thresholds() )

        from Muons_April2012 import Muons_April2012
        __update_conf__(thresholds,  Muons_April2012().Thresholds() )

        from Electrons_July2011 import Electrons_July2011
        __update_conf__(thresholds,  Electrons_July2011().Thresholds() )

        from Hadrons_September2012 import Hadrons_September2012
        __update_conf__(thresholds,  Hadrons_September2012().Thresholds() )

        from DV_draft2012 import DV_draft2012
        __update_conf__(thresholds,  DV_draft2012().Thresholds() )

        from CharmLeptonic_draft2012 import CharmLeptonic_draft2012
        __update_conf__(thresholds, CharmLeptonic_draft2012().Thresholds() )

        return thresholds
                       
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        hlt2 = ['Hlt2PassThrough','Hlt2Lumi','Hlt2DebugEvent','Hlt2Forward','Hlt2ErrorEvent','Hlt2Transparent',
                'Hlt2diPhotonDiMuon',
                'Hlt2LowMultMuon',
                'Hlt2LowMultHadron',
                'Hlt2LowMultPhoton',
                'Hlt2LowMultElectron',
                'Hlt2LowMultHadron_nofilter',
                'Hlt2LowMultElectron_nofilter',
                'Hlt2HighPtJets'
                ]

        from Express_Hlt2_draft2012 import Express_Hlt2_draft2012
        hlt2.extend( Express_Hlt2_draft2012().ActiveHlt2Lines() )

        from Muons_April2012 import Muons_April2012
        hlt2.extend( Muons_April2012().ActiveHlt2Lines() )

        from Electrons_July2011 import Electrons_July2011
        hlt2.extend( Electrons_July2011().ActiveHlt2Lines() )

        from Hadrons_September2012 import Hadrons_September2012
        hlt2.extend( Hadrons_September2012().ActiveHlt2Lines() )
       
        from DV_draft2012 import DV_draft2012 
        hlt2.extend( DV_draft2012().ActiveHlt2Lines() )

        from CharmLeptonic_draft2012 import CharmLeptonic_draft2012
        hlt2.extend( CharmLeptonic_draft2012().ActiveHlt2Lines() )

        from CharmCEP_September2012 import CharmCEP_September2012
        hlt2.extend( CharmCEP_September2012().ActiveHlt2Lines() )

        from KshortMuMuPiPi_July2012 import KshortMuMuPiPi_July2012
        hlt2.extend( KshortMuMuPiPi_July2012().ActiveHlt2Lines() )
 
        return hlt2
       
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        
        return []
