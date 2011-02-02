
//-----------------------------------------------------------------------------
/** @file ITTrackConfirmation.h
 *
 *  Header file for reconstruction tool : ITTrackConfirmation
 *
 *  CVS Log :-
 *  $Id: ITTrackConfirmation.h,v 1.1 2008-10-24 13:45:07 mneedham Exp $
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *  @date   22/10/2008
 */
//-----------------------------------------------------------------------------

#ifndef TRACKTOOLS_ITTrackConfirmation_H
#define TRACKTOOLS_ITTrackConfirmation_H


//-----------------------------------------------------------------------------

#include "GaudiAlg/GaudiHistoTool.h"
#include "TrackInterfaces/ITrackSelector.h"
#include <string>
#include "Event/Track.h"


class ITrackCaloMatch;

class ITTrackConfirmation : public GaudiHistoTool,
                            virtual public ITrackSelector{

public:

  /// constructer
  ITTrackConfirmation( const std::string& type,
                 const std::string& name,
                 const IInterface* parent );

  virtual ~ITTrackConfirmation();

  /// Tool initialization
  virtual StatusCode initialize();

  /** Returns if the given track is selected or not
   *
   *  @param aTrack Reference to the Track to test
   *
   *  @return boolean indicating if the track is selected or not
   *  @retval true  Track is selected
   *  @retval false Track is rejected
   */
  virtual bool accept ( const LHCb::Track& aTrack ) const;

private:

  bool selfConfirm(const LHCb::Track& aTrack) const;

  double m_maxChi2Cut;
  bool m_selfConfirm;  
  bool m_caloConfirm; 
  double m_minCaloEnergy;

  ITrackCaloMatch* m_trackCaloMatch;

   

};

#endif // TRACKTOOLS_ITTrackConfirmation_H
