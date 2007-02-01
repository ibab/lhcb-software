
//-----------------------------------------------------------------------------
/** @file RichRawBufferToSmartIDsTool.h
 *
 *  Header file for tool : Rich::DAQ::RawBufferToSmartIDsTool
 *
 *  CVS Log :-
 *  $Id: RichRawBufferToSmartIDsTool.h,v 1.13 2007-02-01 17:42:29 jonrob Exp $
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

      // Finalization of the tool before deletion
      StatusCode finalize();

      /** Implement the handle method for the Incident service.
       *  This is used to inform the tool of software incidents.
       *
       *  @param incident The incident identifier
       */
      void handle( const Incident& incident );

    public: // methods (and doxygen comments) inherited from interface

      // Access all RichSmartIDs for the current Event
      const Rich::DAQ::PDMap & allRichSmartIDs() const;

    private: // private methods

      /// Initialise for a new event
      void InitNewEvent();

      /// Fill the RichSmartID container
      void fillRichSmartIDs() const;

    private: // private data

      /// Pointer to RICH raw data format tool
      const IRawDataFormatTool * m_rawFormatT;

      /// Flag to turn on the sorting of the RichSmartIDs
      //bool m_sortIDs;

      /// RichSmartIDs
      mutable Rich::DAQ::PDMap m_smartIDs;

      /// New event flag
      mutable bool m_newEvent;

    };

    inline void RawBufferToSmartIDsTool::InitNewEvent()
    {
      m_newEvent = true;
      m_smartIDs.clear();
    }

  }
}

#endif // RICHDAQ_RICHRAWBUFFERTOSMARTIDSTOOL_H
