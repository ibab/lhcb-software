#!/usr/bin/env python
# =============================================================================
# $Id: bendertuple.py,v 1.4 2005-02-10 12:26:12 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
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
        
    def column  ( self , name , value  , **args ) : 
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
        - Hep3Vector        (CLHEP)          CLHEP/Vector/ThreeVector.h 
        - HepPoint3D        (CLHEP)          CLHEP/Geometry/Point3D.h 
        - HepVector3D       (CLHEP)          CLHEP/Geometry/Vector3D.h 
        - HepLorentzVector  (CLHEP)          CLHEP/Vector/LorentzVector.h
        - IOpaqueAddress    (GaudiKernel)    $GAUDIKERNELROOT/GaudiKernel/IOpaqueAddress.h
        - EventHeader       (Event/Event)    $EVENTROOT/Event/EventHeader.h
        - L0DUReport        (Event/L0Event)  $L0EVENTROOT/Event/L0DUReport.h
        - L1Score           (Event/L1Event)  $L1EVENTROOT/Event/L1Score.h
        - TrgDecision       (Event/TrgEvent) $TRGEVENTROOT/Event/TrgDecision.h
        """
        if not args :
            # ordinary-type column
            return BenderTuple.column_( self , name , value )
        elif   args.has_key('max') or args.has_key( 'min' ) :
            # "size-like" column 
            minv   = args.get( 'min' ,    0 )
            maxv   = args.get( 'max' , 1000 ) 
            return BenderTuple.column_( self , name , value , minv , maxv )
        # invalid arguments 
        raise TypeError, " Invalid arguments "
    
    
