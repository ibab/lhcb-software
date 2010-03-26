class Commissioning_Beamgas :
    """
    Threshold settings for Hlt1 for 2010 conditions
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven
    @date 2009-08-26
    """
    
    __all__ = ( 'ActiveHlt1Lines', 'ActiveHlt2Lines', 'Thresholds','L0TCK' )

    def verifyType(self,ref) :
        # verify self.ActiveLines is still consistent with
        # our types self.ActiveHlt2Lines and self.ActiveHlt1Lines...
        # so we can force that classes which inherit from us
        # and overrule either ActiveHlt.Lines also overrule
        # HltType...
        if ( self.ActiveHlt1Lines() != ref.ActiveHlt1Lines(self)  or
             self.ActiveHlt2Lines() != ref.ActiveHlt2Lines(self) ) :
            raise RuntimeError('Must update HltType when modifying ActiveHlt.Lines()')

    
    def L0TCK(self) :
        return '0x1810'

    def HltType(self) :
        self.verifyType( Commissioning_Beamgas )
        return          'Commissioning_Beamgas'
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        return dict()

    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        return [ ]

        
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        return [  'Hlt1BeamGasBeam1', 'Hlt1BeamGasBeam2', 'Hlt1BeamGasCrossing', 'Hlt1BeamGasCrossingForcedRZReco' ]
