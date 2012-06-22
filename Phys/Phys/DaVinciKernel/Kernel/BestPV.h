// $Id: BestPV.h,v 1.3 2009-09-14 15:54:18 jpalac Exp $
#ifndef KERNEL_BESTPV_H 
#define KERNEL_BESTPV_H 1

// Include files
#include "Kernel/IDVAlgorithm.h"
#include "Kernel/GetIDVAlgorithm.h"

/** @namespace DaVinci BestPV.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2009-08-05
 */
namespace DaVinci {

/** @class _BestPV Kernel/BestPV.h
 *  
 *  Functor that calls back to the parent DVAlgorithm in order
 *  to get the "best" PV for a given LHCb::Particle. Calls 
 *  DVAlgorithm::bestPV(const LHCb::Particle*).
 *
 *  <b>Conditions</b>
 *
 *  T must implement T::contextSvc()
 *
 *  The entity creating a BestPV must be owned at some level by a
 *  DVAlgorithm (can be a few generations removed)
 *
 *  A DaVinci::BestPV instance can be created at any moment. It is only
 *  upon calling the operator() for the first time that the contextSvc() 
 *  method is called and the DVAlgorithm used. It is therefore only necessary 
 *  that the first call be made when all tools and services are valid.
 *
 *  <b>Usage</b>
 *
 *  Make an automatic DaVinci::BestPV and use it directly:
 *
 *  @code
 *  // inside a GaudiTool
 *     const DaVinci::BestPV getBestPV(*this);
 *     ....
 *     const LHCb::Particle* p = ....
 *     const LHCb::VerrtexBase* bestPV = getBestPV(p);
 *  @endcode
 *
 *  It is also possible to avoid the intermediate instance:
 *
 *  @code
 *     const LHCb::Particle* p = ....
 *     const LHCb::VerrtexBase* bestPV = DaVinci::BestPV(*this)(p);
 *  @endcode
 *
 *  It is also possible to add it as a data member
 *
 *  @code
 *  // in the header file
 *     DaVinci::BestPV m_bestPV;
 *  // in the implementation
 *     MyTool::MyTool( ..... ) : m_bestPV(*this) { .... }
 *     void MyTool::doSomething()
 *     {
 *        const LHCb::Particle* p = ....
 *        const LHCb::VertexBase* bestPV = m_bestPV(p);
 *     }
 *
 *
 *  @author Juan PALACIOS
 *  @date   2009-08-05
 */
  template <class T>
  class _BestPV
  {
  public:
    _BestPV(const T& parent)
      :
      m_parent(parent),
      m_dvAlg(0)
    {
    }

    inline const LHCb::VertexBase* 
    operator()(const LHCb::Particle* particle) const 
    {
      if (!m_dvAlg) {
        m_dvAlg = Gaudi::Utils::getIDVAlgorithm(m_parent.contextSvc());
      }
      return (m_dvAlg) ? m_dvAlg->bestVertex(particle) : 0; 
    }

  private :
    const T& m_parent;
    mutable IDVAlgorithm* m_dvAlg;
  private :
    _BestPV() { }

  };
  typedef _BestPV<GaudiTool> BestPV; ///< The most useful specialization
} // namespace DaVinci
#endif // KERNEL_BESTPV_H
