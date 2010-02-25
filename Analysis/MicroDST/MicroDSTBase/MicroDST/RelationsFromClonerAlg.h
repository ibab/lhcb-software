// $Id: RelationsFromClonerAlg.h,v 1.1 2010-02-25 07:15:20 jpalac Exp $
#ifndef MICRODST_RELATIONSFROMCLONERALG_H 
#define MICRODST_RELATIONSFROMCLONERALG_H 1

// Include files
#include <boost/function.hpp>
#include <boost/bind.hpp>
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// From MicroDST
#include "MicroDST/MicroDSTAlgorithm.h"
#include "MicroDST/Defaults.h"
#include "MicroDST/RelTableFunctors.h"
// from Boost
#include <boost/type_traits/remove_pointer.hpp>
/** @class RelationsFromClonerAlg RelationsFromClonerAlg.h MicroDST/RelationsFromClonerAlg.h
 *
 *  Templated algorithm to clone a relations table between Froms and Tos. The Froms
 *  are assumed to be cloned already. The Tos are not cloned, so the resulting cloned table 
 *  points to the original Tos.
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2010-02-24
 */

namespace MicroDST 
{

  template <typename TABLE>
  class RelationsFromClonerAlg : public MicroDSTAlgorithm {

  private:

    typedef Location<TABLE> LOCATION;
    typedef MicroDST::TableCloner<TABLE> TableCloner;

  public:
    //===========================================================================
    /// Standard constructor
    RelationsFromClonerAlg( const std::string& name, ISvcLocator* pSvcLocator )
      :
      MicroDSTAlgorithm ( name , pSvcLocator ),
      m_tableCloner(boost::bind(&RelationsFromClonerAlg<TABLE>::cloneFrom, &(*this), _1),
                    boost::bind(&RelationsFromClonerAlg<TABLE>::cloneTo, &(*this), _1))               
    {
    }
    //===========================================================================
    ~RelationsFromClonerAlg( ) {} ///< Destructor
    //===========================================================================
    StatusCode initialize() 
    {
      StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first

      debug() << "==> Initialize" << endmsg;

      if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

      if ( inputTESLocation()=="" )  {
        verbose() << "Setting input TES location to default: " 
                  << LOCATION::Default << endmsg;
        setInputTESLocation(LOCATION::Default);
      }
      verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

      return sc;

    }
    //===========================================================================
    StatusCode execute() 
    {

      if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

      const std::string outputLocation = 
        this->outputTESLocation( this->inputTESLocation() );

      if ( msgLevel(MSG::VERBOSE) ) {
        verbose() << "Going to clone relations from " 
                  << inputTESLocation()
                  << " into " << outputLocation << endmsg;
      }
      
      setFilterPassed(true);

      if (exist<TABLE>(outputLocation) )
      {
        return this->Warning("Object "+ outputLocation + " already exists. Not cloning.", 
                             StatusCode::SUCCESS,10) ;
      }
          

      if (exist<TABLE>(inputTESLocation()) )
      {
        if ( msgLevel(MSG::VERBOSE) ) {
          verbose() << "Retrieving relations table from " 
                    << inputTESLocation() << endmsg;
        }
        const TABLE* table = get<TABLE>(inputTESLocation());
        if (table) {
          if ( msgLevel(MSG::VERBOSE) ) {
            verbose() << "found table with "<< table->relations().size() 
                      << " entries!" << endmsg;
          }
          TABLE* cloneTable = m_tableCloner(table);
          if ( msgLevel(MSG::VERBOSE) ) {
            verbose() << "Going to store relations table from " 
                      << inputTESLocation()
                      << " into " << outputLocation << endmsg;
            verbose() << "Number of relations in cloned table: "
                      << cloneTable->relations().size() << endmsg;
          }
          put( cloneTable, outputLocation );
          return StatusCode::SUCCESS;
        }
        return StatusCode::FAILURE;
      } else {
        if ( msgLevel(MSG::VERBOSE) ) {
          verbose() << "Found no table at " << inputTESLocation() 
                    << ". storing empty table"<< endmsg;
        }
        
        TABLE* cloneTable = new TABLE();
        put( cloneTable, outputLocation );
        return StatusCode::SUCCESS;
      }

      return StatusCode::SUCCESS;

    }
    //===========================================================================
    StatusCode finalize() 
    {
      debug() << "==> Finalize" << endmsg;
      return MicroDSTAlgorithm::finalize();
    }
    //===========================================================================

  private:

    typedef typename boost::remove_pointer<typename TABLE::From>::type _From;


  private:

    inline typename TABLE::From cloneFrom(const typename TABLE::From from) 
    {

      if (0==from) {
        error() << "FROM is NUL!!!!" << endmsg;
        return 0;
      }
      return getStoredClone< _From >(from);

    }

    inline typename TABLE::To cloneTo(const typename TABLE::To to) 
    {
      return to;
    }
  
    //===========================================================================
  
  private:

    TableCloner m_tableCloner;
    
  };



 
}

#endif // MICRODST_RELATIONSCLONERALG_H
