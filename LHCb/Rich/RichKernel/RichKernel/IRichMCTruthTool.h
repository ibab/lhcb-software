
//------------------------------------------------------------------------------------
/** @file IRichMCTruthTool.h
 *
 *  Header file for tool interface : IRichMCTruthTool
 *
 *  CVS Log :-
 *  $Id: IRichMCTruthTool.h,v 1.10 2005-03-03 15:44:21 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */
//------------------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICHMCTRUTHTOOL_H
#define RICHKERNEL_IRICHMCTRUTHTOOL_H 1

// RichKernel
#include "RichKernel/RichParticleIDType.h"

// Event Model
class MCRichOpticalPhoton;
class MCRichSegment;
class MCRichTrack;
class MCRichDigit;
class MCParticle;
class MCRichHit;
class RichDigit;
class TrStoredTrack;
class TrgTrack;

/// Static Interface Identification
static const InterfaceID IID_IRichMCTruthTool( "IRichMCTruthTool", 1, 0 );

//------------------------------------------------------------------------------------
/** @class IRichMCTruthTool IRichMCTruthTool.h RichKernel/IRichMCTruthTool.h
 *
 *  Interface for tool performing MC truth associations between Rich event objects
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//------------------------------------------------------------------------------------

class IRichMCTruthTool : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichMCTruthTool; }

  /** Find best MCParticle association for a given TrStoredTrack
   *
   *  @param track Pointer to a TrStoredTrack
   *
   *  @return Pointer to MCParticle
   *  @retval NULL  No Monte Carlo association was possible
   *  @retval !NULL Association was successful
   */
  virtual const MCParticle *
  mcParticle ( const TrStoredTrack * track ) const = 0;

  /** Find best MCParticle association for a given TrgTrack
   *
   *  @param track Pointer to a TrgTrack
   *
   *  @return Pointer to MCParticle
   *  @retval NULL  No Monte Carlo association was possible
   *  @retval !NULL Association was successful
   */
  virtual const MCParticle *
  mcParticle ( const TrgTrack * track ) const = 0;

  /** Determines the particle mass hypothesis for a given TrtStoredTrack
   *
   *  @param track Pointer to a TrStoredTrack
   *
   *  @return The true particle type of the TrStoredTrack
   */
  virtual Rich::ParticleIDType
  mcParticleType ( const TrStoredTrack * track ) const = 0;

  /** Determines the particle mass hypothesis for a given TrgTrack
   *
   *  @param track Pointer to a TrgTrack
   *
   *  @return The true particle type of the TrStoredTrack
   */
  virtual Rich::ParticleIDType
  mcParticleType ( const TrgTrack * track ) const = 0;

  /** Determines the particle mass hypothesis for a given MCParticle
   *
   *  @param mcPart Pointer to an MCParticle
   *
   *  @return The true particle type of the MCParticle
   */
  virtual Rich::ParticleIDType
  mcParticleType ( const MCParticle * mcPart ) const = 0;

  /** Finds the MCRichDigit association for a given RichDigit
   *
   *  @param digit Pointer to a RichDigit
   *
   *  @return Pointer to an MCRichDigit
   *  @retval NULL  No Monte Carlo association was possible
   *  @retval !NULL Association was successful
   */
  virtual const MCRichDigit *
  mcRichDigit ( const RichDigit * digit ) const = 0;

  /** Finds the MCRichDigit association for a RichSmartID channel identifier
   *
   *  @param id RichSmartID
   *
   *  @return Pointer to an MCRichDigit
   *  @retval NULL  No Monte Carlo association was possible
   *  @retval !NULL Association was successful
   */
  virtual const MCRichDigit *
  mcRichDigit ( const RichSmartID id ) const = 0;

  /** Finds the MCRichTrack associated to a given TrStoredTrack
   *
   *  @param track Pointer to a TrStoredTrack
   *
   *  @return Pointer to an MCRichTrack
   *  @retval NULL  No Monte Carlo association was possible
   *  @retval !NULL Association was successful
   */
  virtual const MCRichTrack *
  mcRichTrack ( const TrStoredTrack * track ) const = 0;

  /** Finds the MCRichTrack associated to a given TrgTrack
   *
   *  @param track Pointer to a TrgTrack
   *
   *  @return Pointer to an MCRichTrack
   *  @retval NULL  No Monte Carlo association was possible
   *  @retval !NULL Association was successful
   */
  virtual const MCRichTrack *
  mcRichTrack ( const TrgTrack * track ) const = 0;

  /** Finds the MCRichTrack associated to a given MCParticle
   *
   *  @param mcPart Pointer to an MCParticle
   *
   *  @return Pointer to an MCRichTrack
   *  @retval NULL  No Monte Carlo association was possible
   *  @retval !NULL Association was successful
   */
  virtual const MCRichTrack *
  mcRichTrack ( const MCParticle * mcPart ) const = 0;

  /** Finds the MCRichOpticalPhoton associated to a given MCRichHit
   *
   *  @param mcHit Pointer to an MCRichHit
   *
   *  @return Pointer to an MCRichOpticalPhoton
   *  @retval NULL  No Monte Carlo association was possible
   *  @retval !NULL Association was successful
   */
  virtual const MCRichOpticalPhoton *
  mcOpticalPhoton ( const MCRichHit * mcHit ) const = 0;

  /** Checks if the given MCRichDigit is the result of a background
   *  hit, i.e. not a true Cherenkov hit
   *
   *  @param digit Pointer to an MCRichDigit
   *
   *  @return Boolean indicating if the digit is background
   *  @retval true  MCRichDigit originated from a background hit
   *  @retval false MCRichDigit originated from Cherenkov Radiation
   */
  virtual bool isBackground ( const MCRichDigit * digit ) const = 0;

  /** Checks if the given MCRichHit is the result of a background
   *  hit, i.e. not a true Cherenkov hit
   *
   *  @param hit Pointer to an MCRichHit
   *
   *  @return Boolean indicating if the hit is background
   *  @retval true  MCRichHit originated from a background hit
   *  @retval false MCRichHit originated from Cherenkov Radiation
   */
  virtual bool isBackground ( const MCRichHit * hit ) const = 0;

};

#endif // RICHKERNEL_IRICHMCTRUTHTOOL_H
