// $Id: ITrajFitter.h,v 1.1 2007-04-17 15:15:39 graven Exp $
#ifndef _ITrajFitter_H
#define _ITrajFitter_H
 
#include "GaudiKernel/IAlgTool.h"
#include "Event/Measurement.h"
#include <vector>

static const InterfaceID IID_ITrajFitter( "ITrajFitter", 0, 0 );
 
/** @class ITrajFitter
 *
 *  interface for fitting a DifTraj
 *
 *  @author G.Raven  
 *  @date   15/07/2006
 */

namespace LHCb {
   template <unsigned N> class DifTraj;
}
  
class ITrajFitter: virtual public IAlgTool{
 
public:

  // unfortunately, using the following hack to emulate
  // templated typedefs doesn't seem to work...
  // so we have to type a bit more (duplicating code), and 
  // (even worse!) it becomes much less clear what is going on...
  // But these _can_ be used by the calling code...
  template <unsigned N> struct Types {
       typedef LHCb::DifTraj<N>                Traj;
       typedef ROOT::Math::SMatrix<double,1,N> Derivative;
       typedef std::pair<double, Derivative>   ResDerivative;
       typedef std::vector<ResDerivative>      ResDerivatives;
  };
                                                                                
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITrajFitter ; }
                                                                                                   
  /// Fit a DifTraj to a set of measurements
  template <unsigned N, typename MeasIter> 
  StatusCode fit(typename LHCb::DifTraj<N>& traj, 
                 MeasIter& begin, MeasIter& end) const {
      return fit(traj,LHCb::Measurement::Vector(begin,end));
  }

  template <unsigned N>
  StatusCode fit(LHCb::DifTraj<N>& traj, 
                 const LHCb::Measurement::Vector& measurements) const {
      typedef  std::vector<std::pair<double,ROOT::Math::SMatrix<double,1,N> > > R;
      return Nfit(NDifTraj(&traj),NResiduals((R*)0),measurements);
  }

  // Fit the DifTraj to the Measurements, and
  // return the vector of pairs of 
  // (residuals,derivatives wrt. the DifTraj parameters)
  template <unsigned N, typename MeasIter>
  StatusCode fit(LHCb::DifTraj<N>& traj, 
                 std::vector<std::pair<double,ROOT::Math::SMatrix<double,1,N> > >& residuals,
                 MeasIter& begin, MeasIter& end) const {
      return fit(traj,residuals,LHCb::Measurement::Vector(begin,end));

  }

  template <unsigned N>
  StatusCode fit(LHCb::DifTraj<N>& traj, 
                 std::vector<std::pair<double,ROOT::Math::SMatrix<double,1,N> > >& residuals,
                 const LHCb::Measurement::Vector& measurements) const {
      return Nfit(NDifTraj(&traj),NResiduals(&residuals),measurements);
  }

protected:
  class NDifTraj{
      public:
          template <unsigned N> explicit NDifTraj(LHCb::DifTraj<N>* obj) :  m_obj(obj), m_N(N){}
          template <unsigned N> LHCb::DifTraj<N>* get() { return N==m_N ? static_cast<LHCb::DifTraj<N>*>(m_obj) : 0; }
          unsigned int n() const { return m_N; }
      private:
          void *m_obj;
          unsigned int m_N;
  };
  class NResiduals{
      public:
          template <unsigned N> explicit NResiduals(std::vector<std::pair<double,ROOT::Math::SMatrix<double,1,N> > >* obj) :  m_obj(obj), m_N(N){}
          template <unsigned N> std::vector<std::pair<double,ROOT::Math::SMatrix<double,1,N> > >* get() 
          { return N==m_N ? static_cast<std::vector<std::pair<double,ROOT::Math::SMatrix<double,1,N> > >*>(m_obj) : 0; }
          unsigned int n() const { return m_N; }
      private:
          void *m_obj;
          unsigned int m_N;
  };

  // dispatch to implementation through a non-templated pure virtual fcn
  virtual StatusCode Nfit(NDifTraj traj,  NResiduals resids, const LHCb::Measurement::Vector& measurements) const = 0;
                                                                                
};

#endif
