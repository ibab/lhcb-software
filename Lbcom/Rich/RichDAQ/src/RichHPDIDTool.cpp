
//-----------------------------------------------------------------------------
/** @file RichHPDIDTool.cpp
 *
 *  Implementation file for class : RichHPDIDTool
 *
 *  CVS Log :-
 *  $Id: RichHPDIDTool.cpp,v 1.2 2005-01-13 13:10:14 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.1  2005/01/07 12:35:59  jonrob
 *  Complete rewrite
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 2004-12-18
 */
//-----------------------------------------------------------------------------

// local
#include "RichHPDIDTool.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichHPDIDTool>          s_factory ;
const        IToolFactory& RichHPDIDToolFactory = s_factory ;

// Standard constructor
RichHPDIDTool::RichHPDIDTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : RichToolBase ( type, name , parent )
{

  declareInterface<IRichHPDIDTool>(this);

}

// Destructor
RichHPDIDTool::~RichHPDIDTool() {};

StatusCode RichHPDIDTool::initialize()
{
  // Initialise base class
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get RichSmartID tool
  IRichSmartIDTool * smartIDs;
  acquireTool( "RichSmartIDTool" , smartIDs );

  // Get list of all valid readout channels
  const RichSmartID::Collection & pixels = smartIDs->readoutChannelList();

  // release tool
  releaseTool( smartIDs );

  // Create temporary mapping between software and hardware numbering
  // Eventually, will need to come from some sort of data base
  unsigned int hID = 1;
  std::vector< unsigned int > nHPDs( Rich::NRiches, 0 );
  for ( RichSmartID::Collection::const_iterator iID = pixels.begin(); iID != pixels.end(); ++iID ) 
  {
    const RichSmartID hpdID = (*iID).pdID();
    if ( m_soft2hard.find(hpdID) == m_soft2hard.end() ) 
    {
      ++nHPDs[hpdID.rich()];
      m_soft2hard[hpdID] = hID;
      m_hard2soft[hID] = hpdID;
      verbose() << "RichSmartID " << hpdID << " -> hardID " << hID << endreq;
      ++hID;
    }
  }

  info() << "Created HPD hardware <-> software map : # HPDs RICH(1/2) = " << nHPDs[Rich::Rich1] 
         << " / " << nHPDs[Rich::Rich2] << endreq;

  return sc;
}

StatusCode RichHPDIDTool::finalize()
{
  // base class finalize
  return RichToolBase::finalize();
}

const RichDAQ::HPDHardwareID RichHPDIDTool::hardwareID( const RichSmartID smartID ) const
{
  // See if this RichSmartID is known
  SoftToHard::const_iterator id = m_soft2hard.find( smartID.pdID() );
  if ( m_soft2hard.end() == id ) 
  {
    std::ostringstream mess;
    mess << "Unknown HPD RichSmartID " << smartID;
    Exception ( mess.str() );
  }

  // Found, so return hardware ID
  return (*id).second;
}

const RichSmartID RichHPDIDTool::richSmartID( const RichDAQ::HPDHardwareID hID ) const
{
  // See if this hardware ID is known
  HardToSoft::const_iterator id = m_hard2soft.find( hID );
  if ( m_hard2soft.end() == id ) 
  {
    Exception ( "Unknown HPD hardware ID" + boost::lexical_cast<std::string>(hID) );
  }

  // Found, so return RichSmartID
  return (*id).second;
}
