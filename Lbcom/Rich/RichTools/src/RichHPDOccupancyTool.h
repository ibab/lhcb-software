
//-----------------------------------------------------------------------------
/** @file RichHPDOccupancyTool.h
 *
 *  Header file for tool : Rich::HPDOccupancyTool
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RichHPDOccupancyTool_H
#define RICHDAQ_RichHPDOccupancyTool_H 1

// STD
#include <sstream>

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
// base class
#include "RichKernel/RichToolBase.h"

// Interfaces
#include "RichKernel/IRichHPDOccupancyTool.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"

// numberings
#include "RichKernel/RichDAQDefinitions.h"

// Kernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichHashMap.h"

// RichDet
#include "RichDet/DeRichSystem.h"

// Boost
#include "boost/lexical_cast.hpp"

// DetDesc
#include "DetDesc/Condition.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class HPDOccupancyTool RichHPDOccupancyTool.h
   *
   *  Tool for monitoring high occupancy HPDs
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   21/03/2006
   */
  //-----------------------------------------------------------------------------

  class HPDOccupancyTool : public Rich::ToolBase,
                           virtual public IHPDOccupancyTool,
                           virtual public IIncidentListener
  {

  public: // Methods for Gaudi Framework

    /// Standard constructor
    HPDOccupancyTool( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

    // Initialization of the tool after creation
    StatusCode initialize();

    // Finalization of the tool before deletion
    StatusCode finalize();

  public:

    /** Implement the handle method for the Incident service.
     *  This is used to inform the tool of software incidents.
     *
     *  @param incident The incident identifier
     */
    void handle( const Incident& incident );

  public: // methods (and doxygen comments) inherited from interface

    // Returns the average occupancy for the given HPD
    double averageOccupancy( const LHCb::RichSmartID hpdID ) const;

  private: // private methods

    /// Print out the XML entries for the background conditions
    void createHPDBackXML( const Rich::DetectorType rich ) const;

    /// Initialise occupancy map
    StatusCode initOccMap( );

    /// Initialise occupancy map for given RICH
    StatusCode initOccMap( const Rich::DetectorType rich );

    /// UMS method for RICH1
    StatusCode umsUpdateRICH1();

    /// UMS method for RICH2
    StatusCode umsUpdateRICH2();

    /// Find HPD data for given HPD RichSmartID
    void findHpdData( const LHCb::RichSmartID hpdID ) const;

    /// Update occupancies
    void updateOccupancies() const;

  protected: // protected data

    /// Rich System detector element
    const DeRichSystem * m_richSys = nullptr;

  protected: // utility classes

    //-----------------------------------------------------------------------------
    /** @class HPDData RichHPDOccupancyTool.h
     *
     *  Utility class to hold occupancy data for a single HPD
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   21/03/2006
     */
    //-----------------------------------------------------------------------------
    class HPDData : private std::pair<unsigned long long,double>
    {
    public:
      /// Constructor
      HPDData( const unsigned long long tally = 0, const double occ = 0 )
        : std::pair<unsigned long long,double>(tally,occ) { }
      /// Access the number of data entries for the HPD
      inline unsigned long long & fillCount()      { return this->first;  }
      /// Access the occupancy for the HPD
      inline double & avOcc()                      { return this->second; }
      /// Const Access the number of data entries for the HPD
      inline unsigned long long fillCount() const  { return this->first;  }
      /// Const Access the occupancy for the HPD
      inline double avOcc() const                  { return this->second; }
    public:
      /// Overload output to ostream
      friend inline std::ostream& operator << ( std::ostream& os, const HPDData & data )
      {
        return os << "[ fillCount=" << data.fillCount() << " avOcc=" << data.avOcc() << " ]"; 
      }
    };

    /// Returns occupancy data object for given HPD identifier
    HPDData & hpdData( const LHCb::RichSmartID hpdID ) const;

  private: // private data

    /// Raw Buffer Decoding tool
    const Rich::DAQ::IRawBufferToSmartIDsTool * m_SmartIDDecoder = nullptr;

    /// Occupancy map
    typedef Rich::HashMap< const LHCb::RichSmartID, HPDData > OccMap;
    typedef Rich::Map< Rich::DetectorType, OccMap >           RichOccMap;
    mutable RichOccMap m_occMap;

    /// Memory fraction
    double m_memory;

    /// Flag to turn on/off the printing of the XML conditions
    bool m_printXML;

    /// Flag to turn on/off reading of data from Conditions
    bool m_readFromCondDB;

    /// Location of occupancies in DB
    std::vector<std::string> m_condBDLocs;

    /// The RICH detector to work on (job option)
    std::string m_whichRICH;

    mutable HPDData * m_currentData;     ///< Pointer to the Data for the current HPD
    mutable LHCb::RichSmartID m_lastHPD; ///< The last HPD to to analysed

    /** @brief Flag to turn on the use of a running average
     *  If set true then a running average of each HPD's occupancy will be kept
     *  If false, then the average occupancies read from file will be used throughout
     */
    bool m_useRunAv;

    /// New event flag
    mutable bool m_updateRunningOccs = false;

  protected:

    // Min number fills before applying occupancy cut
    unsigned int m_minFills;

  };

}

#endif // RICHDAQ_RichHPDOccupancyTool_H
