
//-----------------------------------------------------------------------------
/** @file RichRawBufferToSmartIDsTool.h
 *
 *  Header file for tool : Rich::DAQ::RawBufferToSmartIDsTool
 *
 *  CVS Log :-
 *  $Id: RichRawBufferToSmartIDsTool.h,v 1.15 2007-04-23 12:58:44 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHRAWBUFFERTOSMARTIDSTOOL_H
#define RICHDAQ_RICHRAWBUFFERTOSMARTIDSTOOL_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h" // due to forward declaration in GaudiTool

// base class
#include "RichKernel/RichToolBase.h"

// Rich::DAQ utility classes
#include "RichHPDDataBank.h"

// Interfaces
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"
#include "RichKernel/IRichRawDataFormatTool.h"

namespace Rich
{
  namespace DAQ
  {

    //-----------------------------------------------------------------------------
    /** @class RawBufferToSmartIDsTool RichRawBufferToSmartIDsTool.h
     *
     *  Tool to create RichSmartIDs from the raw buffer.
     *  Uses the raw decoding tool and performs verious post processing tasks.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     *
     *  @todo Review if this tool is really needed
     */
    //-----------------------------------------------------------------------------

    class RawBufferToSmartIDsTool : public ToolBase,
                                    virtual public IRawBufferToSmartIDsTool,
                                    virtual public IIncidentListener
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      RawBufferToSmartIDsTool( const std::string& type,
                               const std::string& name,
                               const IInterface* parent );

      // Initialization of the tool after creation
      StatusCode initialize();

      /** Implement the handle method for the Incident service.
       *  This is used to inform the tool of software incidents.
       *
       *  @param incident The incident identifier
       */
      void handle( const Incident& incident );

    public: // methods (and doxygen comments) inherited from interface

      // Access all RichSmartIDs for the current Event
      const Rich::DAQ::L1Map & allRichSmartIDs() const;

    private: // private methods

      /// Initialise for a new event
      void InitNewEvent();

      /// Fill the RichSmartID container
      void fillRichSmartIDs() const;

    private: // private data

      /// Pointer to RICH raw data format tool
      const IRawDataFormatTool * m_rawFormatT;

      /// The decoded data
      mutable Rich::DAQ::L1Map m_richData;

      /// New event flag
      mutable bool m_newEvent;

    };

    inline void RawBufferToSmartIDsTool::InitNewEvent()
    {
      m_newEvent = true;
    }

  }
}

#endif // RICHDAQ_RICHRAWBUFFERTOSMARTIDSTOOL_H
