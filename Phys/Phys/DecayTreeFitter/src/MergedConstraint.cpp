#include "DecayTreeFitter/VtkFitParams.h"
#include "DecayTreeFitter/VtkParticleBase.h"
#include "DecayTreeFitter/VtkMergedConstraint.h"
using std::endl;
using std::ostream;

namespace vtxtreefit
{

  ErrCode
  MergedConstraint::project(const FitParams& fitpar, 
			    Projection& p) const 
  {
    ErrCode status ;
    for(constraintlist::const_iterator it = m_list.begin() ;
	it != m_list.end() ; ++it) {
      status |= (*it)->project(fitpar,p) ;
      p.incrementOffset((*it)->dim()) ;
    }
 
    return status ;
  }

  void MergedConstraint::print(ostream& os) const
  {
    os << "Merged constraint: " << endl ;
    for(constraintlist::const_iterator it = m_list.begin() ;
	it != m_list.end() ; ++it) {
      os << "          " ;
      (*it)->print(os) ;
    }
  }

}

