#ifndef      GIGA_GIGACNVFUNCTORS_H
#define      GIGA_GIGACNVFUNCTORS_H 1 


#include "LHCbEvent/MCVertex.h"
#include "GiGa/GiGaTrajectoryPoint.h"


namespace GiGaCnvFunctors
{ 
  ///
  class MCVerticesCompare: 
    public std::binary_function<bool,const MCVertex*, const MCVertex*>
  {
  public:
    ///
    inline bool operator() ( const MCVertex* v1 , const MCVertex* v2 ) const 
    {
      return 
	( v1 == v2                                 ) ? true  :
	( 0  == v1                                 ) ? false :
	( 0  == v2                                 ) ? false : 
	( v1->timeOfFlight() != v2->timeOfFlight() ) ? false : 
	( v1->position()     != v2->position    () ) ? false : true ; 
    }  
    ///
  };
  ///
  class MCVertexToPointCompare: 
    public std::unary_function<bool,const MCVertex*>
  {
    ///
    const GiGaTrajectoryPoint* m_p;
    ///
  public: 
    explicit MCVertexToPointCompare( const GiGaTrajectoryPoint* point ) 
      : m_p( point ){};
    ///
    inline bool operator() ( const MCVertex* v ) const 
    {
      return 
	( (void*) v  == (void*) m_p                ) ? true  :
	( 0  == v                                  ) ? false :
	( 0  == m_p                                ) ? false : 
	( v->timeOfFlight()  != m_p->time()        ) ? false : 
        ( v->position()      != m_p->GetPosition() ) ? false : true ; 
    }  
  };
  ///
};


#endif  //   GIGA_GIGACNVFUNCTORS_H

