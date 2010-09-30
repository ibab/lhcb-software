"""
Stripping options for (pre-)selecting B -> hh', B -> p pbar
and Bs -> mu mu events.

Uses the selection framework.

Authors: Angelo Carboni, Stefano Perazzini, Eduardo Rodrigues

Usage:
from StrippingConf.StrippingStream                import StrippingStream
from StrippingSelections.StrippingHb2Charged2Body import Hb2Charged2BodyLines
stream = StrippingStream( 'Bhadron' )
stream.appendLines( Hb2Charged2BodyLines().lines() )
"""

########################################################################

from Configurables import CombineParticles

from PhysSelPython.Wrappers      import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingSelections.Utils   import checkConfig 



class Hb2Charged2BodyLines( object ) :
    """Class defining the Hb -> hh stripping lines"""
    def __init__( self,
                  moduleName = 'Hb2Charged2BodyLines',
                  config = {
                    'Prescales' : {   # Prescale fractions for individual lines
	                 'B2Charged2Body' : 1.,
                         'B2PPbarLine'    : 1.
                    }, 
                    'CheckPV' : True  # checkPV condition for all lines 
                  }
                ) :
        
        # variable storing the list of stripping lines
        self.__lines = []
        
        # check the class configuration
        __configuration_keys__ = ( 'Prescales', 'CheckPV' )
        
        checkConfig( __configuration_keys__, config )
        
        # make the various stripping selections
        B2Charged2Body = makeB2Charged2Body( moduleName )        
        B2PPbar        = makeB2PPbar       ( moduleName )
        
        Hb2Charged2BodySelections = {
            'B2Charged2Body' : B2Charged2Body,
            'B2PPbarLine'    : B2PPbar
            }

        # make the list of stripping lines
        for name, sel in Hb2Charged2BodySelections.iteritems() :
            line = StrippingLine( name,
                                  prescale = config[ 'Prescales' ][ name ],
                                  algos    = [ sel ],
                                  checkPV  = config[ 'CheckPV' ]
                                  )
            self.__lines.append( line )

    def lines( self ) :
        """Get the list of the stripping lines instances"""
        return self.__lines
    
    def lineNames( self ) :
        """Retrieve the names of all stripping lines"""
        return [ line.name() for line in self.__lines ]
    
    def line( self, line_name ) :
        """Get the instance of a given stripping line"""
        stripping_line = [ line for line in self.__lines if line.name() == line_name ]
        if stripping_line == [] :
            raise ValueError( 'No such stripping line with name "%s"!' % line_name )
        return stripping_line


def makeB2Charged2Body( moduleName ) :
    """Make a SelPy.selection Hb -> h h Selection object for a given stripping module name.
    Example module: Hb2Charged2BodyLines"""
    stdLoosePions = DataOnDemand( Location = 'Phys/StdLoosePions' )
    
    PreselHb2Charged2Body = CombineParticles( 'PreselHb2Charged2Body' )
    PreselHb2Charged2Body.DecayDescriptor = 'B0 -> pi+ pi-'
    PreselHb2Charged2Body.DaughtersCuts = { "pi+" : "(PT>700*MeV) & (MIPDV(PRIMARY)>0.08) & (TRCHI2DOF<5)"}
    PreselHb2Charged2Body.CombinationCut = "(AM > 4500*MeV) & (AM < 6500*MeV) & (AMAXDOCA('') < 0.08*mm) & (AMAXCHILD(MAXTREE('pi+'==ABSID,PT))>2400) & (AMAXCHILD(MAXTREE('pi+'==ABSID,MIPDV(PRIMARY)))>0.2)"
    PreselHb2Charged2Body.MotherCut = "(M>4800*MeV) & (M<5800*MeV) & (BPVIP()<0.06) & (BPVVDSIGN>1.8)"
    
    return Selection( 'SelHb2Charged2BodyFor' + moduleName,
                      Algorithm = PreselHb2Charged2Body,
                      RequiredSelections = [ stdLoosePions ] )    


def makeB2PPbar( moduleName ) :
    """Make a SelPy.selection B -> p pbar Selection object for a given stripping module name.
    Example module: Hb2Charged2BodyLines"""
    
    stdLooseProtons = DataOnDemand( Location = 'Phys/StdLooseProtons' )
    
    PreselB2PPbar = CombineParticles( 'PreselB2PPbar' )
    PreselB2PPbar.DecayDescriptor = 'B0 -> p+ p~-'
    PreselB2PPbar.DaughtersCuts = { 'p+' : '(PT>900*MeV) & (TRCHI2DOF<5) & ((PIDp-PIDpi)>0) & ( (PIDp-PIDK) > -2 ) & (MIPCHI2DV(PRIMARY)>9)' }
    PreselB2PPbar.CombinationCut = "(ADAMASS('B0') < 200*MeV) & ( AMAXCHILD(MAXTREE('p+'==ABSID,PT)) > 2500*MeV )"
    PreselB2PPbar.MotherCut = '( VFASPF(VCHI2PDOF) < 16 ) & ( BPVIPCHI2() < 36 ) & ( BPVDIRA > 0.9995 )'
    
    return Selection( 'SelB2PPbarFor' + moduleName,
                      Algorithm = PreselB2PPbar,
                      RequiredSelections = [ stdLooseProtons ] )


########################################################################  
