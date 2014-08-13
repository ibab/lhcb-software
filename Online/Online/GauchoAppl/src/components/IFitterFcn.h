/*
 * FitterFcn.h
 *
 *  Created on: Aug 12, 2014
 *      Author: beat
 */

#ifndef IFITTERFCN_H_
#define IFITTERFCN_H_
namespace LHCb
{
  class IFitterFcn
  {
    public:
//      virtual ~IFitterFcn()=0;
      virtual void init()=0;
      virtual double analyze()=0;
      virtual void ReadParams()=0;
      virtual void PubResult(long reference)=0;
      virtual void run()=0;
  };
}
#endif /* IFITTERFCN_H_ */
