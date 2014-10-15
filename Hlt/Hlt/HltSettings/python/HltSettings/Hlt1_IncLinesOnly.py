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

class Hlt1_IncLinesOnly( object ):
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
        return '0x0044'

    def HltType(self) :
        self.verifyType( Hlt1_IncLinesOnly )
        return          'Hlt1_IncLinesOnly'

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """

        from Hlt1Lines.Hlt1PhiLines            import Hlt1PhiLinesConf
        from Hlt1Lines.Hlt1CharmLines          import Hlt1CharmLinesConf

        thresholds = {  Hlt1PhiLinesConf :    {   'TrackPT'            : 800     # MeV
                                                  ,'TrackP'             : 4000    # MeV
                                                  ,'TrackChi2DOF'       : 5       # dimensionless
                                                  ,'PhiMassWin'         : 30      # MeV
                                                  ,'PhiDOCA'            : 0.2     # mm
                                                  ,'PhiVCHI2'           : 20      # dimensionless
                                                  ,'Velo_Qcut'          : 3       # dimensionless
                                                  ,'TrNTHits'           : 16.
                                                  ,'ValidateTT'         : False
                                                }
                       , Hlt1CharmLinesConf :   {  'TrackPT'           : 500     # MeV
                                                  ,'TrackP'            : 2000    # MeV
                                                  ,'TrackChi2DOF'      : 5       # dimensionless
                                                  ,'D0MassWin'         : 100      # MeV
                                                  ,'D0DOCA'            : 0.2     # mm
                                                  ,'D0VCHI2'           : 20      # dimensionless
                                                  ,'Velo_Qcut'         : 3       # dimensionless
                                                  ,'ValidateTT'        : False
                                                }
                     }

        return thresholds

    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        lines = ['Hlt1IncPhi','Hlt1IncCharm']

        return lines


    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        return []
