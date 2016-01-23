
//-----------------------------------------------------------------------------
/** @file RichHPDHotPixelFinder.h
 *
 *  Header file for tool : Rich::HPDHotPixelFinder
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RichHPDHotPixelFinder_H
#define RICHDAQ_RichHPDHotPixelFinder_H 1

// STD
#include <sstream>

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// base class
#include "RichKernel/RichToolBase.h"

// Interfaces
#include "RichKernel/IRichGenericHPDAnalysisTool.h"

// RichKernel
#include "RichKernel/RichMap.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class HPDHotPixelFinder RichHPDHotPixelFinder.h
   *
   *  Tool for monitoring high occupancy HPDs
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   21/03/2006
   */
  //-----------------------------------------------------------------------------

  class HPDHotPixelFinder : public Rich::ToolBase,
                            virtual public IGenericHPDAnalysisTool,
                            virtual public IIncidentListener
  {

  public: // Methods for Gaudi Framework

    /// Standard constructor
    HPDHotPixelFinder( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );

    // Initialization of the tool after creation
    StatusCode initialize();

  public:

    /** Implement the handle method for the Incident service.
     *  This is used to inform the tool of software incidents.
     *
     *  @param incident The incident identifier
     */
    void handle( const Incident& incident );

  public: // methods (and doxygen comments) inherited from interface

    // Applies HPD data analysis to given HPD data
    StatusCode analyse( const LHCb::RichSmartID hpdID,
                        const LHCb::RichSmartID::Vector & smartIDs,
                        IGenericHPDAnalysisTool::Results & results ) const;

  private:

    /// Run HPD hot pixel analysis on current data values
    void findHotPixels();

  private:

    /// Hit count map
    typedef Rich::Map<LHCb::RichSmartID,unsigned int> HitCountMap;
    
  private: // private data

    /// Count of number of events since last search for hot pixels
    unsigned long long m_nEvts{0};
    
    /// Number of events to look for hot pixels over
    unsigned long long m_nEventsForAverage;

    /// pixel event occupancy to consider a pixel as hot
    double m_hotOcc;

    /// Count hits per channel in current event sample
    mutable HitCountMap m_hitCount;

    typedef Rich::Map< LHCb::RichSmartID, IGenericHPDAnalysisTool::Results > ResultCache;
    /// Cached results
    mutable ResultCache m_results;

  };

}

#endif // RICHDAQ_RichHPDHotPixelFinder_H
