// $Id: RelationsFromClonerAlg.h,v 1.8 2010-09-07 10:28:53 jpalac Exp $
#ifndef MICRODST_RELATIONSFROMCLONERALG_H
#define MICRODST_RELATIONSFROMCLONERALG_H 1

// STL
#include <set>

// Include files
#include <boost/function.hpp>
#include <boost/bind.hpp>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/SmartIF.h"

// From MicroDST
#include "MicroDST/MicroDSTAlgorithm.h"
#include "MicroDST/Defaults.h"
#include "MicroDST/RelTableFunctors.h"

// From DaVinci
#include "DaVinciUtils/Guards.h"

// from Boost
#include <boost/type_traits/remove_pointer.hpp>

namespace MicroDST
{

  /** @class RelationsFromClonerAlg RelationsFromClonerAlg.h MicroDST/RelationsFromClonerAlg.h
   *
   *  Templated algorithm to clone a relations table between Froms and Tos. The Froms
   *  are assumed to be cloned already. The Tos are not cloned, so the resulting cloned table
   *  points to the original Tos.
   *
   *  @author Juan PALACIOS juan.palacios@nikhef.nl
   *  @date   2010-02-24
   */

  template <typename TABLE>
  class RelationsFromClonerAlg : public MicroDSTAlgorithm
  {

  private:

    typedef Location<TABLE> LOCATION;
    typedef MicroDST::TableCloner<TABLE> TableCloner;

  public:

    //===========================================================================
    /// Standard constructor
    RelationsFromClonerAlg( const std::string& name, ISvcLocator* pSvcLocator )
      : MicroDSTAlgorithm ( name, pSvcLocator ),
        m_tableCloner(boost::bind(&RelationsFromClonerAlg<TABLE>::cloneFrom, &(*this), _1),
                      boost::bind(&RelationsFromClonerAlg<TABLE>::cloneTo,   &(*this), _1) )
    {
    }

    //===========================================================================

    ~RelationsFromClonerAlg( ) {} ///< Destructor

    //===========================================================================
    StatusCode initialize()
    {
      StatusCode sc = MicroDSTAlgorithm::initialize();
      if ( sc.isFailure() ) return sc;

      if ( inputTESLocation().empty() )
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "Setting input TES location to default: "
                    << LOCATION::Default << endmsg;
        setInputTESLocation(LOCATION::Default);
      }
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

      return sc;
    }

    //===========================================================================
    StatusCode execute()
    {
      if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

      setFilterPassed(true);

      for ( std::vector<std::string>::const_iterator inputLoc = this->inputTESLocations().begin();
            inputLoc != this->inputTESLocations().end(); ++inputLoc)
      {
        copyTableFromLocation(*inputLoc);
      }
      return StatusCode::SUCCESS;
    }

    //=========================================================================
    void copyTableFromLocation(const std::string& inputLocation)
    {
      const std::string outputLocation =
        this->outputTESLocation( inputLocation );

      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "Going to clone relations from "
                  << inputLocation
                  << " into " << outputLocation << endmsg;
      }

      if (exist<TABLE>(outputLocation) )
      {
        this->Warning("Object "+ outputLocation + " already exists. Not cloning.",
                      StatusCode::SUCCESS,0).ignore() ;
        return;
      }

      if (exist<TABLE>(inputLocation) )
      {
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << "Retrieving relations table from "
                    << inputLocation << endmsg;
        }
        const TABLE* table = get<TABLE>(inputLocation);
        if ( table && !table->relations().empty() )
        {
          if ( msgLevel(MSG::VERBOSE) )
          {
            verbose() << "found table with "<< table->relations().size()
                      << " entries!" << endmsg;
          }
          TABLE* cloneTable = m_tableCloner(table);
          DaVinci::Utils::DataObjectGuard guard(cloneTable);
          if ( msgLevel(MSG::VERBOSE) )
          {
            verbose() << "Going to store relations table from "
                      << inputLocation
                      << " into " << outputLocation << endmsg;
            verbose() << "Number of relations in cloned table: "
                      << cloneTable->relations().size() << endmsg;
          }
          if ( !cloneTable->relations().empty() )
          {
            put( cloneTable, outputLocation );
          }
        }
        else
        {
          if ( msgLevel(MSG::VERBOSE) )
          {
            this->Warning("Found no table at "+inputLocation,
                          StatusCode::FAILURE, 0).ignore();
          }
        }

      }
      else
      {
        if ( msgLevel(MSG::VERBOSE) )
        {
          this->Warning("Found no table at "+inputLocation,
                        StatusCode::FAILURE, 0).ignore();
        }
      }

    }

    //===========================================================================

    void selectContainers ( DataObject* obj,
                            std::set<std::string>& names,
                            const unsigned int classID,
                            const bool forceRead = false )
    {
      SmartIF<IDataManagerSvc> mgr( eventSvc() );
      typedef std::vector<IRegistry*> Leaves;
      Leaves leaves;
      StatusCode sc = mgr->objectLeaves( obj, leaves );
      if ( sc )
      {
        for ( const auto& leaf : leaves )
        {
          const std::string& id = leaf->identifier();
          DataObject* tmp(NULL);
          if ( forceRead )
          {
            sc = this->eventSvc()->retrieveObject( id, tmp );
          }
          else
          {
            sc = this->eventSvc()->findObject( id, tmp );
          }
          if ( sc && NULL != tmp )
          {
            if ( 0xFFFFFFFF == classID )
            {
              if ( tmp->clID() != CLID_DataObject )
              {
                this->info() << format( "Class %8.8x (%5d) name ", tmp->clID(), tmp->clID()&0xFFFF )
                             << id << endmsg;
              }
            }
            if ( this->msgLevel(MSG::DEBUG) )
              this->debug() << "Found container '" << id << "' ClassID=" << tmp->clID()
                      << " Type='" << System::typeinfoName( typeid(*tmp) )
                      << endmsg;
            if ( tmp->clID() == classID )
            {
              if ( this->msgLevel(MSG::DEBUG) )
                this->debug() << " -> Matches target class ID " << classID << endmsg; 
              names.insert( id );
            }
            selectContainers( tmp, names, classID, forceRead );
          }
        }
      }
    }

    //===========================================================================

  private:

    typedef typename boost::remove_pointer<typename TABLE::From>::type _From;

  private:

    inline typename TABLE::From cloneFrom(const typename TABLE::From from)
    {
      if ( ! from )
      {
        error() << "FROM is NUL!!!!" << endmsg;
        return 0;
      }
      if ( ! from->parent() )
      {
        Warning("From is not in TES. Cannot clone!", StatusCode::FAILURE,0).ignore();
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
