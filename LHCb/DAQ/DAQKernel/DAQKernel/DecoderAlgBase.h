
//-----------------------------------------------------------------------------
/** @file DecoderAlgBase.h
 *
 *  Header file for algorithm base class : Decoder::AlgBase
 *
 *  @author Rob Lambert   Rob.Lambert@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef DAQKERNEL_DECODERALGBASE_H
#define DAQKERNEL_DECODERALGBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiAlgorithm.h"

// local
#include "DAQKernel/DecoderCommonBase.h"

namespace Decoder
{

  //-----------------------------------------------------------------------------
  /** @class AlgBase DAQAlgBase.h DAQKernel/DecoderAlgBase.h
   *
   *  Abstract base class for DAQ algorithms providing
   *  some basic functionality.
   *
   *  @author Rob Lambert   Rob.Lambert@cern.ch
   *  @date   02/10/2014
   */
  //-----------------------------------------------------------------------------

  class AlgBase : public Decoder::CommonBase<GaudiAlgorithm>
  {

  public:

    /// Standard constructor
    AlgBase( const std::string& name,
             ISvcLocator* pSvcLocator )
      : Decoder::CommonBase<GaudiAlgorithm> ( name, pSvcLocator ) { }

  protected:

  protected:
    ///avoid getIfExists name resolution for now
    virtual inline LHCb::RawEvent* getIfExistsRaw(const std::string loc, bool rootOnTes) const
    {
    	return GaudiAlgorithm::getIfExists<LHCb::RawEvent>(loc,rootOnTes);
    }
    /// override getIfExists to aid the name resolution?
    //template < class TYPE  > inline typename Gaudi::Utils::GetData<TYPE>::return_type getIfExists (
    //		IDataProviderSvc*  svc ,
    //		const std::string& location  ,
    //		const bool useRootInTES = true
    //		) const
    //{ return GaudiAlgorithm::getIfExists<TYPE> ( svc , location , useRootInTES ) ; }

    /// override getIfExists to aid the name resolution?
    //template < class TYPE  > inline typename Gaudi::Utils::GetData<TYPE>::return_type getIfExists (
    //		const std::string& location  ,
    //		const bool useRootInTES = true
    //		) const
    //{ return GaudiAlgorithm::getIfExists<TYPE> ( this->GaudiAlgorithm::evtSvc() , location , useRootInTES ) ; }
  };

}

#endif // DAQKERNEL_DECODERALGBASE_H
