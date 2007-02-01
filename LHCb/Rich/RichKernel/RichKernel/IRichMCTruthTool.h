
//------------------------------------------------------------------------------------
/** @file IRichMCTruthTool.h
 *
 *  Header file for tool interface : Rich::IMCTruthTool
 *
 *  CVS Log :-
 *  $Id: IRichMCTruthTool.h,v 1.22 2007-02-01 17:24:54 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */
//------------------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICHMCTRUTHTOOL_H
#define RICHKERNEL_IRICHMCTRUTHTOOL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Kernel
#include "Kernel/RichParticleIDType.h"
#include "Kernel/RichRadiatorType.h"

// Event Model
#include "Event/MCRichHit.h"
namespace LHCb
{
  class MCRichOpticalPhoton;
  class MCRichSegment;
  class MCRichTrack;
  class MCRichDigit;
  class MCParticle;
  class MCRichHit;
  class MCRichDigitSummary;
}

/// Static Interface Identification
static const InterfaceID IID_IRichMCTruthTool( "Rich::IMCTruthTool", 1, 0 );

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
  /** @namespace MC
   *
   *  General namespace for RICH MC related software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   05/12/2006
   */
  //-----------------------------------------------------------------------------
  namespace MC
  {

    //------------------------------------------------------------------------------------
    /** @class IMCTruthTool IRichMCTruthTool.h RichKernel/IRichMCTruthTool.h
     *
     *  Interface for tool performing MC truth associations between Rich event objects
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //------------------------------------------------------------------------------------

    class IMCTruthTool : public virtual IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichMCTruthTool; }

      /** get the MCRichHits associated to a given MCParticle
       *
       *  @param mcp Pointer to an MCParticle
       *
       *  @return Vector of associated MCRichHits
       */
      virtual const SmartRefVector<LHCb::MCRichHit> &
      mcRichHits( const LHCb::MCParticle * mcp ) const = 0;

      /** Get the MCRichHits associated to a given RichSmartID
       *
       *  @param smartID RichSmartID
       *
       *  @return Vector of associated MCRichHits
       */
      virtual const SmartRefVector<LHCb::MCRichHit> &
      mcRichHits( const LHCb::RichSmartID smartID ) const = 0;

      /** Get a vector of MCParticles associated to given RichSmartID
       *
       *  @param id RichSmartID identifying the RICH readout channel
       *
       *  @return boolean indicating if any associated MCParticles where found
       */
      virtual bool mcParticles( const LHCb::RichSmartID id,
                                std::vector<const LHCb::MCParticle*> & mcParts ) const = 0;

      /** Determines the particle mass hypothesis for a given MCParticle
       *
       *  @param mcPart Pointer to an MCParticle
       *
       *  @return The true particle type of the MCParticle
       */
      virtual Rich::ParticleIDType
      mcParticleType ( const LHCb::MCParticle * mcPart ) const = 0;

      /** Finds the MCRichDigit association for a RichSmartID channel identifier
       *
       *  @param id RichSmartID
       *
       *  @return Pointer to an MCRichDigit
       *  @retval NULL  No Monte Carlo association was possible
       *  @retval !NULL Association was successful
       */
      virtual const LHCb::MCRichDigit *
      mcRichDigit ( const LHCb::RichSmartID id ) const = 0;

      /** Finds the MCRichTrack associated to a given MCParticle
       *
       *  @param mcPart Pointer to an MCParticle
       *
       *  @return Pointer to an MCRichTrack
       *  @retval NULL  No Monte Carlo association was possible
       *  @retval !NULL Association was successful
       */
      virtual const LHCb::MCRichTrack *
      mcRichTrack ( const LHCb::MCParticle * mcPart ) const = 0;

      /** Finds the MCRichOpticalPhoton associated to a given MCRichHit
       *
       *  @param mcHit Pointer to an MCRichHit
       *
       *  @return Pointer to an MCRichOpticalPhoton
       *  @retval NULL  No Monte Carlo association was possible
       *  @retval !NULL Association was successful
       */
      virtual const LHCb::MCRichOpticalPhoton *
      mcOpticalPhoton ( const LHCb::MCRichHit * mcHit ) const = 0;

      /** Access the bit-pack history objects for the given RichSmartID
       *
       *  @param id        RichSmartID
       *  @param histories Vector of pointers to history objects
       *
       *  @return Boolean indicating if at least one history object was found
       *  @retval true  History objects were found
       *  @retval false No history objects were found
       */
      virtual bool
      getMcHistories( const LHCb::RichSmartID id,
                      std::vector<const LHCb::MCRichDigitSummary*> & histories ) const = 0;

      /** Checks if the given RichSmartID is the result of a background
       *  hit, i.e. not a true Cherenkov hit
       *
       *  @param id RichSmartID
       *
       *  @return Boolean indicating if the digit is background
       *  @retval true  RichSmartID originated from a background hit
       *  @retval false RichSmartID originated from Cherenkov Radiation
       */
      virtual bool isBackground ( const LHCb::RichSmartID id ) const = 0;

      /** Checks if the given RichSmartID is the result of true Cherenkov
       *  radiation from the given radiator
       *
       *  @param id  RichSmartID identifying channel
       *  @param rad Radiator medium
       *
       *  @return Boolean indicating if the hit is Cherenkov radiation
       *          in given radiator
       *  @retval true  RichSmartID did originate from Cherenkov radiation
       *  @retval false RichSmartID did not originate from Cherenkov radiation
       */
      virtual bool isCherenkovRadiation( const LHCb::RichSmartID id,
                                         const Rich::RadiatorType rad ) const = 0;

      /** Checks if MC summary information for the RICH hits (RichSmartIDs) are available
       *
       *  @retval true  MC summary inforamtion is available
       *  @retval false MC summary inforamtion is NOT available
       */
      virtual bool richMCHistoryAvailable() const = 0;

      /** Checks if RICH extended MC information (MCRichOpticalPhoton, MCRichSegment etc.)
       *  is available.
       *
       *  @retval true  Extended MC inforamtion is available
       *  @retval false Extended MC inforamtion is NOT available
       */
      virtual bool extendedMCAvailable() const = 0;

    };

  }
}

#endif // RICHKERNEL_IRICHMCTRUTHTOOL_H
