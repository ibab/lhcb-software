#ifndef DECAYTREEFITTER_CONSTRAINT_H
#define DECAYTREEFITTER_CONSTRAINT_H

#include <string>
#include <iostream>
#include "VtxErrCode.h"

namespace DecayTreeFitter
{
  class ParticleBase ;
  class Projection ;
  class FitParams ;

  class Constraint
  {
  public:
    // the order of these constraints is important: it is the order in
    // which they are applied.

    enum Type { unknown=0,
		beamspot,
		beamenergy,
		lifetime,
		btaresonance,
		btacomposite,
		track,
		photon,
		conversion,
		kinematic,
		massEnergy,
		geometric,
		mass,
		merged,
		externalmomentum,
		ntypes} ;
    
    bool operator<(const Constraint& rhs) const ;
    
    bool operator==(const Constraint& rhs) const { 
      return m_type == rhs.m_type ; }
    
    // accessors
    Type type() const { return m_type ; }
    unsigned int dim() const { return m_dim ; }
    bool isLineair() const { return m_maxNIter <=1 ; }
    unsigned int nIter() const { return m_maxNIter ; }

    Constraint() : m_node(0),m_depth(0),m_type(unknown),m_dim(0) {}

    Constraint( const ParticleBase* node, Type type, int depth, 
		unsigned int dim, 
		int maxniter=1 /*, double precision=1e-5*/)
      : m_node(node), m_depth(depth), m_type(type), m_dim(dim), 
	m_weight(1), m_maxNIter(maxniter) {}
    
    virtual ~Constraint() {}

    virtual ErrCode project(const FitParams& fitpar, Projection& p) const ;
    virtual ErrCode filter(FitParams& fitpar) const ;
    virtual ErrCode filter(FitParams& fitpar,
			   const FitParams& reference) const ;
    virtual void print(std::ostream& os=std::cout) const ;
    std::string name() const ;

    // set to minus one if constraints needs to be removed on next filter
    void setWeight(int w) { m_weight = w<0 ? -1 : 1 ; }
    

  protected:
    Constraint(Constraint::Type type) :
      m_node(0),m_depth(0),m_type(type),m_dim(0),
      m_weight(0),m_maxNIter(0) {}
    void setDim(unsigned int d) { m_dim = d ; }
    void setNIter(unsigned int d) { m_maxNIter = d ; }
  private:
    const ParticleBase* m_node ;
    int m_depth ;
    Type m_type ;
    unsigned int m_dim ;
    // the weight: guassian constraint can be 'unfilter'
    int m_weight ; 
    int m_maxNIter ;     // maximum number of iterations for non-linear constraints
  } ;

}

#endif
