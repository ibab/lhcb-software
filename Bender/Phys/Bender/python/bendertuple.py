#!/usr/bin/env python
# =============================================================================
# $Id: bendertuple.py,v 1.2 2004-08-26 19:34:36 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.1  2004/07/11 15:47:06  ibelyaev
#  regular incrment
#
# =============================================================================

# =============================================================================
# @file
#
# defintion of all Bender Tuples 
#
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

# import global namespace  
import gaudimodule as gaudi 

gbl = gaudi.gbl

# load the 'minimal' set of dictionaries
gaudi.loaddict('BenderDict')

# load the 'intermediate object 
BenderTuple   = gbl.Bender.BenderTuple

# extend the intermediate tuple 
class Tuple(BenderTuple):
    """
    The basic object for easy manipulation with N-Tuples
    The underlying C++ classes are:
    - Bender::BenderTuple   (Tools/BenderLibs)   $BENDERLIBSROOT/Bender/Tuple.h
    - LoKi::Tuple           (Tools/LoKi)         $LOKIROOT/LoKi/Tuple.h
    - LoKi::TupleObj        (Tools/LoKi)         $LOKIROOT/LoKi/TupleObj.h
    - NTuple::Tuple         (GaudiKernel)        $GAUDIKERNELROOT/GaudiKernel/NTuple.h
    - INTuple               (GaudiKernel)        $GAUDIKERNELROOT/GaudiKernel/INTuple.h
    """
    def __init__( self ,  obj ) :
        """
        Constructor from BenderTuple object
        """
        BenderTuple.__init__ ( self , obj )
        
    def column  ( self , **args ) : 
        """
        Fill single N-Tuple column. The column can be primitive or specialized.
        Usage :
        tuple.column( name = 'pt' , value = pt / GeV )
        where :
        -  tuple    is the instance of Tuple object
        - 'name'    is column name 
        - 'value'   is column value
        - 'min'     is minimal value for integer column  (optional)
        - 'max'     is maximal value for integer columnt (optional)
        Currently supported single column types are
        - float (32bits)
        - long  (32bits)
        - bool    
        - Hep3Vector        (CLHEP)       CLHEP/Vector/ThreeVector.h 
        - HepPoint3D        (CLHEP)       CLHEP/Geometry/Point3D.h 
        - HepVector3D       (CLHEP)       CLHEP/Geometry/Vector3D.h 
        - HepLorentzVector  (CLHEP)       CLHEP/Vector/LorentzVector.h
        - EventHeader       (Event/Event) $EVENTROOT/Event/EventHeader.h
        """
        if not args.has_key('name') :
            raise TypeError, " Column 'name'  is not specified "
        if not args.has_key('value') :
            raise TypeError, " Column 'value' is not specified "
        name  = args.get ( 'name'  )
        value = args.get ( 'value' )
        if args.has_key('min') | args.has_key( 'max' ) :
            # "size-like" column 
            minv   = args.get( 'min' ,   0 )
            maxv   = args.get( 'max' , 500 ) 
            return BenderTuple.column( self , name , value , minv , maxv )
        # ordinary-type column
        return BenderTuple.column( self , name , value )
    def write  ( self ) :
        """
        Commit the filled row into N-Tuple
        """
        return BenderTuple.write( self ) ;
    def commit ( self ) :
        """
        Commit the filled row into N-Tuple
        """
        return BenderTuple.write( self ) ;

    
