#ifndef VTK_MERGEDCONSTRAINT_HH
#define VTK_MERGEDCONSTRAINT_HH

#include <vector>
#include "Constraint.h"

namespace DecayTreeFitter
{
  class MergedConstraint : public Constraint
  {
  public:
    typedef std::vector<Constraint*> constraintlist ;

    MergedConstraint() : Constraint(Constraint::merged) {}
    virtual ~MergedConstraint() {}

    MergedConstraint( const constraintlist& list ) :
      Constraint(Constraint::merged),m_list(list) {
      int d(0) ;
      for(constraintlist::iterator it = m_list.begin() ;
	  it != m_list.end(); ++it) d += (*it)->dim() ;
      setDim(d) ;
    }

    virtual ErrCode project(const FitParams& fitpar, Projection& p) const ;
    
    void push_back(Constraint* c) { 
      m_list.push_back(c) ; 
      setDim(dim()+c->dim()) ; 
      setNIter(std::max(nIter(),c->nIter())) ;
    }

    virtual void print(std::ostream& os=std::cout) const ;

  private:
    constraintlist m_list ;
  } ;

}

#endif
