// $Id: RichRingRecTupleAlgBase.h,v 1.2 2009-06-05 17:21:33 jonrob Exp $
#ifndef RICHRINGREC_RICHRINGRECTUPLEALGBASE_H
#define RICHRINGREC_RICHRINGRECTUPLEALGBASE_H 1

// Include files
// from Gaudi
#include "RichRecBase/RichRecTupleAlgBase.h"
//Interfaces
#include "RichRingRec/IRichRingRecToolBase.h"
// local
#include "RichRingRec/RichDataArrayDefinitions.h"

using namespace LHCb;

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

      /** @class RichRingRecTupleAlgBase RichRingRecTupleAlgBase.h RichRingRec/RichRingRecTupleAlgBase.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-06-29
       */
      class RichRingRecTupleAlgBase : public RichRecTupleAlgBase {
      public:
        /// Standard constructor
        RichRingRecTupleAlgBase( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~RichRingRecTupleAlgBase( ); ///< Destructor

        virtual StatusCode initialize();    ///< Algorithm initialization
        virtual StatusCode execute   ();    ///< Algorithm execution
        virtual StatusCode finalize  ();    ///< Algorithm finalization
        IRichRingRecToolBase* rt(){  return  m_ringtool; }

      protected:

      private:
        IRichRingRecToolBase* m_ringtool;

      };

    }
  }
}

#endif // RICHRINGREC_RICHRINGRECTUPLEALGBASE_H
