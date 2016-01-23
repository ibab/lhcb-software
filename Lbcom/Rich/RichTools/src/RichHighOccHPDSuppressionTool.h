
//-----------------------------------------------------------------------------
/** @file RichHighOccHPDSuppressionTool.h
 *
 *  Header file for tool : Rich::DAQ::HighOccHPDSuppressionTool
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
#include "RichHPDOccupancyTool.h"

// Interfaces
#include "RichKernel/IRichPixelSuppressionTool.h"

// numberings
#include "RichKernel/RichDAQDefinitions.h"

// RichDet
#include "RichDet/DeRichSystem.h"

namespace Rich
{
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

    class HighOccHPDSuppressionTool : public Rich::HPDOccupancyTool,
                                      virtual public IPixelSuppressionTool
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      HighOccHPDSuppressionTool( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

      // Initialization of the tool after creation
      StatusCode initialize();

    public: // methods (and doxygen comments) inherited from interface

      // Monitor the occupancy of the given HPD
      bool applyPixelSuppression( const LHCb::RichSmartID hpdID,
                                  LHCb::RichSmartID::Vector & smartIDs ) const;

    protected: // protected data

      /// Rich System detector element
      const DeRichSystem * m_richSys = nullptr;

    private: // private data

      // absolute max HPD occupancy
      unsigned int m_overallMax;

      /// Occupancy scale cut
      double m_scale;

    protected:

      bool m_sumPrint; ///< Print summary of suppressions each event ?

    };

  }
}

#endif // RICHDAQ_RichHighOccHPDSuppressionTool_H
