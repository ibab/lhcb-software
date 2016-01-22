
//--------------------------------------------------------------------------
/** @file RichGlobalPIDLikelihood.h
 *
 *  Header file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::Likelihood
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RICHGLOBALPIDALG_H
#define RICHGLOBALPID_RICHGLOBALPIDALG_H 1

// base class
#include "RichGlobalPIDAlgBase.h"

// local
#include "IRichGlobalPID.h"

namespace Rich
{
  namespace Rec
  {
    namespace GlobalPID
    {

      //--------------------------------------------------------------------------
      /** @class Likelihood RichGlobalPIDLikelihood.h
       *
       *  Performs a single iteration of the global PID overall event
       *  likelihood minimisation.
       *
       *  Note : This algorithm works with -(Log-likelihood), and thus attempts to
       *         minimise the negative log likelihoods
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   12/12/2002
       *
       *  @todo Update algorithm to work with "normal" positive likelihoods
       */
      //--------------------------------------------------------------------------

      class Likelihood : public AlgBase
      {

      public:

        /// Standard constructor
        Likelihood( const std::string& name,
                    ISvcLocator* pSvcLocator );

        virtual ~Likelihood();        ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution
  
      private:  // Private data members

        const IRichGlobalPID * m_pidTool = nullptr; ///< Global PID tool

        /// PID tracks one at a time ?
        bool m_oneAtATime;

      };

    }
  }
}

#endif // RICHGLOBALPID_RICHGLOBALPIDALG_H
