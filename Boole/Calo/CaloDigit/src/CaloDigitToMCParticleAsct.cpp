// $Id: CaloDigitToMCParticleAsct.cpp,v 1.1 2003-03-20 08:22:15 ocallot Exp $
// Include files 

// LHCb Kernel
#include "Relations/RelationWeighted1D.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/IDataProviderSvc.h"


// from Event
#include "Event/MCParticle.h"
#include "Event/MCCaloDigit.h"
#include "Event/CaloDigit.h"

// local
#include "CaloDigitToMCParticleAsct.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloDigitToMCParticleAsct
//
// 2003-02-20 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CaloDigitToMCParticleAsct>          s_factory ;
const        IAlgFactory& CaloDigitToMCParticleAsctFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloDigitToMCParticleAsct::CaloDigitToMCParticleAsct( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  , m_inputContainer       ( CaloDigitLocation::Hcal   )
  , m_mcContainer          ( MCCaloDigitLocation::Hcal )
  , m_outputTable          ( "Rec/Relations/HcalDigit" )
  , m_minimalFraction      ( 0.10 )
  , m_zMax                 ( 12335. )
{
  declareProperty( "InputContainer"   , m_inputContainer  );
  declareProperty( "McContainer"      , m_mcContainer     );
  declareProperty( "OutputTable"      , m_outputTable     );
  declareProperty( "MinimalFraction"  , m_minimalFraction );  
  declareProperty( "zMax"             , m_zMax            );  
}

//=============================================================================
// Destructor
//=============================================================================
CaloDigitToMCParticleAsct::~CaloDigitToMCParticleAsct() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode CaloDigitToMCParticleAsct::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloDigitToMCParticleAsct::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;
  
  SmartDataPtr< CaloDigits > digits ( eventSvc(), m_inputContainer );
  if( 0 == digits ) { 
    msg << MSG::ERROR << "Unable to retrieve " << m_inputContainer << endreq; 
    return StatusCode::SUCCESS;
  }   

  SmartDataPtr<MCCaloDigits > mcDigs ( eventSvc(), m_mcContainer );
  if( 0 == mcDigs ) { 
    msg << MSG::ERROR << "Unable to retrieve " << m_mcContainer << endreq; 
    return StatusCode::SUCCESS;
  }   

  typedef RelationWeighted1D<CaloDigit,MCParticle,double>  Table;

  // create relation table and register it in the event transient store
  Table* table = new Table();
  StatusCode sc = eventSvc()->registerObject( m_outputTable, table );
  if( sc.isFailure() ) {
    msg << MSG::ERROR << "Unable to register the relation container="
        << m_outputTable << endreq;
    msg << MSG::ERROR << "Status is " << sc << endreq;
    return sc ;
  }

  CaloDigits::const_iterator dig;
  for ( dig = digits->begin() ; digits->end() != dig ; ++dig ) {
    std::vector<const MCParticle*> parts;
    std::vector<double> energies;
    double eTot = 0.;
    
    CaloCellID cell = (*dig)->cellID();
    MCCaloDigit* mcDig = mcDigs->object( cell );
    msg << MSG::DEBUG << "Digit for " << cell << endreq;

    if ( 0 != mcDig ) {
      SmartRefVector<MCCaloHit> hits = mcDig->hits();
      SmartRefVector<MCCaloHit>::const_iterator ith;
      
      for ( ith = hits.begin(); hits.end() != ith; ith++ ) {
        const MCParticle* part = (*ith)->particle();
        
        msg << MSG::VERBOSE 
            << format( "    Part %4d ", part->key() );

        //=== Go back to parent if created after m_zMax
        const MCVertex*   vert   = part->originVertex();
        double zVert = vert->position().z();
        while ( m_zMax < zVert ) {
          zVert = -1000000.;
          part  = vert->mother();
          if ( 0 != part ) {
            vert = part->originVertex();
            if ( 0 != vert ) zVert = vert->position().z();
          }
        }

        double energy = (*ith)->activeE();
        eTot += energy;
        msg << format( " parent %4d Energy %8.1f", 
                       part->key(), energy ) << endreq;
        bool add = true;
        for ( unsigned int ll = 0; parts.size() > ll; ll++ ) {
          if ( part == parts[ll] ) {
            energies[ll] += energy;
            add = false;
          }
        }
        if ( add ) {
          parts.push_back( part );
          energies.push_back( energy );
        }
      }
    }
    //== Now, select those particles with enough energy
    if ( 0 < eTot ) {
      for ( unsigned int ll = 0; parts.size() > ll; ll++ ) {
        if ( energies[ll] > m_minimalFraction * eTot ) {
          table->relate( *dig, parts[ll], energies[ll] / eTot );
          msg << MSG::DEBUG 
              << format( " .. Relate MC %4d fraction %7.3f", 
                         parts[ll]->key(),
                         energies[ll] / eTot )
              << endreq;
        }
      }
    }
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloDigitToMCParticleAsct::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
