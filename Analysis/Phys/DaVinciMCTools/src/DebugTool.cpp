// $Id: DebugTool.cpp,v 1.5 2003-03-07 11:01:55 odie Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// from Event
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/Particle.h"

// from CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// local
#include "DebugTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DebugTool
//
// 29/03/2001 : Olivier Dormond
//-----------------------------------------------------------------------------

// Declaration of the AlgTool Factory
static ToolFactory<DebugTool>          Factory;
const IToolFactory& DebugToolFactory = Factory;

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
DebugTool::DebugTool( const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
  : AlgTool( type, name, parent ), m_ppSvc(0), m_keys(0)
{

  if( serviceLocator() ) {
    StatusCode sc = StatusCode::FAILURE;
    sc = service("ParticlePropertySvc",m_ppSvc);
  }
  if( !m_ppSvc ) {
    throw GaudiException( "ParticlePropertySvc not found",
                          "DebugException",
                          StatusCode::FAILURE );
  }

  declareInterface<IDebugTool>(this);

  declareProperty( "PrintDepth", m_depth = 999 );
  declareProperty( "TreeWidth", m_treeWidth = 20 );
  declareProperty( "FieldWidth", m_fWidth = 10 );
  declareProperty( "FieldPrecision", m_fPrecision = 3 );
  declareProperty( "Arrow", m_arrow = "+-->" );
  declareProperty( "Informations", m_informations = "Name E M P Pt phi Vz" );
}

//=============================================================================
// Standard destructor
//=============================================================================

DebugTool::~DebugTool( ) { }

StatusCode DebugTool::initialize( void )
{
  MsgStream log(msgSvc(), name());

  std::size_t oldpos = 0, pos;
  do {
    pos=m_informations.find( ' ', oldpos );
    std::string tok(m_informations, oldpos, pos-oldpos);
    if( tok=="Name" )       m_keys.push_back(Name);
    else if( tok=="E" )     m_keys.push_back(E);
    else if( tok=="M" )     m_keys.push_back(M);
    else if( tok=="P" )     m_keys.push_back(P);
    else if( tok=="Pt" )    m_keys.push_back(Pt);
    else if( tok=="Px" )    m_keys.push_back(Px);
    else if( tok=="Py" )    m_keys.push_back(Py);
    else if( tok=="Pz" )    m_keys.push_back(Pz);
    else if( tok=="Vx" )    m_keys.push_back(Vx);
    else if( tok=="Vy" )    m_keys.push_back(Vy);
    else if( tok=="Vz" )    m_keys.push_back(Vz);
    else if( tok=="theta" ) m_keys.push_back(theta);
    else if( tok=="phi" )   m_keys.push_back(phi);
    else if( tok=="eta" )   m_keys.push_back(eta);
    else if( tok=="IDCL" )  m_keys.push_back(idcl);
    else
      log << MSG::ERROR << "Unknown output key '" << tok << "'. Ignoring it."
          << endreq;
    if( pos != std::string::npos ) oldpos = pos+1;
    else                           oldpos = pos;
  }
  while( pos != std::string::npos );
  return StatusCode::SUCCESS;
}

//=============================================================================
// printHeader
//=============================================================================
void DebugTool::printHeader( MsgStream &log, bool mcfirst, bool associated )
{
  static const std::string mctitle = " MCParticle ";
  static const std::string title = " Particle ";
  const std::string *title1, *title2;
  if( mcfirst ) {
    title1 = &mctitle;
    title2 = &title;
  } else {
    title1 = &title;
    title2 = &mctitle;
  }

  bool name_key = false;
  std::vector<InfoKeys>::iterator i;
  for( i = m_keys.begin(); i!= m_keys.end(); i++ )
    if( *i == Name )
      name_key = true;
  int n_keys = m_keys.size() - (name_key ? 1 : 0);
  int width1 = n_keys*m_fWidth + (name_key ? m_treeWidth : 0);
  int width2 = n_keys*m_fWidth + (name_key ? m_fWidth : 0);
  int left1  = (width1-title1->length()-2)/2;
  int right1 =  width1-title1->length()-2 - left1;
  int left2  = (width2-title2->length()-2)/2;
  int right2 =  width2-title2->length()-2 - left2;

  if( left1 >= 0 )
    log << '<' << std::string(left1,'-');
  log << *title1;
  if( right1 >= 0 )
    log<< std::string(right1,'-') << '>';

  if( associated ) {
    if( left2 >= 0 )
      log << '<' << std::string(left2,'-');
    log << *title2;
    if( right2 >= 0 )
      log<< std::string(right2,'-') << '>';
  }
  log << std::endl;

  for( i = m_keys.begin(); i!= m_keys.end(); i++ )
    switch( *i ) {
    case Name:      log << std::setw(m_treeWidth) << "Name";   break;
    case E:         log << std::setw(m_fWidth) << "E";         break;
    case M:         log << std::setw(m_fWidth) << "M";         break;
    case P:         log << std::setw(m_fWidth) << "P";         break;
    case Pt:        log << std::setw(m_fWidth) << "Pt";        break;
    case Px:        log << std::setw(m_fWidth) << "Px";        break;
    case Py:        log << std::setw(m_fWidth) << "Py";        break;
    case Pz:        log << std::setw(m_fWidth) << "Pz";        break;
    case Vx:        log << std::setw(m_fWidth) << "Vx";        break;
    case Vy:        log << std::setw(m_fWidth) << "Vy";        break;
    case Vz:        log << std::setw(m_fWidth) << "Vz";        break;
    case theta:     log << std::setw(m_fWidth) << "theta";     break;
    case phi:       log << std::setw(m_fWidth) << "phi";       break;
    case eta:       log << std::setw(m_fWidth) << "eta";       break;
    case idcl:      log << std::setw(m_fWidth) << "ID CL";     break;
    }
  if( associated )
    for( i = m_keys.begin(); i!= m_keys.end(); i++ )
      switch( *i ) {
      case Name:    log << std::setw(m_fWidth) << "Name";      break;
      case E:       log << std::setw(m_fWidth) << "E";         break;
      case M:       log << std::setw(m_fWidth) << "M";         break;
      case P:       log << std::setw(m_fWidth) << "P";         break;
      case Pt:      log << std::setw(m_fWidth) << "Pt";        break;
      case Px:      log << std::setw(m_fWidth) << "Px";        break;
      case Py:      log << std::setw(m_fWidth) << "Py";        break;
      case Pz:      log << std::setw(m_fWidth) << "Pz";        break;
      case Vx:      log << std::setw(m_fWidth) << "Vx";        break;
      case Vy:      log << std::setw(m_fWidth) << "Vy";        break;
      case Vz:      log << std::setw(m_fWidth) << "Vz";        break;
      case theta:   log << std::setw(m_fWidth) << "theta";     break;
      case phi:     log << std::setw(m_fWidth) << "phi";       break;
      case eta:     log << std::setw(m_fWidth) << "eta";       break;
      case idcl:    log << std::setw(m_fWidth) << "ID CL";     break;
      }
  log << std::endl;

  for( i = m_keys.begin(); i!= m_keys.end(); i++ )
    switch( *i ) {
    case Name:      log << std::setw(m_treeWidth) << " ";      break;
    case E:         log << std::setw(m_fWidth) << "GeV";       break;
    case M:         log << std::setw(m_fWidth) << "GeV";       break;
    case P:         log << std::setw(m_fWidth) << "GeV";       break;
    case Pt:        log << std::setw(m_fWidth) << "GeV";       break;
    case Px:        log << std::setw(m_fWidth) << "GeV";       break;
    case Py:        log << std::setw(m_fWidth) << "GeV";       break;
    case Pz:        log << std::setw(m_fWidth) << "GeV";       break;
    case Vx:        log << std::setw(m_fWidth) << "cm";        break;
    case Vy:        log << std::setw(m_fWidth) << "cm";        break;
    case Vz:        log << std::setw(m_fWidth) << "cm";        break;
    case theta:     log << std::setw(m_fWidth) << "mrad";      break;
    case phi:       log << std::setw(m_fWidth) << "mrad";      break;
    case eta:       log << std::setw(m_fWidth) << "prap";      break;
    case idcl:      log << std::setw(m_fWidth) << " ";         break;
    }
  if( associated )
    for( i = m_keys.begin(); i!= m_keys.end(); i++ )
      switch( *i ) {
      case Name:    log << std::setw(m_fWidth) << " ";         break;
      case E:       log << std::setw(m_fWidth) << "GeV";       break;
      case M:       log << std::setw(m_fWidth) << "GeV";       break;
      case P:       log << std::setw(m_fWidth) << "GeV";       break;
      case Pt:      log << std::setw(m_fWidth) << "GeV";       break;
      case Px:      log << std::setw(m_fWidth) << "GeV";       break;
      case Py:      log << std::setw(m_fWidth) << "GeV";       break;
      case Pz:      log << std::setw(m_fWidth) << "GeV";       break;
      case Vx:      log << std::setw(m_fWidth) << "cm";        break;
      case Vy:      log << std::setw(m_fWidth) << "cm";        break;
      case Vz:      log << std::setw(m_fWidth) << "cm";        break;
      case theta:   log << std::setw(m_fWidth) << "mrad";      break;
      case phi:     log << std::setw(m_fWidth) << "mrad";      break;
      case eta:     log << std::setw(m_fWidth) << "prap";      break;
      case idcl:    log << std::setw(m_fWidth) << " ";         break;
      }
  log << std::endl;
}

//=============================================================================
// printInfo (MCParticle)
//=============================================================================
void DebugTool::printInfo( const std::string &prefix, const MCParticle *part,
                           Particle2MCAsct::IAsct *assoc, MsgStream &log )
{
  ParticleProperty* p = m_ppSvc->findByStdHepID( part->particleID().pid() );
  const MCVertex *origin = part->originVertex();

  std::vector<InfoKeys>::iterator i;
  for( i = m_keys.begin(); i!= m_keys.end(); i++ )
    switch( *i ) {
    case Name:
      {
        std::string p_name = p ? p->particle() : "N/A";
        int p_len = p_name.length();
        if( prefix.length() == 0 ) {
          if( p_len > m_treeWidth )
            p_len = m_treeWidth - 1;
          log << p_name << std::string( m_treeWidth - p_len - 1,' ' );
        } else {
          if( p_len > (int)(m_treeWidth-prefix.length()-m_arrow.length()) )
            p_len = m_treeWidth - prefix.length() - m_arrow.length() - 1;
          log << prefix.substr(0, prefix.length()-1) << m_arrow << p_name
              << std::string( m_treeWidth - prefix.length()
                              - m_arrow.length() - p_len, ' ' );
        }
      }
      break;
    case E:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->momentum().e()/GeV;
      break;
    case M:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->momentum().m()/GeV;
      break;
    case P:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->momentum().vect().mag()/GeV;
      break;
    case Pt:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->momentum().perp()/GeV;
      break;
    case Px:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->momentum().px()/GeV;
      break;
    case Py:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->momentum().py()/GeV;
      break;
    case Pz:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->momentum().pz()/GeV;
      break;
    case Vx:
      if( origin )
        log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
            << origin->position().x()/cm;
      else
        log << std::setw(m_fWidth) << " N/A ";
      break;
    case Vy:
      if( origin )
        log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
            << origin->position().y()/cm;
      else
        log << std::setw(m_fWidth) << " N/A ";
      break;
    case Vz:
      if( origin )
        log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
            << origin->position().z()/cm;
      else
        log << std::setw(m_fWidth) << " N/A ";
      break;
    case theta:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->momentum().theta()/mrad;
      break;
    case phi:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->momentum().phi()/mrad;
      break;
    case eta:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->momentum().pseudoRapidity();
      break;
    case idcl:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << 1.0;
      break;
    default:
      break;
    }

  if( assoc ) {
    Particle *reco = NULL;
    //StatusCode sc = assoc->retrieveInverse( const_cast<MCParticle *>(part),
    //                                        reco );
    //if( sc.isSuccess() && reco )
    // New associator
    // take the first one (just to show how it works). If void pointer no
    // association found
    reco = assoc->associatedTo( const_cast<MCParticle *>(part) );
    if( reco )
      for( i = m_keys.begin(); i!= m_keys.end(); i++ )
        switch( *i ) {
        case Name:
          {
            p = m_ppSvc->findByStdHepID( reco->particleID().pid() );
            std::string p_name = p ? p->particle() : "N/A";
            int p_len = p_name.length();
            if( p_len > m_fWidth-1 )
              p_len = m_fWidth-2;
            log << ' ' << p_name
                << std::string(m_fWidth-1-p_len, ' ');
          }
          break;
        case E:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->momentum().e()/GeV;
          break;
        case M:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->momentum().m()/GeV;
          break;
        case P:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->momentum().vect().mag()/GeV;
          break;
        case Pt:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->momentum().perp()/GeV;
          break;
        case Px:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->momentum().px()/GeV;
          break;
        case Py:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->momentum().py()/GeV;
          break;
        case Pz:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->momentum().pz()/GeV;
          break;
        case Vx:
          if( origin )
            log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
                << reco->pointOnTrack().x()/cm;
          else
            log << std::setw(m_fWidth) << " N/A ";
          break;
        case Vy:
          if( origin )
            log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
                << reco->pointOnTrack().y()/cm;
          else
            log << std::setw(m_fWidth) << " N/A ";
          break;
        case Vz:
          if( origin )
            log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
                << reco->pointOnTrack().z()/cm;
          else
            log << std::setw(m_fWidth) << " N/A ";
          break;
        case theta:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->momentum().theta()/mrad;
          break;
        case phi:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->momentum().phi()/mrad;
          break;
        case eta:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->momentum().pseudoRapidity();
          break;
        case idcl:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->confLevel();
          break;
        default:
          break;          
        }
    else 
      log << "  No associated particle";
  }
  log << std::endl;
}

//=============================================================================
// printInfo
//=============================================================================
void DebugTool::printInfo( const std::string &prefix, const Particle *reco,
                           Particle2MCAsct::IAsct *assoc, MsgStream &log )
{
  ParticleProperty* p = m_ppSvc->findByStdHepID( reco->particleID().pid() );
  std::vector<InfoKeys>::iterator i;
  for( i = m_keys.begin(); i!= m_keys.end(); i++ )
    switch( *i ) {
    case Name:
      {
        std::string p_name = p ? p->particle() : "N/A";
        int p_len = p_name.length();
        if( prefix.length() == 0 ) {
          if( p_len > m_treeWidth )
            p_len = m_treeWidth - 1;
          log << p_name << std::string( m_treeWidth - p_len - 1,' ' );
        } else {
          if( p_len > (int)(m_treeWidth-prefix.length()-m_arrow.length()) )
            p_len = m_treeWidth - prefix.length() - m_arrow.length() - 1;
          log << prefix.substr(0, prefix.length()-1) << m_arrow << p_name
              << std::string( m_treeWidth - prefix.length() - m_arrow.length()
                              - p_len, ' ' );
        }
      }
      break;
    case E:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->momentum().e()/GeV;
      break;
    case M:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->momentum().m()/GeV;
      break;
    case P:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->momentum().vect().mag()/GeV;
      break;
    case Pt:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->momentum().perp()/GeV;
      break;
    case Px:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->momentum().px()/GeV;
      break;
    case Py:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->momentum().py()/GeV;
      break;
    case Pz:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->momentum().pz()/GeV;
      break;
    case Vx:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->pointOnTrack().x()/cm;
      break;
    case Vy:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->pointOnTrack().y()/cm;
      break;
    case Vz:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->pointOnTrack().z()/cm;
      break;
    case theta:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->momentum().theta()/mrad;
      break;
    case phi:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->momentum().phi()/mrad;
      break;
    case eta:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->momentum().pseudoRapidity();
      break;
    case idcl:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->confLevel();
      break;
    default:
      break;
    }

  if( assoc ) {
    MCParticle *part = NULL;
    //StatusCode sc = assoc->retrieveDirect( const_cast<Particle *>(reco),
    //                                       part );
    //if( sc.isSuccess() && part )
    // New associator
    // take the first one (just to show how it works). If void pointer no
    // association found
    part = assoc->associatedFrom( const_cast<Particle *>(reco) );
    if( part )
      for( i = m_keys.begin(); i!= m_keys.end(); i++ )
        switch( *i ) {
        case Name:
          {
            p = m_ppSvc->findByStdHepID( reco->particleID().pid() );
            std::string p_name = p ? p->particle() : "N/A";
            int p_len = p_name.length();
            if( p_len > m_fWidth-1 )
              p_len = m_fWidth-2;
            log << ' ' << p_name
                << std::string(m_fWidth-1-p_len, ' ');
          }
          break;
        case E:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << part->momentum().e()/GeV;
          break;
        case M:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << part->momentum().m()/GeV;
          break;
        case P:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << part->momentum().vect().mag()/GeV;
          break;
        case Pt:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << part->momentum().perp()/GeV;
          break;
        case Px:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << part->momentum().px()/GeV;
          break;
        case Py:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << part->momentum().py()/GeV;
          break;
        case Pz:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << part->momentum().pz()/GeV;
          break;
        case Vx:
          {
            const MCVertex *origin = part->originVertex();
            if( origin )
              log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
                  << origin->position().x()/cm;
            else
              log << std::setw(m_fWidth) << " N/A ";
          }
          break;
        case Vy:
          {
            const MCVertex *origin = part->originVertex();
            if( origin )
              if( origin )
                log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
                    << origin->position().y()/cm;
              else
                log << std::setw(m_fWidth) << " N/A ";
          }
          break;
        case Vz:
          {
            const MCVertex *origin = part->originVertex();
            if( origin )
              if( origin )
                log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
                    << origin->position().z()/cm;
              else
                log << std::setw(m_fWidth) << " N/A ";
          }
          break;
        case theta:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << part->momentum().theta()/mrad;
          break;
        case phi:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << part->momentum().phi()/mrad;
          break;
        case eta:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << part->momentum().pseudoRapidity();
          break;
        case idcl:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << 1.0;
          break;
        default:
          break;
        }
    else
      log << "  No associated particle";
  }
  log << std::endl;  
}

//=============================================================================
// Print decay tree for a given MCparticle
//=============================================================================
void DebugTool::printTree( const MCParticle *mother, int maxDepth = -1 )
{
  printTree( mother, NULL, maxDepth );
}

//=============================================================================
// Print decay tree for a given MCparticle
//=============================================================================
void DebugTool::printTree( const MCParticle *mother, 
                           Particle2MCAsct::IAsct *assoc,
                           int maxDepth = -1 )
{
  if( maxDepth == -1 )
    maxDepth = m_depth;
  
  MsgStream log(msgSvc(), name());

  if( !mother ) {
    log << MSG::ERROR << "printTree called with NULL MCParticle" << endreq;
    return;
  }

  log << MSG::INFO << std::endl;
  printHeader( log, true, assoc != NULL );

  log.setf(std::ios::fixed,std::ios::floatfield);
  printDecayTree( mother, assoc, "", maxDepth, log );
  log << endreq;
}

//=============================================================================
// printEventAsTree (MCParticle)
//=============================================================================
void DebugTool::printEventAsTree( const std::vector<MCParticle*> &event )
{
  printEventAsTree( event, NULL );
}

//=============================================================================
// printEventAsTree (MCParticle)
//=============================================================================
void DebugTool::printEventAsTree( const std::vector<MCParticle*> &event,
                                  Particle2MCAsct::IAsct *assoc )
{
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << std::endl;
  printHeader( log, true, assoc != NULL );

  log.setf(std::ios::fixed,std::ios::floatfield);
  std::vector<MCParticle*>::const_iterator i;
  for( i=event.begin(); i!=event.end(); i++ ) {
    if( ((*i)->originVertex() == NULL) ||
        ((*i)->originVertex()->mother() == NULL) )
      printDecayTree( *i, assoc, "", m_depth, log );
  }
  log << endreq;
}

//=============================================================================
// printEventAsTree (MCParticle) (KeyedContainer)
//=============================================================================
void DebugTool::printEventAsTree( const MCParticles &event )
{
  printEventAsTree( event, NULL );
}

//=============================================================================
// printEventAsTree (MCParticle) (KeyedContainer)
//=============================================================================
void DebugTool::printEventAsTree( const MCParticles &event,
                                  Particle2MCAsct::IAsct *assoc )
{
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << std::endl;
  printHeader( log, true, assoc != NULL );

  log.setf(std::ios::fixed,std::ios::floatfield);
  MCParticles::const_iterator i;
  for( i=event.begin(); i!=event.end(); i++ ) {
    if( ((*i)->originVertex() == NULL) ||
        ((*i)->originVertex()->mother() == NULL) )
      printDecayTree( *i, assoc, "", m_depth, log );
  }
  log << endreq;
}

//=============================================================================
// printDecayTree (Particle)
//=============================================================================
void DebugTool::printDecayTree( const MCParticle *mother, 
                                Particle2MCAsct::IAsct *assoc,
                                const std::string &prefix,
                                int depth, MsgStream &log )
{
  printInfo( prefix, mother, assoc, log );

  if( depth ) {
    SmartRefVector<MCVertex>::const_iterator iv;
    for ( iv = mother->endVertices().begin();
          iv != mother->endVertices().end(); iv++ ) {
      SmartRefVector<MCParticle>::const_iterator idau;
      for ( idau = (*iv)->products().begin();
            idau != (*iv)->products().end(); idau++ ) {
        if ( (*idau == (*iv)->products().back())
             && (*iv == mother->endVertices().back()) )
          printDecayTree( *idau, assoc, prefix+' ', depth-1, log );
        else
          printDecayTree( *idau, assoc, prefix+'|', depth-1, log );
      }
    }
  }
}

//=============================================================================
// printTree (Particle)
//=============================================================================
void DebugTool::printTree( const Particle *mother, int maxDepth = -1 )
{
  printTree( mother, NULL, maxDepth );
}

//=============================================================================
// printTree (Particle)
//=============================================================================
void DebugTool::printTree( const Particle *mother, 
                           Particle2MCAsct::IAsct *assoc,
                           int maxDepth = -1 )
{
  if( maxDepth == -1 )
    maxDepth = m_depth;
  
  MsgStream log(msgSvc(), name());

  if( !mother ) {
    log << MSG::ERROR << "printTree called with NULL Particle" << endreq;
    return;
  }

  log << MSG::INFO << std::endl;
  printHeader( log, false, assoc != NULL );

  log.setf(std::ios::fixed,std::ios::floatfield);
  printDecayTree( mother, assoc, "", maxDepth, log );
  log << endreq;
}

//=============================================================================
// printDecayTree (Particle)
//=============================================================================
void DebugTool::printDecayTree( const Particle *mother, 
                                Particle2MCAsct::IAsct *assoc,
                                const std::string &prefix,
                                int depth, MsgStream &log )
{
  printInfo( prefix, mother, assoc, log );

  if( depth ) {
    if( !mother->endVertex() )
      return;
    SmartRefVector<Particle>::const_iterator iprod;
    for ( iprod = mother->endVertex()->products().begin();
          iprod != mother->endVertex()->products().end(); iprod++ ) {
      if ( (*iprod) != mother->endVertex()->products().back() )
        printDecayTree( *iprod, assoc, prefix+'|', depth-1, log );
      else
        printDecayTree( *iprod, assoc, prefix+' ', depth-1, log );
    }
  }
}

//=============================================================================
// printAncestor (MCParticle)
//=============================================================================
void DebugTool::printAncestor( const MCParticle *child )
{
  MsgStream log( msgSvc(), name() );
  ParticleProperty *p = m_ppSvc->findByStdHepID(child->particleID().pid());
  std::string decay = p ? p->particle() : "N/A";
  const MCVertex *origin = child->originVertex();
  while( origin && (child = origin->mother()) ) {
    p = m_ppSvc->findByStdHepID(child->particleID().pid());
    decay = (p ? p->particle() : std::string("N/A")) + " -> "+ decay;
    origin = child->originVertex();
  }
  log << MSG::INFO << decay << endreq;
}

//=============================================================================
// printEventAsList (Particle)
//=============================================================================
void DebugTool::printEventAsList( const ParticleVector &event )
{
  printEventAsList( event, NULL );
}

//=============================================================================
// printEventAsList (Particle)
//=============================================================================
void DebugTool::printEventAsList( const ParticleVector &event,
                                  Particle2MCAsct::IAsct *assoc )
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << std::endl;
  printHeader( log, false, assoc != NULL );

  ParticleVector::const_iterator i;
  for( i=event.begin(); i!=event.end(); i++ )
    printInfo( "", *i, assoc, log );
  log << endreq;
}

//=============================================================================
// printEventAsList (MCParticle)
//=============================================================================
void DebugTool::printEventAsList( const std::vector<MCParticle*> &event )
{
  printEventAsList( event, NULL );
}

//=============================================================================
// printEventAsList (MCParticle)
//=============================================================================
void DebugTool::printEventAsList( const std::vector<MCParticle*> &event,
                                  Particle2MCAsct::IAsct *assoc )
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << std::endl;
  printHeader( log, true, assoc != NULL );
  
  int c = 0;
  std::vector<MCParticle*>::const_iterator i;
  for( i=event.begin(); i!=event.end(); i++, c++ )
    printInfo( "", *i, assoc, log );
  log << endreq;
}

//=============================================================================
// printEventAsList (Particle) (KeyedContainer)
//=============================================================================
void DebugTool::printEventAsList( const Particles &event )
{
  printEventAsList( event, NULL );
}

//=============================================================================
// printEventAsList (Particle) (KeyedContained)
//=============================================================================
void DebugTool::printEventAsList( const Particles &event,
                                  Particle2MCAsct::IAsct *assoc )
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << std::endl;
  printHeader( log, false, assoc != NULL );

  Particles::const_iterator i;
  for( i=event.begin(); i!=event.end(); i++ )
    printInfo( "", *i, assoc, log );
  log << endreq;
}

//=============================================================================
// printEventAsList (MCParticle) (KeyedContainer)
//=============================================================================
void DebugTool::printEventAsList( const MCParticles &event )
{
  printEventAsList( event, NULL );
}

//=============================================================================
// printEventAsList (MCParticle) (KeyedContainer)
//=============================================================================
void DebugTool::printEventAsList( const MCParticles &event,
                                  Particle2MCAsct::IAsct *assoc )
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << std::endl;
  printHeader( log, true, assoc != NULL );
  
  int c = 0;
  MCParticles::const_iterator i;
  for( i=event.begin(); i!=event.end(); i++, c++ )
    printInfo( "", *i, assoc, log );
  log << endreq;
}

