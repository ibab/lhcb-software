
#include "RichSignalSICB.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichSignalSICB>  s_factory ;
const         IAlgFactory& RichSignalSICBFactory = s_factory ;

// Standard constructor, initializes variables
RichSignalSICB::RichSignalSICB( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : RichAlgBase ( name, pSvcLocator ) {

  declareProperty( "HitLocation",
                   m_RichHitLocation =  MCRichHitLocation::Default );
  declareProperty( "PrevLocation",
                   m_RichPrevLocation = "Prev/" + MCRichHitLocation::Default );
  declareProperty( "PrevPrevLocation",
                   m_RichPrevPrevLocation = "PrevPrev/" + MCRichHitLocation::Default );
  declareProperty( "NextLocation",
                   m_RichNextLocation = "Next/" + MCRichHitLocation::Default );
  declareProperty( "NextNextLocation",
                   m_RichNextNextLocation = "NextNext/" + MCRichHitLocation::Default );
  declareProperty( "SummedDepositLocation",
                   m_RichSummedDepositLocation = MCRichSummedDepositLocation::Default );
  declareProperty( "DepositLocation",
                   m_RichDepositLocation = MCRichDepositLocation::Default );

  declareProperty( "ChargedTrackTreatment", m_doChargedTracks = true );
  declareProperty( "UseSpillover", m_doSpillover = true );

}

RichSignalSICB::~RichSignalSICB() {};

StatusCode RichSignalSICB::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Initialize base class
  if ( !RichAlgBase::initialize() ) return StatusCode::FAILURE;

  // detector tool
  acquireTool( "PixelFinder", m_finder );

  if ( !m_rndm.initialize( randSvc(), Rndm::Flat(0.,1.) ) ) {
    msg << MSG::FATAL << "Unable to create Random generator" << endreq;
    return StatusCode::FAILURE;
  }

  // initialise charged track treatment
  if ( !InitParameters() ) return StatusCode::FAILURE;

  msg << MSG::DEBUG
      << " Using HPD signal algorithm for SICB" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichSignalSICB::execute() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Execute" << endreq;

  // Form a new containers of MCRichSummedDeposits and MCRichDeposits
  mcSummedDeposits = new MCRichSummedDeposits();
  if ( !eventSvc()->registerObject(m_RichSummedDepositLocation,mcSummedDeposits) ) {
    msg << MSG::ERROR << "Failed to register MCRichSummedDeposits at "
        << m_RichSummedDepositLocation << endreq;
    return StatusCode::FAILURE;
  }
  mcDeposits = new MCRichDeposits();
  if ( !eventSvc()->registerObject(m_RichDepositLocation,mcDeposits) ) {
    msg << MSG::ERROR << "Failed to register MCRichDeposits at "
        << m_RichDepositLocation << endreq;
    return StatusCode::FAILURE;
  }

  // Process main and spillover events with TOF offsets
  ProcessEvent( m_RichHitLocation,       0  );
  if ( m_doSpillover ) {
    ProcessEvent( m_RichPrevLocation,     -25 );
    ProcessEvent( m_RichPrevPrevLocation, -50 );
    ProcessEvent( m_RichNextLocation,      25 );
    // not needed yet
    //ProcessEvent( m_RichNextNextLocation,  50 );
  }

  // Debug Printout
  msg << MSG::DEBUG << "Created " << mcSummedDeposits->size()
      << " MCRichSummedDeposits at " << m_RichSummedDepositLocation << endreq;
  msg << MSG::DEBUG << "Created " << mcDeposits->size()
      << " MCRichDeposits at " << m_RichDepositLocation << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichSignalSICB::ProcessEvent( std::string hitLoc,
                                         double tofOffset ) {

  MsgStream msg(msgSvc(), name());

  // Load hits
  SmartDataPtr<MCRichHits> hits( eventSvc(), hitLoc );
  if ( !hits ) {
    msg << MSG::DEBUG << "Cannot locate MCRichHits at " << hitLoc << endreq;
    return StatusCode::SUCCESS;
  } else {
    msg << MSG::DEBUG << "Successfully located " << hits->size()
        << " MCRichHits at " << hitLoc << endreq;
  }

  // loop over hits
  for ( MCRichHits::const_iterator iHit = hits->begin();
        iHit != hits->end(); ++iHit ) {

    // Select normal hits
    if ( !(*iHit)->chargedTrack() ) {

      // positions on HPD window and correlated pixelID
      HepPoint3D position( (*iHit)->entry()/10.0  );
      RichSmartID id = m_finder->smartID(position);

      // Is hit in active pixel
      if ( 0 != id.index() && m_finder->isActive(id) ) {

        // Find out if we already have a hit for this super-pixel
        MCRichSummedDeposit * dep = mcSummedDeposits->object(id);
        if ( tofOffset < -1 && dep ) {
          // Toss a coin to see if we add this hit to the existing deposits
          // Simulate a 1/8 chance of additional hit falling in same sub-pixel as
          // already existing hit
          int iRan = (int)(m_rndm()*8.);
          if ( iRan != 0 ) continue;
        }

        // Then create a new deposit
        MCRichDeposit* newDeposit = new MCRichDeposit();
        mcDeposits->insert( newDeposit );
        newDeposit->setParentHit( *iHit );

        // Hit energy
        newDeposit->setEnergy( SimpleEnergy() );

        // TOF information
        double tof = tofOffset + (*iHit)->timeOfFlight()/1e9;
        if ( (*iHit)->rich() == Rich::Rich2 ) tof -= 30; // Global shift for Rich2
        //std::cout <<  (*iHit)->rich() << " normal hit tof " << tof << std::endl;
        newDeposit->setTime( tof );
        msg << MSG::VERBOSE << "Creating Deposit with TOF = " << tof << endreq;

        // Add to the set of other deposits in the pixel
        if ( !dep ) {
          dep = new MCRichSummedDeposit();
          mcSummedDeposits->insert( dep, id );
        }
        dep->addToDeposits(newDeposit);

      } // id OK

    } // normal hit if

    // select charged track hits
    if ( m_doChargedTracks && (*iHit)->chargedTrack() ) {

      // positions on HPD window and correlated pixelID
      HepPoint3D position( (*iHit)->entry()/10.0  );
      RichSmartID id = m_finder->smartID(position);

      // select valid hits
      if ( 0 != id.index() ) {

        // SicbCnv puts direction into exit !!!
        HepVector3D globalDirection = (*iHit)->exit();

        HepPoint3D position2 = position - globalDirection;

        HepPoint3D local = m_finder->localPosition2(position);
        HepPoint3D local2 = m_finder->localPosition2(position2);

        local.setZ( -local.z() );
        local2.setZ( -local2.z() );

        HepVector3D localDirection = local-local2;
        localDirection.setMag(1);

        if ( (*iHit)->mcParticle()
             && (*iHit)->timeOfFlight() < 50.0
             && (*iHit)->mcParticle()->momentum().isTimelike()
             && fabs( local.z() ) <= 10.0
             && id.index() != 0 ) {

          HepPoint3D hpdPosition = m_finder->hpdLocalPosition( id );

          HepVector3D normal = local-hpdPosition;
          if(local.z() > 0. ) normal.setZ( -normal.z() );

          double angle = normal.angle(localDirection);

          double beta = (*iHit)->mcParticle()->momentum().beta();
          double QE = 0.96;
          double upperAngle = 0;
          double lowerAngle = 0;
          double N = -9;

          int iBin = -999;

          std::map<int,double>::const_iterator itDbl;
          std::map<int,int>::const_iterator itInt;

          for ( int index = 0; index < 19; ++index ) {
            itDbl = m_Angle.find(index+1);
            upperAngle = (*itDbl).second;
            itDbl = m_Angle.find(index);
            lowerAngle = (*itDbl).second;
            if ( angle >= lowerAngle && angle < upperAngle ) {
              itInt = m_NPhotons.find(index);
              iBin = index;
              N = double((*itInt).second);
              continue;
            }
          }

          int nphotons;
          double betafac;
          double winradcurv = 5.50;
          double winthick = 0.70;

          if ( beta > 2/3 ) {
            betafac = pow( sin( acos( 1.0/(1.5*beta) ) ) , 2 );
            nphotons = int(  N*(betafac/0.5555555)*QE  );

            double aa = ( localDirection.x()*localDirection.x() +
                          localDirection.y()*localDirection.y() +
                          localDirection.z()*localDirection.z() );

            double bb = 2*( localDirection.x()*normal.x() +
                            localDirection.y()*normal.y() +
                            localDirection.z()*normal.z()  );

            double cc = ( normal.x()*normal.x() +
                          normal.y()*normal.y() +
                          normal.z()*normal.z() -
                          winradcurv*winradcurv );

            double b2minus4ac = bb*bb - 4.*aa*cc;

            double L11 = 0;
            double L12 = 0;
            double L21 = 0;
            double L22 = 0;
            double L1 = 0;
            double L2 = 0;

            if( b2minus4ac > 0) {L11 = (-bb-sqrt( b2minus4ac) ) / (2.*aa); }
            if( b2minus4ac > 0) {L12 = (-bb+sqrt( b2minus4ac) )/ (2.*aa); }
            if( fabs(L11) < fabs(L12)) { L1=L11; }
            if( fabs(L12) <= fabs(L11)) { L1=L12; }

            cc = ( normal.x()*normal.x()
                   + normal.y()*normal.y()
                   + normal.z()*normal.z()
                   - (winradcurv+winthick)*(winradcurv+winthick) );

            b2minus4ac=bb*bb-4.*aa*cc;

            if( b2minus4ac > 0) { L21 = ( -bb-sqrt(b2minus4ac))/(2.*aa);  }
            if( b2minus4ac > 0) { L22 = ( -bb+sqrt(b2minus4ac))/(2.*aa); }
            if( fabs(L21) <   fabs(L22)) { L2=L21; }
            if( fabs(L22) <= fabs(L21)) { L2=L22; }

            HepPoint3D entryloc(0.,0.,0.);
            HepPoint3D exitloc(0.,0.,0.);

            if ( angle < M_PI/2 ) {
              entryloc.setX( local.x() + L1*localDirection.x() );
              entryloc.setY( local.y() + L1*localDirection.y() );
              exitloc.setX( local.x() + L2*localDirection.x() );
              exitloc.setY( local.y() + L2*localDirection.z() );
            } else {
              entryloc.setX( local.x() + L2*localDirection.x() );
              entryloc.setY( local.y() + L2*localDirection.y() );
              exitloc.setX( local.x() + L1*localDirection.x() );
              exitloc.setY( local.y() + L1*localDirection.y() );
            }

            for ( int i = 0; i < nphotons; ++i ) {

              double X = entryloc.x()
                + localDirection.x()*x[ iBin+1 ][ i ]
                + localDirection.y()*y[ iBin+1 ][ i ];
              double Y =   entryloc.y()
                + localDirection.y()*x[ iBin+1 ][ i ]
                - localDirection.x()*y[ iBin+1 ][ i ];

              SicbPixelLocation sicbLocal(id.rich(), id.panel(),  X , Y  );
              HepPoint3D global = m_finder->globalPosition(sicbLocal);
              RichSmartID newid = m_finder->smartID(global);

              if ( id.PDCol() == newid.PDCol() &&
                   id.PDRow() == newid.PDRow() ) {

                if ( ( newid.index() != 0 ) && m_finder->isActive(newid) ) {

                  MCRichSummedDeposit * dep = mcSummedDeposits->object(newid);
                  if ( tofOffset < -1 && dep ) {
                    // Toss a coin to see if we add this hit to the existing deposits
                    // Simulate a 1/8 chance of additional hit falling in same sub-pixel as
                    // already existing hit
                    int iRan = (int)(m_rndm()*8.);
                    if ( iRan != 0 ) continue;
                  }

                  MCRichDeposit* newDeposit = new MCRichDeposit();
                  mcDeposits->insert( newDeposit );
                  newDeposit->setParentHit( *iHit );

                  // Hit energy
                  newDeposit->setEnergy( SimpleEnergy() );

                  // Charged track TOF correction
                  //double tof = tofOffset + (*iHit)->timeOfFlight()/1e9;
                  //if ( (*iHit)->rich() == Rich::Rich2 ) tof -= 47; // Incident shift for Rich2
                  //if ( (*iHit)->rich() == Rich::Rich1 ) tof -= 5;  // Incident shift for Rich1
                  double tof = tofOffset + 5;
                  newDeposit->setTime( tof );

                  // Add to the set of other deposits in the pixel
                  if ( !dep ) {
                    dep = new MCRichSummedDeposit();
                    mcSummedDeposits->insert( dep, newid );
                  }
                  dep->addToDeposits(newDeposit);

                }
              }
            }

          }
        }

      }

    } // end charged track hit if

  } // loop over MC Hits

  return StatusCode::SUCCESS;
}

StatusCode RichSignalSICB::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools
  releaseTool( m_finder );

  // finalize randomn number generator
  m_rndm.finalize();

  // finalize base class
  return RichAlgBase::finalize();
}

double RichSignalSICB::SimpleEnergy() {

  double energy = 0.015; // MeV
  if ( m_rndm() <= 0.2 ) {
    return m_rndm()*energy;
  } else {
    return energy;
  }

  return 0; // shouldn't happen
}

bool RichSignalSICB::InitParameters() {

  MsgStream msg(msgSvc(), name());

  m_Angle.clear();
  m_NPhotons.clear();

  m_Angle.insert( photonmap::value_type(0, 0.0000000));
  m_NPhotons.insert( photonmap2::value_type(0,  124));
  m_Angle.insert( photonmap::value_type(1, 0.1745329));
  m_NPhotons.insert( photonmap2::value_type(1,  110));
  m_Angle.insert( photonmap::value_type(2, 0.3490659));
  m_NPhotons.insert( photonmap2::value_type(2, 71));
  m_Angle.insert( photonmap::value_type(3,  0.5235988));
  m_NPhotons.insert( photonmap2::value_type(3, 62));
  m_Angle.insert( photonmap::value_type(4,  0.6981317));
  m_NPhotons.insert( photonmap2::value_type(4,  35));
  m_Angle.insert( photonmap::value_type(5, 0.8726646));
  m_NPhotons.insert( photonmap2::value_type(5, 43));
  m_Angle.insert( photonmap::value_type(6, 1.0471980));
  m_NPhotons.insert( photonmap2::value_type(6,  84));
  m_Angle.insert( photonmap::value_type(7, 1.2217300));
  m_NPhotons.insert( photonmap2::value_type(7,  394));
  m_Angle.insert( photonmap::value_type(8,  1.3962630));
  m_NPhotons.insert( photonmap2::value_type(8, 425));
  m_Angle.insert( photonmap::value_type(9,  1.5707960));
  m_NPhotons.insert( photonmap2::value_type(9,  443));
  m_Angle.insert( photonmap::value_type(10, 1.7453290));
  m_NPhotons.insert( photonmap2::value_type(10,  458));
  m_Angle.insert( photonmap::value_type(11,  1.9198620));
  m_NPhotons.insert( photonmap2::value_type(11,  102));
  m_Angle.insert( photonmap::value_type(12,  2.0943950));
  m_NPhotons.insert( photonmap2::value_type(12,  73));
  m_Angle.insert( photonmap::value_type(13,  2.2689280));
  m_NPhotons.insert( photonmap2::value_type(13,  68));
  m_Angle.insert( photonmap::value_type(14,  2.4434610));
  m_NPhotons.insert( photonmap2::value_type(14,  81));
  m_Angle.insert( photonmap::value_type(15,  2.6179940));
  m_NPhotons.insert( photonmap2::value_type(15,  111));
  m_Angle.insert( photonmap::value_type(16,  2.7925270));
  m_NPhotons.insert( photonmap2::value_type(16,  169));
  m_Angle.insert( photonmap::value_type(17,  2.9670600));
  m_NPhotons.insert( photonmap2::value_type(17,  189));
  m_Angle.insert( photonmap::value_type(18,  3.1415927));
  m_NPhotons.insert( photonmap2::value_type(18,  0));

  std::ifstream input;
  std::string dataFile = ( (std::string)getenv("PARAMFILESROOT") + 
                           "/data/RichHPD_sicb_incident_track.data" );
  input.open(dataFile.c_str());

  if ( !input ) {
    msg << MSG::ERROR << "Failed to load background from " << dataFile << endreq;
    return false;
  } else {
    msg << MSG::DEBUG << " Reading background file " << dataFile << endreq;

    char cline[1024];
    std::string line;

    double xx[3050];
    double yy[3050];
    unsigned int n;

    n = 0;
    while( input.good() ) {

      input.getline(cline,1024);
      line = cline;
      std::string::size_type pos = line.find( " " , 1 );

      xx[ n ] = atof( line.substr( 0,  13).c_str()  );
      yy[ n ] = atof( line.substr( pos, 13).c_str() );

      ++n;
    };

    n = 0;

    std::map<int,int>::const_iterator it;
    for ( int j = 0; j < 19; ++j ){
      it = m_NPhotons.find(j);
      for (int i = 0; i < (*it).second; ++i ) {
        x[ j ][ i ] = xx[ n ]/10.;
        y[ j ][ i ] = yy[ n ]/10.;
        n++;
      }
    }

  }

  return true;
}
