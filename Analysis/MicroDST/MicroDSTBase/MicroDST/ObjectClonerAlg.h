// $Id: ObjectClonerAlg.h,v 1.2 2008-09-01 09:04:06 jpalac Exp $
#ifndef MICRODST_MICRODSTOBJECTCLONERALG_H 
#define MICRODST_MICRODSTOBJECTCLONERALG_H 1

// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// From MicroDST
#include "MicroDST/MicroDSTAlgorithm.h"


/** @class ObjectClonerAlg ObjectClonerAlg.h MicroDST/ObjectClonerAlg.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-08-27
 */
namespace MicroDST 
{
  
  template <typename T, typename CLONER = MicroDST::BasicCopy<T> >
  class ObjectClonerAlg : public MicroDSTAlgorithm {
  public: 
    /// Standard constructor
    ObjectClonerAlg( const std::string& name, ISvcLocator* pSvcLocator )
      : MicroDSTAlgorithm ( name , pSvcLocator ) 
    {
    }

    ~ObjectClonerAlg( ) {} ///< Destructor

    StatusCode initialize() 
    {
      StatusCode sc = MicroDSTAlgorithm::initialize();

      debug() << "==> Initialize" << endmsg;

      if ( sc.isFailure() ) return sc;

      if (inputTESLocation()=="")  {
        error() << "No input TES location set" 
                << endmsg;
        return StatusCode::FAILURE;
      }
      verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

      return StatusCode::SUCCESS;
    }
    
    StatusCode execute() 
    {
        debug() << "==> Execute" << endmsg;

        setFilterPassed(true);
  
        verbose() << "Going to object from " << inputTESLocation()
                  << " into " << fullOutputTESLocation() << endmsg;
        return (0!=copyAndStoreObject<T, CLONER >(inputTESLocation(),
                                                  fullOutputTESLocation() ) ) ?
          StatusCode::SUCCESS : StatusCode::FAILURE;
    }
    
    StatusCode finalize  () 
    {
      debug() << "==> Finalize" << endmsg;
      return MicroDSTAlgorithm::finalize();
    }
    
  };

}

#endif // MICRODST_MICRODSTOBJECTCLONERALG_H
