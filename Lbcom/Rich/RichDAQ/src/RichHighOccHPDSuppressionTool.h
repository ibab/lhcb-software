
//-----------------------------------------------------------------------------
/** @file RichHighOccHPDSuppressionTool.h
 *
 *  Header file for tool : Rich::DAQ::HighOccHPDSuppressionTool
 *
 *  CVS Log :-
 *  $Id: RichHighOccHPDSuppressionTool.h,v 1.10 2007-02-01 17:42:29 jonrob Exp $
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

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace DAQ
   *
   *  namespace for RICH DAQ software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace DAQ
  {

    //-----------------------------------------------------------------------------
    /** @class HighOccHPDSuppressionTool RichHighOccHPDSuppressionTool.h
     *
     *  Tool for monitoring high occupancy HPDs
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   21/03/2006
     */
    //-----------------------------------------------------------------------------

    class HighOccHPDSuppressionTool : public RichToolBase,
                                      virtual public IPixelSuppressionTool
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      HighOccHPDSuppressionTool( const std::string& type,
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

      // Returns the average occupancy for the given HPD
      double averageOccupancy( const LHCb::RichSmartID hpdID ) const;

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

      /// Find HPD data for given HPD RichSmartID
      void findHpdData( const LHCb::RichSmartID hpdID ) const;

    protected: // methods

      /// Which Rich ?
      inline Rich::DetectorType rich() const { return m_rich; }

    protected: // protected data

      /// Rich System detector element
      const DeRichSystem * m_richSys;

    protected: // utility classes

      //-----------------------------------------------------------------------------
      /** @class HPDData RichHighOccHPDSuppressionTool.h
       *
       *  Utility class to hold occupancy data for a single HPD
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

      /// Returns occupancy data object for given HPD identifier
      HPDData & hpdData( const LHCb::RichSmartID hpdID ) const;

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

      /// The RICH detector to work on (job option)
      std::string m_whichRICH;
      /// The RICH detector to work on
      Rich::DetectorType m_rich;

      mutable HPDData * m_currentData;     ///< Pointer to the Data for the current HPD
      mutable LHCb::RichSmartID m_lastHPD; ///< The last HPD to to analysed

      /** @brief Flag to turn on the use of a running average
       *  If set true then a running average of each HPD's occupancy will be kep
       *  If false, then the average occupancies read from file will be used throughout
       */
      bool m_useRunAv;

    protected:

      bool m_sumPrint; ///< Print summary of suppressions each event ?

    };

    inline HighOccHPDSuppressionTool::HPDData &
    HighOccHPDSuppressionTool::hpdData( const LHCb::RichSmartID hpdID ) const
    {
      if ( m_lastHPD != hpdID ) { findHpdData(hpdID); }
      return *m_currentData;
    }

  }
}

#endif // RICHDAQ_RichHighOccHPDSuppressionTool_H
