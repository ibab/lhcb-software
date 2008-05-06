// $Id: InstantaneousRate.h,v 1.1 2008-05-06 12:18:20 jucogan Exp $
#ifndef COMPONENT_INSTANTANEOUSRATE_H 
#define COMPONENT_INSTANTANEOUSRATE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"

static const InterfaceID IID_InstantaneousRate ( "InstantaneousRate", 1, 0 );

/** @class InstantaneousRate InstantaneousRate.h component/InstantaneousRate.h
 *  
 *
 *  @author 
 *  @date   2008-04-16
 */
class InstantaneousRate : public GaudiHistoTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_InstantaneousRate; }

  /// Standard constructor
  InstantaneousRate( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~InstantaneousRate( ); ///< Destructor

  void bookHistos();
  void fillHistos();

  double rate();
  int bunchId();
  int deltaT()
protected:

private:

  double m_max;
  AIDA::IHistogram1D * m_hrate;
  AIDA::IHistogram1D * m_hrate2;
  AIDA::IHistogram1D * m_hbid;
  AIDA::IHistogram1D * m_hbid2;
  AIDA::IHistogram1D * m_hdt;
  
  unsigned int m_previous_orbit;
  ulonglong m_previous_event;

};
#endif // COMPONENT_INSTANTANEOUSRATE_H
