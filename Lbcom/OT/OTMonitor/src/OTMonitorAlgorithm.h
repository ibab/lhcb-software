// $Id: OTMonitorAlgorithm.h,v 1.1 2004-09-10 13:13:49 cattanem Exp $
#ifndef OTMONITOR_OTMONITORALGORITHM_H_
#define OTMONITOR_OTMONITORALGORITHM_H 1

#include "GaudiKernel/Algorithm.h"

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

class IHistogram1D;
class IHistogram2D;

class OTMonitorAlgorithm: public Algorithm {

public:

  /// Constructor
  OTMonitorAlgorithm(const std::string& name, 
                     ISvcLocator* pSvcLocator); 

  /// Destructor
  virtual ~OTMonitorAlgorithm();

  /// path for histos
  std::string histoDirPath() const;
  /// convert integer to string
  std::string intToString(const int id) const;
 
  bool fullDetail() const;

private:  

 // flag of histos to produce
 bool m_fullDetail;

};

inline bool OTMonitorAlgorithm::fullDetail() const{
  return m_fullDetail;
}

#endif //OTMONITOR_OTMONITORALGORITHM_H
