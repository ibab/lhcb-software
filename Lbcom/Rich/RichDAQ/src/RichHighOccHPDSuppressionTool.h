
//-----------------------------------------------------------------------------
/** @file RichHighOccHPDSuppressionTool.h
 *
 *  Header file for tool : RichHighOccHPDSuppressionTool
 *
 *  CVS Log :-
 *  $Id: RichHighOccHPDSuppressionTool.h,v 1.4 2006-03-22 14:19:31 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RichHighOccHPDSuppressionTool_H
#define RICHDAQ_RichHighOccHPDSuppressionTool_H 1

// STD
#include <sstream>

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichKernel/RichToolBase.h"

// Interfaces
#include "RichKernel/IRichPixelSuppressionTool.h"

// numberings
#include "RichDet/RichDAQDefinitions.h"

// Kernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichHashMap.h"

// RichDet
#include "RichDet/DeRichSystem.h"

// Boost
#include "boost/lexical_cast.hpp"

// DetDesc
#include "DetDesc/Condition.h"

// namespaces
using namespace LHCb; ///< LHCb general namespace

//-----------------------------------------------------------------------------
/** @class RichHighOccHPDSuppressionTool RichHighOccHPDSuppressionTool.h
 *
 *  Tool for monitoring high occupancy HPDs
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   21/03/2006
 */
//-----------------------------------------------------------------------------

class RichHighOccHPDSuppressionTool : public RichToolBase,
                                      virtual public IRichPixelSuppressionTool
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichHighOccHPDSuppressionTool( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

  // Initialization of the tool after creation
  StatusCode initialize();

  // Finalization of the tool before deletion
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from interface

  // Monitor the occupancy of the given HPD
  bool applyPixelSuppression( const LHCb::RichSmartID hpdID,
                              LHCb::RichSmartID::Vector & smartIDs ) const;

private: // private methods

  /// Print out the XML entries for the background conditions
  void createHPDBackXML() const;

  /// Initialise occupancy map
  StatusCode initOccMap( );

  /// Initialise occupancy map for given RICH
  StatusCode initOccMap( const Rich::DetectorType rich );

  /// UMS method for RICH1
  StatusCode umsUpdateRICH1();

  /// UMS method for RICH2
  StatusCode umsUpdateRICH2();

protected: // protected data

  /// Rich System detector element
  const DeRichSystem * m_richSys;

protected: // utility classes

  //-----------------------------------------------------------------------------
  /** @class HPDData RichHighOccHPDSuppressionTool.h
   *
   *  Utility class hold data for a single HPD
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   21/03/2006
   */
  //-----------------------------------------------------------------------------
  class HPDData : private std::pair<long unsigned,double>
  {
  public:
    /// Constructor
    HPDData( const long unsigned tally = 0, const double occ = 0 )
      : std::pair<long unsigned,double>(tally,occ) { }
    /// Access the number of data entries for the HPD
    inline long unsigned & fillCount()      { return this->first;  }
    /// Access the occupancy for the HPD
    inline double & avOcc()                 { return this->second; }
    /// Const Access the number of data entries for the HPD
    inline long unsigned fillCount() const  { return this->first;  }
    /// Const Access the occupancy for the HPD
    inline double avOcc() const             { return this->second; }
  };

  /// Find HPD data for given HPD RichSmartID
  void findHpdData( const LHCb::RichSmartID hpdID ) const;

  /// Returns current HPD Data object
  inline HPDData & hpdData() const { return *m_currentData; }

private: // private data

  /// Occupancy map
  typedef Rich::HashMap< const LHCb::RichSmartID, HPDData > OccMap;
  mutable OccMap m_occMap;

  // Min number fills before applying occupancy cut
  unsigned int m_minFills;

  // absolute max HPD occupancy
  unsigned int m_overallMax;

  /// Occupancy scale cut
  double m_scale;

  /// Memory fraction
  double m_memory;

  /// Flag to turn on/off the printing of the XML conditions
  bool m_printXML;

  /// Flag to turn on/off reading of data from Conditions
  bool m_readFromCondDB;

  /// Location of occupancies in DB
  std::vector<std::string> m_condBDLocs;

  mutable HPDData * m_currentData; ///< Pointer to the Data for the current HPD

};

inline void
RichHighOccHPDSuppressionTool::findHpdData( const LHCb::RichSmartID hpdID ) const
{
  // get data for this HPD
  OccMap::iterator iD = m_occMap.find(hpdID);
  if ( iD == m_occMap.end() )
  {
    std::ostringstream mess;
    mess << "Unknown HPD RichSmartID " << hpdID;
    throw GaudiException( mess.str(),
                          "RichHighOccHPDSuppressionTool",
                          StatusCode::FAILURE );
  }
  m_currentData = &(*iD).second;
}

#endif // RICHDAQ_RichHighOccHPDSuppressionTool_H
