
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleCALOBaseAlg.h
 *
 * Header file for algorithm ChargedProtoParticleCALOBaseAlg
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleCALOBaseAlg.h,v 1.1 2009-08-29 20:37:19 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_ChargedProtoParticleCALOBaseAlg_H
#define GLOBALRECO_ChargedProtoParticleCALOBaseAlg_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Event
#include "Event/ProtoParticle.h"

// from CaloUtils
#include "CaloUtils/ICaloElectron.h"
#include "CaloUtils/Calo2Track.h"
#include "Event/CaloDataFunctor.h"
#include "CaloDet/DeCalorimeter.h"
#include "CaloUtils/CaloAlgUtils.h"

// Relations
#include "Relations/IRelation.h"
#include "Relations/IRelationWeighted2D.h"
#include "CaloInterfaces/ICaloHypoEstimator.h"

/** @class ChargedProtoParticleCALOBaseAlg ChargedProtoParticleCALOBaseAlg.h
 *
 *  Base class for ProtoParticle algorithms that add CALO information
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 28/08/2009
 */

class ChargedProtoParticleCALOBaseAlg : public GaudiAlgorithm
{

public:

  /// Standard constructor
  ChargedProtoParticleCALOBaseAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedProtoParticleCALOBaseAlg( ); ///< Destructor

  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode execute();    ///< Algorithm execution
  virtual StatusCode finalize();   ///< Algorithm finalization

protected:

  /// Loads a CALO relations table
  template < typename TYPE >
  inline bool loadCaloTable( TYPE *& table, const std::string & location ) const
  {
    const bool ok = exist<TYPE>(location);
    if ( !ok )
    {
      table = NULL;
      Warning("No CALO "+System::typeinfoName(typeid(TYPE))+
              " table at '"+location+"'", StatusCode::SUCCESS ).ignore();
    }
    else 
    {
      table = get<TYPE>( location );
    }
    return ok;
  }

  // Add extra info from CaloDigits (Spd+Prs)
  ICaloHypoEstimator* m_estimator ;

private:



};

#endif // GLOBALRECO_ChargedProtoParticleCALOBaseAlg_H
