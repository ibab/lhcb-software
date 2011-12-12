// $Id: ObjectClonerAlg.h,v 1.8 2010-09-07 10:25:55 jpalac Exp $
#ifndef MICRODST_MICRODSTOBJECTCLONERALG_H
#define MICRODST_MICRODSTOBJECTCLONERALG_H 1

// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// From MicroDST
#include "MicroDST/MicroDSTAlgorithm.h"
#include "MicroDST/BindType2ClonerDef.h"
#include "MicroDST/Defaults.h"


namespace MicroDST
{

  /** @class ObjectClonerAlg ObjectClonerAlg.h MicroDST/ObjectClonerAlg.h
   *
   *
   *  @author Juan PALACIOS
   *  @date   2008-08-27
   */

  template <typename T>
  class ObjectClonerAlg : public MicroDSTAlgorithm 
  {
  
  private:
  
    typedef Location<T> LOCATION;
    typedef typename BindType2Cloner<T>::Cloner CLONER;
  
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
      if ( sc.isFailure() ) return sc;

      if ( inputTESLocation().empty() )
      {
        setInputTESLocation(LOCATION::Default);
      }
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

      return sc;
    }

    StatusCode execute()
    {
      for ( std::vector<std::string>::const_iterator iLoc = this->inputTESLocations().begin();
            iLoc != this->inputTESLocations().end(); ++iLoc )
      {
        const std::string inputLocation  = niceLocationName(*iLoc);
        const std::string outputLocation =
          this->outputTESLocation( inputLocation );

        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "Going to clone KeyedContainer from " << inputLocation
                    << " into " << outputLocation << endmsg;

        const T* cont =
          copyAndStoreObject<CLONER>( inputLocation, outputLocation );

        if ( !cont )
        {
          Warning("Unable to clone or get object from "+ inputLocation,
                  StatusCode::FAILURE, 0).ignore();
        }
      }

      setFilterPassed(true);

      return StatusCode::SUCCESS;
    }

  };

}

#endif // MICRODST_MICRODSTOBJECTCLONERALG_H
