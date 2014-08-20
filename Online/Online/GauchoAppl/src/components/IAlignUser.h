/*
 * IAlignUser.h
 *
 *  Created on: Aug 12, 2014
 *      Author: beat
 */
#ifndef IALIGNUSER_H_
#define IALIGNUSER_H_
#include "GaudiKernel/IAlgTool.h"
namespace LHCb
{
  class IAlignFcn : virtual public IAlgTool
  {
    public:
      DeclareInterfaceID(LHCb::IAlignFcn,1,0);
      virtual void init()=0;
      virtual double analyze()=0;
      virtual void ReadParams()=0;
      virtual void PubResult(long reference)=0;
      virtual void i_run()=0;
  };
  class IAlignIterator : virtual public IAlgTool
  {
    public:
      DeclareInterfaceID(LHCb::IAlignIterator,1,0);
      virtual StatusCode i_start()=0;
  };
}
#endif /* IALIGNUSER_H_ */
