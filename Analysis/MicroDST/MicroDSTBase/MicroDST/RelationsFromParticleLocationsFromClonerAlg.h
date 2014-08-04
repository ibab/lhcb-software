
#ifndef MICRODST_RelationsFromParticleLocationsFromClonerAlg_H
#define MICRODST_RelationsFromParticleLocationsFromClonerAlg_H 1

// From MicroDST
#include "MicroDST/RelationsFromClonerAlg.h"

// Event
#include "Event/Particle.h"

// boost
#include "boost/algorithm/string.hpp"

namespace MicroDST
{

  /** @class MicroDST/RelationsFromParticleLocationsFromClonerAlg.h
   *
   *  Extension to RelationsClonerFromAlg that dynamically finds 
   *  the relations to clone from Particles
   *
   *  @author Juan PALACIOS juan.palacios@nikhef.nl
   *  @date   2009-04-14
   */

  template <typename TABLE>
  class RelationsFromParticleLocationsFromClonerAlg : public RelationsFromClonerAlg<TABLE>
  {

    //===========================================================================

  private:

    //===========================================================================

    typedef Defaults<TABLE> DEFAULTS;

    //===========================================================================

  public:

    //===========================================================================

    /// Standard constructor
    RelationsFromParticleLocationsFromClonerAlg( const std::string& name,
                                             ISvcLocator* pSvcLocator )
      : RelationsFromClonerAlg<TABLE> ( name, pSvcLocator )
    {
      this->declareProperty( "RemoveOriginals", m_removeOriginals = false );
      this->declareProperty( "RelationsBaseName",
                             m_relationsBaseName = DEFAULTS::relationsName );
      this->declareProperty( "UseRelationsCLID", m_useClassID = false,
                             "Flag to turn on the automatic searching for relations to clone based on CLID" );
      //this->setProperty( "OutputLevel", 1 );
    }

    ~RelationsFromParticleLocationsFromClonerAlg( ) {} ///< Destructor

    //===========================================================================

  public:

    //=========================================================================

    virtual StatusCode execute()
    {
      if ( this->msgLevel(MSG::DEBUG) ) this->debug() << "==> Execute" << endmsg;

      this->setFilterPassed(true);

      // Clear the list of relations locations
      m_relationLocations.clear();

      // Load the Particles and iterate over them, and their daughters,
      // to find all the relations to save
      for ( const auto& loc : this->inputTESLocations() )
      {
        const LHCb::Particle::Range particles =
          this -> template getIfExists<LHCb::Particle::Range>(loc);
        if ( !particles.empty() )
        {
          particleLoop( particles.begin(), particles.end() );
        }
      }

      // Now, loop over the gathered relation locations and clone them
      for ( const auto& loc : m_relationLocations )
      {
        // Clone the relations
        this->copyTableFromLocation( loc );
        // if requested, remove the originals
        if ( m_removeOriginals )
        {
          TABLE * table = this -> template getIfExists<TABLE>(loc);
          if ( table )
          {
            const StatusCode sc = this -> evtSvc()->unregisterObject(table);
            if ( sc.isSuccess() )
            {
              delete table;
            }
            else
            {
              this->Error( "Failed to delete '" + loc + "'" ).ignore();
            }
          }
        }
      }

      return StatusCode::SUCCESS;
    }

    //===========================================================================

  private:

    //===========================================================================

    template <class Iter>
    void particleLoop( const Iter begin, const Iter end )
    {
      for ( Iter it = begin; it != end; ++it )
      {
        // If m_relationsBaseName is not empty, use it to deduce possible 
        // TES locations for relations to clone
        if ( !m_relationsBaseName.empty() ) 
        { 
          m_relationLocations.insert( relationsLocation((*it)->parent()) );
        }
        // If the CLID is defined, use this to search for objects of the correct
        // type in the TES at the same level as the particles
        if ( m_useClassID )
        {
          // Get particle location
          std::string pLoc = objectLocation( (*it)->parent() );
          // Strip the trailing "/Particles"
          boost::erase_all( pLoc, "/Particles" );
          // Load the resulting data node
          DataObject * node = this -> template getIfExists<DataObject>(pLoc);
          if ( node )
          {
            // Search for objects with the requested CLID below this location
            this->selectContainers( node, m_relationLocations, TABLE::classID() );
          }            
        }
        // type in the TES for each Particle location
        // Recursively find the relations for the daughters
        const SmartRefVector<LHCb::Particle> & daughters = (*it)->daughters();
        if ( !daughters.empty() )
        {
          this->particleLoop( daughters.begin(), daughters.end() );
        }
      }
    }

    inline std::string relationsLocation( const DataObject * pObj ) const
    {
      // Get the parent container TES location
      std::string pLoc = objectLocation( pObj );
      if ( pLoc.empty() ) { return pLoc; }

      // Form the relations TES location for this location
      boost::replace_all( pLoc, "/Particles", m_relationsBaseName );

      // return the new location
      return pLoc;
    }

    //===========================================================================

  private:

    //===========================================================================

    bool m_removeOriginals;

    std::string m_relationsBaseName;
    bool m_useClassID;

    std::set<std::string> m_relationLocations;

    //===========================================================================

  };

}

#endif // MICRODST_RelationsFromParticleLocationsFromClonerAlg_H
