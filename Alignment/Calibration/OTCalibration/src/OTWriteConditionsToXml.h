// $Id: OTWriteConditionsToXml.h,v 1.1.1.1 2009-04-21 10:57:19 jblouw Exp $
#ifndef OTWRITECONDITIONSTOXML_H 
#define OTWRITECONDITIONSTOXML_H 1

// Include files
// from STD
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <fstream>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from Kernel
#include "Kernel/OTChannelID.h"

// from OTDet
#include "OTDet/DeOTQuarter.h"
#include "OTDet/DeOTModule.h"

/** @class OTWriteConditionsToXml OTWriteConditionsToXml.h
 *  Simple class to write status/calibration conditions to xml.  
 *  
 *  Note I assume that the conditions are per Tell1 or rather quarter. 
 *  So a file contains the coditions for all the modules in that quarter.
 *  Seems logical.
 *
 *  @author Jan Amoraal
 *  @date   2008-09-18
 */

class OTWriteConditionsToXml : public GaudiAlgorithm {

  public: 
  /// Standard constructor
  OTWriteConditionsToXml( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~OTWriteConditionsToXml( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  protected:
  
  // Private methods
  private:
  // some methods to return a nickname. Maybe we want to do something similar as the ST(?)
  std::string qNickname( const LHCb::OTChannelID& channel ) const {
    /// I assume the following order:
    /// T1 T2 T3
    /// X1 U V X2
    /// Q0 Q1 Q2 Q3
    /// M1 M2 M3 M4 M5 M6 M7 M8 M9
    return m_names[ channel.station() - 1u ] + m_names[ channel.layer() + 3 ] + m_names[ channel.quarter() + 7 ] ;
  };
   
  std::string mNickname( const LHCb::OTChannelID& channel ) const {
    /// T1 T2 T3
    /// X1 U V X2
    /// Q0 Q1 Q2 Q3
    /// M1 M2 M3 M4 M5 M6 M7 M8 M9
    return qNickname( channel ) + m_names[ channel.module() + 10 ];
  };

  StatusCode writeConditions( const std::string& prefix, const unsigned& precision ) const;
  
  // Private members
  private:
  
  std::vector< std::string >                       m_names;
  bool                                             m_writeCalibration;
  bool                                             m_writeStatus;
  std::string                                      m_calibrationPrefix;
  std::string                                      m_statusPrefix;
  unsigned                                         m_precision;
  typedef LHCb::OTChannelID QChannelID;
  std::map< QChannelID, std::vector<DeOTModule*> > m_mapQtoM;
};
#endif // OTWRITECONDITIONSTOXML_H
