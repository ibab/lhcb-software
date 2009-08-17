// $Id: ObjectClonerAlg.h,v 1.4 2009-08-17 19:13:43 jpalac Exp $
#ifndef MICRODST_MICRODSTOBJECTCLONERALG_H 
#define MICRODST_MICRODSTOBJECTCLONERALG_H 1

// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// From MicroDST
#include "MicroDST/MicroDSTAlgorithm.h"
#include "MicroDST/BindType2ClonerDef.h"
#include "MicroDST/Defaults.h"

/** @class ObjectClonerAlg ObjectClonerAlg.h MicroDST/ObjectClonerAlg.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-08-27
 */
namespace MicroDST 
{
  
  template <typename T>
  class ObjectClonerAlg : public MicroDSTAlgorithm {
  private:
    typedef Location<T> LOCATION;
    typedef typename BindType2Cloner<T>::cloner CLONER;
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
        verbose() << "Setting input TES location to default: " 
                  << LOCATION::Default << endmsg;
        setInputTESLocation(LOCATION::Default);
      }
      verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

      return StatusCode::SUCCESS;
    }
    
    StatusCode execute() 
    {
      debug() << "==> Execute" << endmsg;
      typedef std::vector<std::string>::const_iterator stringIter;
      stringIter locBegin = this->inputTESLocations().begin();
      stringIter locEnd = this->inputTESLocations().end();
    
      for (stringIter iLoc = locBegin; iLoc != locEnd; ++iLoc) {
        const std::string inputLocation = MicroDST::niceLocationName(*iLoc);
        const std::string outputLocation = 
          this->outputTESLocation( inputLocation );
        verbose() << "Going to clone KeyedContainer from " << inputLocation
                  << " into " << outputLocation << endmsg;

        const T* cont = 
          copyAndStoreObject<T, CLONER>(inputLocation,
                                        outputLocation      );

        if (0==cont) {
          Warning("Unable clone or get object from "+ inputLocation,
                  StatusCode::FAILURE, 10).ignore();
        }
      }

      setFilterPassed(true);
      return StatusCode::SUCCESS;
      
    }
    
    StatusCode finalize  () 
    {
      debug() << "==> Finalize" << endmsg;
      return MicroDSTAlgorithm::finalize();
    }
    
  };

}

#endif // MICRODST_MICRODSTOBJECTCLONERALG_H
