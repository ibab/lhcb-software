
//-----------------------------------------------------------------------------
/** @file RichStatusCreator.h
 *
 *  Header file for tool : Rich::Rec::StatusCreator
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHSTATUSCREATOR_H
#define RICHRECTOOLS_RICHSTATUSCREATOR_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event
#include "Event/RichRecStatus.h"

// interface
#include "RichRecBase/IRichStatusCreator.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class StatusCreator RichStatusCreator.h
     *
     *  Tool for the creation and book-keeping of the RichRecStatus object.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class StatusCreator : public Rich::Rec::ToolBase,
                          virtual public IStatusCreator,
                          virtual public IIncidentListener
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      StatusCreator( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

      /// Destructor
      virtual ~StatusCreator(){}

      // Initialize method
      StatusCode initialize();

    public: // methods (and doxygen comments) inherited from public interface

      // Implement the handle method for the Incident service.
      // This is used to inform the tool of software incidents.
      void handle( const Incident& incident );

      // Returns a pointer to the RichRecStatus
      LHCb::RichRecStatus * richStatus() const;

    private: // methods

      /// Initialise for a new event
      void InitNewEvent();

    private: // data

      /// Pointer to RichRecStatus
      mutable LHCb::RichRecStatus * m_status = nullptr;

      /// Location of RichRecStatus in TES
      std::string m_richStatusLocation;

    };

    inline void StatusCreator::InitNewEvent()
    {
      m_status = nullptr;
    }

  }
}

#endif // RICHRECTOOLS_RICHSTATUSCREATOR_H
