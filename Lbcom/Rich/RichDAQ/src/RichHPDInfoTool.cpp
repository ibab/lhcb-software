
//-----------------------------------------------------------------------------
/** @file RichHPDInfoTool.cpp
 *
 *  Implementation file for class : RichHPDInfoTool
 *
 *  CVS Log :-
 *  $Id: RichHPDInfoTool.cpp,v 1.3 2005-06-23 14:50:28 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 2004-12-18
 */
//-----------------------------------------------------------------------------

// local
#include "RichHPDInfoTool.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichHPDInfoTool>          s_factory ;
const        IToolFactory& RichHPDInfoToolFactory = s_factory ;

// Standard constructor
RichHPDInfoTool::RichHPDInfoTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : RichToolBase      ( type, name , parent ),
    m_killRandomHPDs  ( false ),
    m_killFrac        ( 0.1   ),
    m_raninit         ( 0     )
{
  // Interface
  declareInterface<IRichHPDInfoTool>(this);

  // job options
  declareProperty( "FractionToRandomlyKill", m_killFrac );
  declareProperty( "KillRandomHPDs", m_killRandomHPDs );
  declareProperty( "KilledHPDs", m_deadHpds );
  declareProperty( "RandomInit", m_raninit  );

  // random number distribution
  Rndm::Numbers m_uniDist;
}

// Destructor
RichHPDInfoTool::~RichHPDInfoTool() {};

StatusCode RichHPDInfoTool::initialize()
{
  // Initialise base class
  StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Build Mappings (should come from D-Base)
  sc = buildHPDMappings();
  if ( sc.isFailure() ) return sc;

  // Initialise "additional" dead and alive HPDs

  // Randomly choosen deadh HPDs
  sc = buildRandomDeadHPDs();
  if ( sc.isFailure() ) return sc;

  // Fixed dead HPDs
  sc = buildFixedDeadHPDs();
  if ( sc.isFailure() ) return sc;

  return sc;
}

StatusCode RichHPDInfoTool::buildHPDMappings()
{
  // get RichSmartID tool
  const IRichSmartIDTool * smartIDs;
  acquireTool( "RichSmartIDTool" , smartIDs );

  // Get list of all valid readout channels
  const RichSmartID::Collection & pixels = smartIDs->readoutChannelList();

  // Create temporary mapping between software and hardware numbering
  // Eventually, will need to come from some sort of data base
  m_soft2hard.clear();
  m_hard2soft.clear();
  m_smartIDs.clear();
  m_hardIDs.clear();
  m_hpdIsAlive.clear();
  unsigned int hID = 0;
  std::vector< unsigned int > nHPDs( Rich::NRiches, 0 );
  for ( RichSmartID::Collection::const_iterator iID = pixels.begin(); iID != pixels.end(); ++iID )
  {
    const RichSmartID hpdID = (*iID).pdID();
    if ( m_soft2hard.find(hpdID) == m_soft2hard.end() )
    {
      // set this HPD as alive
      m_hpdIsAlive[hpdID] = true;
      // count HPDs
      ++nHPDs[hpdID.rich()];
      // set up mappings
      m_soft2hard[hpdID] = hID;
      m_hard2soft[hID] = hpdID;
      // fill vectors of valid HPD IDs
      m_smartIDs.push_back(hpdID);
      m_hardIDs.push_back(hID);
      // debug printout
      verbose() << "RichSmartID " << hpdID << " -> hardID " << hID << endreq;
      // increment hardward ID
      ++hID;
    }
  }

  info() << "Created HPD hardware <-> software map : # HPDs RICH(1/2) = " << nHPDs[Rich::Rich1]
         << " / " << nHPDs[Rich::Rich2] << endreq;

  // release tool
  releaseTool( smartIDs );

  return StatusCode::SUCCESS;
}

StatusCode RichHPDInfoTool::buildFixedDeadHPDs()
{

  if ( !m_deadHpds.empty() )
  {

    info() << "Will manually disable the following HPDs :-" << endreq;
    for ( std::vector<std::string>::const_iterator iS = m_deadHpds.begin();
          iS != m_deadHpds.end(); ++iS )
    {
      // Extract HPD info from the string...
      std::string hpdInfo = *iS;
      int slash = hpdInfo.find_first_of( "/" );
      const int rich  = boost::lexical_cast<int>( hpdInfo.substr(0,slash) );
      hpdInfo = hpdInfo.substr(slash+1);
      slash = hpdInfo.find_first_of( "/" );
      const int panel = boost::lexical_cast<int>( hpdInfo.substr(0,slash) );
      hpdInfo = hpdInfo.substr(slash+1);
      slash = hpdInfo.find_first_of( "/" );
      const int pdCol = boost::lexical_cast<int>( hpdInfo.substr(0,slash) );
      hpdInfo = hpdInfo.substr(slash+1);
      slash = hpdInfo.find_first_of( "/" );
      const int pdRow = boost::lexical_cast<int>( hpdInfo.substr(0,slash) );

      // Create HPD smartID
      const RichSmartID hpdID(rich,panel,pdRow,pdCol);

      // kill it
      m_hpdIsAlive[hpdID] = false;

      // printout
      info() << " -> disabled HPD " << hpdID << endreq;

    }

  }

  return StatusCode::SUCCESS;
}

StatusCode RichHPDInfoTool::buildRandomDeadHPDs()
{

  if ( m_killRandomHPDs )
  {

    // Check fraction to kill
    if ( m_killFrac > 1 || m_killFrac < 0 )
    {
      return Error( "Fraction of HPDs to kill must be between 0 and 1 !!");
    }

    // randomn number service
    IRndmGenSvc * randSvc = svc<IRndmGenSvc>( "RndmGenSvc", true );
    if ( !m_uniDist.initialize( randSvc, Rndm::Flat(0,1) ) )
    {
      return Error( "Unable to initialise random numbers" );
    }
    // simple trick to change HPDs randomly killed
    info() << "Intialising random numbers with " << m_raninit << " calls" << endreq;
    for ( unsigned int i = 0; i < m_raninit; ++i ) { m_uniDist(); }

    // Get list of active HPD RichSmartIDs
    const RichSmartID::Collection & hpdIDs = activeHPDRichSmartIDs();

    // Number of HPDs to kill
    const unsigned int nHPDsToKill = (unsigned int)(m_killFrac * hpdIDs.size());
    info() << "Will randomly disable " << nHPDsToKill << " ("
           <<  m_killFrac*100<< "%) of HPDs" << endreq;

    // Loop over HPDs and kill random ones
    const unsigned int maxTries(999);
    unsigned int nKilled(0), nTries(0);
    while ( nKilled < nHPDsToKill && nTries < maxTries )
    {
      // loop over all HPDs
      for ( RichSmartID::Collection::const_iterator iHPD = hpdIDs.begin();
            iHPD != hpdIDs.end(); ++ iHPD )
      {
        // Randomly decide to kill it or not
        if ( m_uniDist() < m_killFrac )
        {
          // Decided to kill this one, but first see if already dead
          if ( m_hpdIsAlive.end() == m_hpdIsAlive.find(*iHPD) || m_hpdIsAlive[*iHPD] )
          {
            // Kill this HPD and increment count
            m_hpdIsAlive[*iHPD] = false;
            ++nKilled;
            info() << " -> disabled HPD " << *iHPD << endreq;
          }
        }
        else
        {
          m_hpdIsAlive[*iHPD] = true;
        }
      }
      ++nTries; // Protect against infinite loops
    }
    if ( nTries == maxTries )
    {
      return Warning( "Timed out whilst trying to create dead HPD list. Something is wrong" );
    }

  }

  return StatusCode::SUCCESS;
}

StatusCode RichHPDInfoTool::finalize()
{
  // Release things
  m_uniDist.finalize();

  // base class finalize
  return RichToolBase::finalize();
}

const RichDAQ::HPDHardwareID RichHPDInfoTool::hardwareID( const RichSmartID smartID ) const
{
  // See if this RichSmartID is known
  SoftToHard::const_iterator id = m_soft2hard.find( smartID.pdID() );
  if ( m_soft2hard.end() == id )
  {
    std::ostringstream mess;
    mess << "hardwareID:: Unknown RichSmartID " << smartID;
    Exception ( mess.str() );
  }

  // Found, so return hardware ID
  return (*id).second;
}

const RichSmartID RichHPDInfoTool::richSmartID( const RichDAQ::HPDHardwareID hID ) const
{
  // See if this hardware ID is known
  HardToSoft::const_iterator id = m_hard2soft.find( hID );
  if ( m_hard2soft.end() == id )
  {
    Exception ( "richSmartID:: Unknown HPD hardware ID " + boost::lexical_cast<std::string>(hID) );
  }

  // Found, so return RichSmartID
  return (*id).second;
}

const RichSmartID::Collection & RichHPDInfoTool::activeHPDRichSmartIDs() const
{
  return m_smartIDs;
}

const RichDAQ::HPDHardwareIDs & RichHPDInfoTool::activeHPDHardwareIDs() const
{
  return m_hardIDs;
}

bool RichHPDInfoTool::hpdIsActive( const RichSmartID id ) const
{
  // Try and find HPD in status map
  HPDDeadMap::iterator iID = m_hpdIsAlive.find( id.pdID() );
  // If failed, issue an error
  if ( m_hpdIsAlive.end() == iID )
  {
    std::ostringstream mess;
    mess << "hpdIsActive:: Unknown RichSmartID " << id;
    Exception ( mess.str() );
  }
  // return the status of the HPD
  if ( msgLevel(MSG::DEBUG) && !(*iID).second )
  {
    debug() << "HPD " << id.pdID() << " is disabled" << endreq;
  }
  return (*iID).second;
}

bool RichHPDInfoTool::hpdIsActive( const RichDAQ::HPDHardwareID id ) const
{
  return hpdIsActive( richSmartID(id) );
}
