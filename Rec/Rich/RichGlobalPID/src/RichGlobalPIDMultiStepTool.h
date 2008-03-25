
//--------------------------------------------------------------------------
/** @file RichGlobalPIDMultiStepTool.h
 *
 *  Header file for RICH Global PID tool : Rich::Rec::GlobalPID::MultiStepTool
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDMultiStepTool.h,v 1.1 2008-03-25 16:26:44 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2008-03-01
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RichGlobalPIDMultiStepTool_H
#define RICHGLOBALPID_RichGlobalPIDMultiStepTool_H 1

// STL
#include <vector>

// Base class
#include "RichGlobalPIDToolBase.h"
#include "RichRecBase/IRichPID.h"
#include "IRichGlobalPID.h"

// interfaces
#include "RichRecBase/IRichPixelBackgroundEsti.h"
#include "IRichGlobalPIDTrackCreator.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// boost
#include "boost/lexical_cast.hpp"

namespace Rich
{
  namespace Rec
  {
    namespace GlobalPID
    {

      /** @class MultiStepTool RichGlobalPIDMultiStepTool.h
       *
       *  Tool to create RichGlobalPIDTrack track objects for the RICH
       *  Global PID algorithm
       *
       *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
       *  @date   2008-03-01
       */
      class MultiStepTool : public Rich::Rec::GlobalPID::ToolBase,
                            virtual public Rich::Rec::IRichPID,
                            virtual public IRichGlobalPID
      {

      public:

        /// Standard constructor
        MultiStepTool( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );

        /// Destructor
        virtual ~MultiStepTool( );

        // Initialize method
        virtual StatusCode initialize();

      public: // IRichPID

        // Access the RichPID associated to the given Track
        virtual const LHCb::RichPID * pid( const LHCb::Track * track ) const;

        // Access the RichPID data objects for the given Tracks
        virtual void pids( const LHCb::Track::ConstVector & tracks,
                           LHCb::RichPID::ConstVector & richpids ) const;

      public: // IRichGlobalPID

        // Access the RichGlobalPID associated to the given Track
        virtual LHCb::RichGlobalPID * pid( LHCb::RichGlobalPIDTrack * track ) const;

        // Create the RichGlobalPID data objects for the given Tracks
        virtual void pids( const LHCb::RichGlobalPIDTrack::Vector & tracks ) const;

      private:  // Private data members
        
        /// RichGlobalPIDTrack creator
        const ITrackCreator * m_gtkCreator;   

        /// The number of PID steps (background estimation then Global PID likelihood)
        unsigned int m_nSteps;

        /// Background tools
        std::vector<const IPixelBackgroundEsti*> m_bckTools;

        /// Likelihood tools
        std::vector<const IRichGlobalPID*> m_pidTools;

      };

    }
  }
}

#endif // RICHGLOBALPID_RichGlobalPIDMultiStepTool_H
