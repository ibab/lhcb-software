// $Id: RichRingRecAlgBase.h,v 1.2 2009-06-05 17:21:33 jonrob Exp $
#ifndef RICHRINGRECALGBASE_H
#define RICHRINGRECALGBASE_H 1

// Include files
// from Rich
#include "RichRecBase/RichRecAlgBase.h"
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

      //-----------------------------------------------------------------------------
      //namespace Rich
      //{
      //  namespace Rec
      //  {
      //    namespace RingRec
      //    {

      /** @class RichRingRecAlgBase RichRingRecAlgBase.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-05-14
       */

      class RichRingRecAlgBase: public RichRecAlgBase
      {

      public:
        /// Standard constructor
        RichRingRecAlgBase( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~RichRingRecAlgBase( ); ///< Destructor
        virtual StatusCode initialize();    ///< Algorithm initialization
        virtual StatusCode execute   ();    ///< Algorithm execution
        virtual StatusCode finalize  ();    ///< Algorithm finalization

        IRichRingRecToolBase* rt()
        {  return  m_ringtool; }

      protected:

      private:

        IRichRingRecToolBase* m_ringtool;


      };

    }
  }
}

#endif // RICHRINGRECALGBASE_H
