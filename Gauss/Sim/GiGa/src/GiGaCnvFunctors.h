#ifndef      GIGA_GIGACNVFUNCTORS_H
#define      GIGA_GIGACNVFUNCTORS_H 1 


#include "LHCbEvent/MCVertex.h"
#include "GiGa/GiGaTrajectoryPoint.h"


namespace GiGaCnvFunctors
{ 
  /// ordering criteria                     
  class MCVerticesLess   :  
    public std::binary_function<const MCVertex*, const MCVertex*,bool>
  {
  public:
    ///
    inline bool operator() ( const MCVertex* v1 , const MCVertex* v2 ) const 
    {
      return 
	( 0  == v1                                 ) ? true  :
	( 0  == v2                                 ) ? false :
	( v1 == v2                                 ) ? false :
	( v1->timeOfFlight() <  v2->timeOfFlight() ) ? true  : 
	( v1->timeOfFlight() >  v2->timeOfFlight() ) ? false : 
	( v1->position().z() <  v2->position().z() ) ? true  : 
	( v1->position().z() >  v2->position().z() ) ? false : 
	( v1->position().x() <  v2->position().x() ) ? true  : 
	( v1->position().x() >  v2->position().x() ) ? false : 
	( v1->position().y() <  v2->position().y() ) ? true  : false ; 
    }  
    ///
  };
  ///
  class MCVerticesEqual  : 
    public std::binary_function<const MCVertex*, const MCVertex*,bool>
  {
  public:
    ///
    inline bool operator() ( const MCVertex* v1 , const MCVertex* v2 ) const 
    {
      return 
	( 0  == v1 || 0 == v2                      ) ? false :
	( v1 == v2                                 ) ? true  :
	( v1->timeOfFlight() != v2->timeOfFlight() ) ? false : 
	( v1->position    () != v2->position    () ) ? false : true ; 
    }  
    ///
  };
  ///
};


#endif  //   GIGA_GIGACNVFUNCTORS_H















