
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
      : RelationsFromClonerAlg<TABLE> ( name , pSvcLocator )
    {
      this->declareProperty( "RemoveOriginals", m_removeOriginals = false );
      this->declareProperty( "RelationsBaseName",
                             m_relationsBaseName = DEFAULTS::relationsName );
      //this->setProperty( "OutputLevel", 2 );
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
      for ( std::vector<std::string>::const_iterator inputLoc = this->inputTESLocations().begin();
            inputLoc != this->inputTESLocations().end(); ++inputLoc )
      {
        const LHCb::Particle::Range particles =
          this -> template getIfExists<LHCb::Particle::Range>(*inputLoc);
        if ( !particles.empty() )
        {
          particleLoop( particles.begin(), particles.end() );
        }
      }

      // Now, loop over the gathered relation locations and clone them
      for ( std::set<std::string>::const_iterator iRLoc = m_relationLocations.begin();
            iRLoc != m_relationLocations.end(); ++iRLoc )
      {
        // Clone the relations and associated MCParticles
        this->copyTableFromLocation( *iRLoc );
        // if requested, remove the originals
        if ( m_removeOriginals )
        {
          TABLE * table = this -> template getIfExists<TABLE>(*iRLoc);
          if ( table )
          {
            const StatusCode sc = this -> evtSvc()->unregisterObject(table);
            if ( sc.isSuccess() )
            {
              delete table;
            }
            else
            {
              this->Error( "Failed to delete '" + *iRLoc + "'" ).ignore();
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
        // Save the relations location for this Particle
        m_relationLocations.insert( relationsLocation((*it)->parent()) );
        // Recursively find the relations for the daughters
        const SmartRefVector<LHCb::Particle> & daughters = (*it)->daughters();
        if ( !daughters.empty() )
        {
          particleLoop( daughters.begin(), daughters.end() );
        }
      }
    }

    /** Returns the full location of the given object in the Data Store
     *
     *  @param pObj Data object
     *
     *  @return Location of given data object
     */
    inline std::string objectLocation( const DataObject * pObj ) const
    {
      
      return ( !pObj ? "" : (pObj->registry() ? pObj->registry()->identifier() : "") );
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
    std::set<std::string> m_relationLocations;

    //===========================================================================

  };

}

#endif // MICRODST_RelationsFromParticleLocationsFromClonerAlg_H
