// $Id: RichStatusCreator.h,v 1.4 2004-03-16 13:45:06 jonesc Exp $
#ifndef RICHRECTOOLS_RICHSTATUSCREATOR_H
#define RICHRECTOOLS_RICHSTATUSCREATOR_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interface
#include "RichRecBase/IRichStatusCreator.h"

// Event

/** @class RichStatusCreator RichStatusCreator.h
 *
 *  Tool for the creation and book-keeping of the RichRecStatus object.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichStatusCreator : public RichRecToolBase,
                          virtual public IRichStatusCreator,
                          virtual public IIncidentListener {

public:

  /// Standard constructor
  RichStatusCreator( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

  /// Destructor
  virtual ~RichStatusCreator(){}

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Implement the handle method for the Incident service.
  /// This is used to inform the tool of software incidents.
  void handle( const Incident& incident );

  /// Returns a pointer to the RichRecStatus
  RichRecStatus * richStatus() const;

private: // methods

  /// Initialise for a new event
  void InitNewEvent();

private: // data

  /// Pointer to RichRecStatus
  mutable RichRecStatus * m_status;

  /// Location of RichRecStatus in TES
  std::string m_richStatusLocation;

};

inline void RichStatusCreator::InitNewEvent()
{
  m_status = 0;
}

#endif // RICHRECTOOLS_RICHSTATUSCREATOR_H
