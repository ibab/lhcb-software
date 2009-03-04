// $Id: RichRingRecAlgBase.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
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

using namespace Rich::Rec;


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
//    }
    
//  }
  
//}

#endif // RICHRINGRECALGBASE_H
