// $Id: ITrajFitter.h,v 1.5 2007-05-03 13:20:53 cattanem Exp $
#ifndef _ITrajFitter_H
#define _ITrajFitter_H
 
// Include files
// -------------
#include "GaudiKernel/IAlgTool.h"
#include "Event/Measurement.h"
#include <vector>

static const InterfaceID IID_ITrajFitter( "ITrajFitter", 0, 0 );
 
/** @class ITrajFitter ITrajFitter.h TrackInterfaces/ITrajFitter.h
 *  
 *  Interface for fitting a 'DifTraj' to a set of Measrements.
 *
 *  @author G.Raven  
 *  @date   2007-04-17
 */

// Forward declarations
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
                                                                                                   
/** Fit a DifTraj to a set of measurements.
  * @param traj  on input: initial guess for DifTraj<N>; on output: updated to reflect the fit
  * @param begin begin iterator over Measurements used to fit the DifTraj<N>
  * @param end   end   iterator over Measurements used to fit the DifTraj<N>
  */
  template <unsigned N, typename MeasIter> 
  StatusCode fit(typename LHCb::DifTraj<N>& traj, 
                 MeasIter& begin, MeasIter& end) const {
      return fit(traj,LHCb::Measurement::Vector(begin,end));
  }

/** Fit a DifTraj to a set of measurements.
  * @param traj         on input: initial guess for DifTraj<N>; on output: updated to reflect the fit
  * @param measurements vector Measurements used to fit the DifTraj<N>
  */
  template <unsigned N>
  StatusCode fit(LHCb::DifTraj<N>& traj, 
                 const LHCb::Measurement::Vector& measurements) const {
      typedef  std::vector<std::pair<double,ROOT::Math::SMatrix<double,1,N> > > R;
      return Nfit(NDifTraj(&traj),NResiduals((R*)0),measurements);
  }

/** Fit a DifTraj to a set of measurements.
  * @param traj on input: initial guess for DifTraj<N>; on output: updated to reflect the fit
  * @param residuals on input: an empty vector; 
  *                  on output: contains, for each measurement, a pair with the (normalized)
  *                   residual, and its derivative wrt the N parameters of the DifTraj<N>
  * @param begin begin iterator over Measurements used to fit the DifTraj<N>
  * @param end   end   iterator over Measurements used to fit the DifTraj<N>
  */
  template <unsigned N, typename MeasIter>
  StatusCode fit(LHCb::DifTraj<N>& traj, 
                 std::vector<std::pair<double,ROOT::Math::SMatrix<double,1,N> > >& residuals,
                 MeasIter& begin, MeasIter& end) const {
      return fit(traj,residuals,LHCb::Measurement::Vector(begin,end));

  }

/** Fit a DifTraj to a set of measurements.
  * @param traj on input: initial guess for DifTraj<N>; on output: updated to reflect the fit
  * @param residuals on input: an empty vector; 
  *                  on output: contains for each measurement, a pair with the (normalized)
  *                   residual, and its derivativ wrt the N parameters of the DifTraj<N>
  * @param[in] measurements vector of Measurements used to fit the DifTraj<N>
  */
  template <unsigned N>
  StatusCode fit(LHCb::DifTraj<N>& traj, 
                 std::vector<std::pair<double,ROOT::Math::SMatrix<double,1,N> > >& residuals,
                 const LHCb::Measurement::Vector& measurements) const {
      return Nfit(NDifTraj(&traj),NResiduals(&residuals),measurements);
  }

protected:
  /// Helper classes to go from templated to non-templated in such 
  /// a way that one can retrieve the templated input back later,
  /// without loss of type-safety. Needed because we need to dispatch
  /// to the actual implementation through a (non-templated!) virtual function.
  /// (cannot have templated virtual functions: since they are not a bound
  ///  set, that would imply a (potentially) infinite v-table)
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

  /// dispatch to actual implementation through 
  /// a non-templated pure virtual fcn
  virtual StatusCode Nfit(NDifTraj traj,  NResiduals resids, const LHCb::Measurement::Vector& measurements) const = 0;
                                                                                
};

#endif
