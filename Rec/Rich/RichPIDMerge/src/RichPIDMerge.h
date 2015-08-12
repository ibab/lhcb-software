
/** @file RichPIDMerge.h
 *
 *  Header file for RICH algorithm : Rich::Rec::PIDMerge
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */

#ifndef RICHPIDMERGE_PIDMerge_H
#define RICHPIDMERGE_PIDMerge_H 1

// STL
#include <unordered_set>

// base class
#include "RichKernel/RichAlgBase.h"

// event model
#include "Event/ProcStatus.h"
#include "Event/RichPID.h"

namespace Rich
{
  namespace Rec
  {

    /** @class PIDMerge RichPIDMerge.h
     *
     *  Merges multiple PID locations into one.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2002-07-10
     */

    class PIDMerge : public Rich::AlgBase
    {

    public:

      /// Standard constructor
      PIDMerge( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~PIDMerge( ); ///< Destructor

      virtual StatusCode initialize();   // Algorithm initialization
      virtual StatusCode execute   ();   // Algorithm execution

    private:

      /// Output location 
      std::string m_output;

      /// Input RichPID locations
      std::vector<std::string> m_inputs;

    };

  }
}

#endif // RICHPIDMERGE_PIDMerge_H
