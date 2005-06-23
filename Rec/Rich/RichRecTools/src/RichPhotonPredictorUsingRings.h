
//----------------------------------------------------------------------------------
/** @file RichPhotonPredictorUsingRings.h
 *
 *  Header file for tool : RichPhotonPredictorUsingRings
 *
 *  CVS Log :-
 *  $Id: RichPhotonPredictorUsingRings.h,v 1.5 2005-06-23 15:17:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//----------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHPHOTONPREDICTORUSINGRINGS_H
#define RICHRECTOOLS_RICHPHOTONPREDICTORUSINGRINGS_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event
#include "Event/RichRecRing.h"
#include "Event/RichRecPixel.h"
#include "Event/RichRecSegment.h"

// Interfaces
#include "RichRecBase/IRichPhotonPredictor.h"
#include "RichRecBase/IRichRecGeomTool.h"

//----------------------------------------------------------------------------------
/** @class RichPhotonPredictorUsingRings RichPhotonPredictorUsingRings.h
 *
 *  Tool which performs the association between RichRecSegments and
 *  RichRecPixels to form RichRecPhotons.  This particular instance uses
 *  RichRecRing objects (which must already exist in the TES) to only select
 *  pixel/segment combinations that form a valid Ring candidate.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//----------------------------------------------------------------------------------

class RichPhotonPredictorUsingRings : public RichRecToolBase,
                                      virtual public IRichPhotonPredictor,
                                      virtual public IIncidentListener 
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichPhotonPredictorUsingRings( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

  /// Destructor
  virtual ~RichPhotonPredictorUsingRings(){}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Is it possible to make a photon candidate using this segment and pixel.
  bool photonPossible( RichRecSegment * segment,
                       RichRecPixel * pixel ) const;

  // Implement the handle method for the Incident service.
  void handle( const Incident& incident );

private: // methods

  /// Initialise for a new event
  void InitNewEvent();

  /// Retuns pointer RichRecRings
  RichRecRings * richRings() const;

private: // private data

  /// Geometry tool
  const IRichRecGeomTool * m_geomTool;

  std::vector<double> m_minROI;  ///< Min hit radius of interest around track centres
  std::vector<double> m_maxROI;  ///< Max hit radius of interest around track centres
  std::vector<double> m_maxROI2; ///< Square of m_maxROI
  std::vector<double> m_minROI2; ///< Square of m_minROI

  /// Pointer to RichRecRings
  mutable RichRecRings * m_recRings;

  /// location of rings in TES
  std::string m_ringLoc;

};


inline void RichPhotonPredictorUsingRings::InitNewEvent()
{
  // Initialise navigation data
  m_recRings = 0;
}

inline RichRecRings *
RichPhotonPredictorUsingRings::richRings() const
{
  if ( !m_recRings ) {
    m_recRings = get<RichRecRings>( m_ringLoc );
    debug() << "Located " << m_recRings->size() 
            << " RichRecRings at " << m_ringLoc << endreq;
  }
  return m_recRings;
}

#endif // RICHRECTOOLS_RICHPHOTONPREDICTORUSINGRINGS_H

