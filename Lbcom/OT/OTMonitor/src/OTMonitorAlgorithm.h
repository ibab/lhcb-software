// $Id: OTMonitorAlgorithm.h,v 1.3 2006-01-30 13:42:55 janos Exp $
#ifndef OTMONITOR_OTMONITORALGORITHM_H_
#define OTMONITOR_OTMONITORALGORITHM_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

/** @class OTMonitorAlgorithm OTMonitorAlgorithm.h \
 *         "OTMonitor/OTMonitorAlgorithm.h"
 *
 *  Base class for the OT monitoring and checking algorithms. Inherits
 *  from Algorithm base class. Includes a method to get the histogram 
 *  path and has a member function to convert integers to strings. 
 *
 *  @author M.Needham
 *  @date   21/4/2001
 *  @Adapted J. Nardulli
 *  @date   14/7/2004
 */

class OTMonitorAlgorithm: public GaudiAlgorithm {
  
 public:

  /// Constructor
  OTMonitorAlgorithm( const std::string& name, 
                      ISvcLocator* pSvcLocator ); 

  /// Destructor
  virtual ~OTMonitorAlgorithm();

  /// path for histos
  std::string histoDirPath() const;
  /// convert integer to string
  std::string intToString( const int id ) const;
 
  bool fullDetail() const;

 private:  

  // flag of histos to produce
  bool m_fullDetail;

};

inline bool OTMonitorAlgorithm::fullDetail() const{
  return m_fullDetail;
}

#endif //OTMONITOR_OTMONITORALGORITHM_H
