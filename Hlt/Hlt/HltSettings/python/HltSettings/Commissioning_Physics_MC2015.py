from Commissioning_Physics_2015 import Commissioning_Physics_2015

class Commissioning_Physics_MC2015(Commissioning_Physics_2015):
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
        return '0xFF66'

    def HltType(self) :
        self.verifyType( Commissioning_Physics_MC2015 ) 
        return          'Commissioning_Physics_MC2015'
