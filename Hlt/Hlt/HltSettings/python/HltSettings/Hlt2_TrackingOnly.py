from GaudiKernel.SystemOfUnits import GeV, MeV, mm

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

class Hlt2_TrackingOnly( object ):
    """
    Settings for only running the tracking in the HLT

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author S. Neubert, J. Albrecht, V. Gligorov
    @date 2013-03-27
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
        return '0x00a8'

    def HltType(self) :
        self.verifyType( Hlt2_TrackingOnly )
        return          'Hlt2_TrackingOnly'

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt2Lines.CaloTest.Lines       import CaloTestLines
        from Hlt1Lines.Hlt1MVALines         import Hlt1MVALinesConf
        from Hlt2Lines.Commissioning.Lines  import CommissioningLines
        return {CommissioningLines : {'Prescale' : { 'Hlt2PassThrough'  : 0.0001
                                                   , 'Hlt2Forward'      : 0.00001
                                                   , 'Hlt2DebugEvent'   : 0.000001  }
                                                   , 'Postscale' : { 'Hlt2ErrorEvent'   : 'RATE(0.01)' }
                                                   },
                Hlt1MVALinesConf :     {'DoTiming'                     : False,
                                        'TrackMVA' :    {'TrChi2'      :     2.5,
                                                         'MinPT'       :  1000.  * MeV,
                                                         'MaxPT'       : 25000.  * MeV,
                                                         'MinIPChi2'   :     7.4,
                                                         'Param1'      :     1.0,
                                                         'Param2'      :     1.0,
                                                         'Param3'      :     1.1,
                                                         'GEC'         : 'Loose'},
                                        'TwoTrackMVA' : {'P'           :  5000. * MeV,
                                                         'PT'          :   500. * MeV,
                                                         'TrChi2'      :     2.5,
                                                         'IPChi2'      :     4.,
                                                         'MinMCOR'     :  1000. * MeV,
                                                         'MaxMCOR'     :   1e9  * MeV,
                                                         'MinETA'      :     2.,
                                                         'MaxETA'      :     5.,
                                                         'MinDirA'     :     0.,
                                                         'V0PT'        :  2000. * MeV,
                                                         'VxChi2'      :    10.,
                                                         'Threshold'   :     0.95,
                                                         'MvaVars'     : {'chi2'   : 'VFASPF(VCHI2)',
                                                                          'fdchi2' : 'BPVVDCHI2',
                                                                          'sumpt'  : 'SUMTREE(PT, ISBASIC, 0.0)',
                                                                          'nlt16'  : 'NINTREE(ISBASIC & (BPVIPCHI2() < 16))'},
                                                         'Classifier'  : {'Type'   : 'MatrixNet',
                                                                          'File'   : '$PARAMFILESROOT/data/Hlt1TwoTrackMVA.mx'},
                                                         'GEC'         : 'Loose'},
                                        'L0Channels'  : {'TrackMVA'    : 'L0_DECISION_PHYSICS',
                                                         'TwoTrackMVA' : 'L0_DECISION_PHYSICS'},
                                        'Priorities'  : {'TrackMVA'    : 20,
                                                         'TwoTrackMVA' : 21}
                                        }

               }

    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        return ['Hlt2PassThrough', 'Hlt2Lumi', 'Hlt2DebugEvent', 'Hlt2Forward', 'Hlt2ErrorEvent', 'Hlt2Transparent', 'Hlt2B2HH']

    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        return  ['Hlt1TrackMVA', 'Hlt1TwoTrackMVA']
