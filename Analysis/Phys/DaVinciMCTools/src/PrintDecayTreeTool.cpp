// $Id: PrintDecayTreeTool.cpp,v 1.4 2009-11-17 08:12:50 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// from LHCb
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// from Event
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/Particle.h"

// from LHCb
#include "GaudiKernel/PhysicalConstants.h"

// local
#include "PrintDecayTreeTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrintDecayTreeTool
//
// 29/03/2001 : Olivier Dormond
//-----------------------------------------------------------------------------

// Declaration of the AlgTool Factory

DECLARE_TOOL_FACTORY( PrintDecayTreeTool )

using namespace Gaudi::Units;

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
PrintDecayTreeTool::PrintDecayTreeTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool( type, name, parent ),
    m_ppSvc(0),
    m_keys(0),
    m_energyUnitName("MeV"),
    m_lengthUnitName("mm")
{

  declareInterface<IPrintDecayTreeTool> (this);
  declareInterface<IPrintDecay>         (this);

  declareProperty( "PrintDepth", m_depth = 999 );
  declareProperty( "TreeWidth", m_treeWidth = 20 );
  declareProperty( "FieldWidth", m_fWidth = 10 );
  declareProperty( "FieldPrecision", m_fPrecision = 2 );
  declareProperty( "Arrow", m_arrow = "+-->" );
  declareProperty( "Information", m_information = "Name E M P Pt phi Vz PK PPK" );
  declareProperty( "EnergyUnit", m_energyUnit = MeV );
  declareProperty( "LengthUnit", m_lengthUnit = mm );
}

//=============================================================================
// initialise
//=============================================================================
StatusCode PrintDecayTreeTool::initialize( void )
{
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;

  if( serviceLocator() ) {
    sc = service("LHCb::ParticlePropertySvc",m_ppSvc);
  }
  if( !m_ppSvc ) {
    throw GaudiException( "LHCb::ParticlePropertySvc not found",
                          "DebugException",
                          StatusCode::FAILURE );
  }

  std::size_t oldpos = 0, pos;
  do {
    pos=m_information.find( ' ', oldpos );
    std::string tok(m_information, oldpos, pos-oldpos);
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
    else if( tok=="chi2" )  m_keys.push_back(chi2);
    else if( tok=="PK" )    m_keys.push_back(PK);
    else if( tok=="PPK" )   m_keys.push_back(PPK);
    else
      err() << "Unknown output key '" << tok << "'. Ignoring it."
            << endmsg;
    if( pos != std::string::npos ) { oldpos = pos+1; }
    else                           { oldpos = pos; }
  }
  while( pos != std::string::npos );

  if (m_energyUnit == TeV) m_energyUnitName = "TeV" ;
  else if (m_energyUnit == GeV) m_energyUnitName = "GeV" ;
  else if (m_energyUnit == MeV) m_energyUnitName = "MeV" ;
  else if (m_energyUnit <= 0) {
    err() << "You have chosen a unit for energies: "
          << m_energyUnit << endmsg;
    return StatusCode::FAILURE ;
  }
  else {
    warning() << "You have chosen a non-standard unit for energies: "
              << m_energyUnit << endmsg;
    m_energyUnitName = "???" ;
  }
  if (m_lengthUnit == mm) m_lengthUnitName = "mm" ;
  else if (m_lengthUnit == cm) m_lengthUnitName = "cm" ;
  else if (m_lengthUnit == m) m_lengthUnitName = "m" ;
  else if (m_lengthUnit <= 0) {
    err() << "You have chosen a unit for lengths: "
          << m_lengthUnit << endmsg;
    return StatusCode::FAILURE ;
  }
  else {
    warning() << "You have chosen a non-standard unit for lengths: "
              << m_lengthUnit << endmsg;
    m_lengthUnitName = "??" ;

  }

  return sc;
}
//=============================================================================
void PrintDecayTreeTool::printHeader( MsgStream& log,
                                      bool mcfirst,
                                      bool associated )
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
  for( std::vector<InfoKeys>::iterator i = m_keys.begin(); i!= m_keys.end(); i++ )
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
  log << endmsg;

  for( std::vector<InfoKeys>::iterator i = m_keys.begin(); i!= m_keys.end(); i++ )
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
    case chi2:      log << std::setw(m_fWidth) << "chi2";      break;
    case PK:        log << std::setw(m_fWidth) << "P(C/K)";    break;
    case PPK:       log << std::setw(m_fWidth) << "PP(C/K)";   break;
    }
  if( associated )
    for( std::vector<InfoKeys>::iterator i = m_keys.begin(); i!= m_keys.end(); i++ )
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
      case chi2:    log << std::setw(m_fWidth) << "chi2";      break;
      case PK:      log << std::setw(m_fWidth) << "Pkey";      break;
      case PPK:     log << std::setw(m_fWidth) << "PPkey";     break;
      }
  log << endmsg;

  for( std::vector<InfoKeys>::iterator i = m_keys.begin(); i!= m_keys.end(); i++ )
    switch( *i ) {
    case Name:      log << std::setw(m_treeWidth) << " ";      break;
    case E:         log << std::setw(m_fWidth) << m_energyUnitName;       break;
    case M:         log << std::setw(m_fWidth) << m_energyUnitName;       break;
    case P:         log << std::setw(m_fWidth) << m_energyUnitName;       break;
    case Pt:        log << std::setw(m_fWidth) << m_energyUnitName;       break;
    case Px:        log << std::setw(m_fWidth) << m_energyUnitName;       break;
    case Py:        log << std::setw(m_fWidth) << m_energyUnitName;       break;
    case Pz:        log << std::setw(m_fWidth) << m_energyUnitName;       break;
    case Vx:        log << std::setw(m_fWidth) << m_lengthUnitName;       break;
    case Vy:        log << std::setw(m_fWidth) << m_lengthUnitName;       break;
    case Vz:        log << std::setw(m_fWidth) << m_lengthUnitName;       break;
    case theta:     log << std::setw(m_fWidth) << "mrad";      break;
    case phi:       log << std::setw(m_fWidth) << "mrad";      break;
    case eta:       log << std::setw(m_fWidth) << "prap";      break;
    case idcl:      log << std::setw(m_fWidth) << " ";         break;
    case chi2:      log << std::setw(m_fWidth) << " ";         break;
    case PK:        log << std::setw(m_fWidth) << " ";         break;
    case PPK:       log << std::setw(m_fWidth) << " ";         break;
    }
  if( associated )
    for( std::vector<InfoKeys>::iterator i = m_keys.begin(); i!= m_keys.end(); i++ )
      switch( *i ) {
      case Name:    log << std::setw(m_fWidth) << " ";         break;
      case E:       log << std::setw(m_fWidth) << m_energyUnitName;       break;
      case M:       log << std::setw(m_fWidth) << m_energyUnitName;       break;
      case P:       log << std::setw(m_fWidth) << m_energyUnitName;       break;
      case Pt:      log << std::setw(m_fWidth) << m_energyUnitName;       break;
      case Px:      log << std::setw(m_fWidth) << m_energyUnitName;       break;
      case Py:      log << std::setw(m_fWidth) << m_energyUnitName;       break;
      case Pz:      log << std::setw(m_fWidth) << m_energyUnitName;       break;
      case Vx:      log << std::setw(m_fWidth) << m_lengthUnitName;       break;
      case Vy:      log << std::setw(m_fWidth) << m_lengthUnitName;       break;
      case Vz:      log << std::setw(m_fWidth) << m_lengthUnitName;       break;
      case theta:   log << std::setw(m_fWidth) << "mrad";      break;
      case phi:     log << std::setw(m_fWidth) << "mrad";      break;
      case eta:     log << std::setw(m_fWidth) << "prap";      break;
      case idcl:    log << std::setw(m_fWidth) << " ";         break;
      case chi2:    log << std::setw(m_fWidth) << " ";         break;
      case PK:      log << std::setw(m_fWidth) << " ";         break;
      case PPK:     log << std::setw(m_fWidth) << " ";         break;
      }
  log << endmsg;
}
//=============================================================================
void PrintDecayTreeTool::printInfo( const std::string& prefix,
                                    const LHCb::MCParticle* part,
                                    Particle2MCLinker* assoc,
                                    MsgStream& log )
{
  const LHCb::ParticleProperty* p = m_ppSvc->find( part->particleID() );
  const LHCb::MCVertex *origin    = part->originVertex();

  m_usedTesLocs.insert( tesLocation(part) );

  for( std::vector<InfoKeys>::iterator i = m_keys.begin(); i!= m_keys.end(); i++ )
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
          << part->momentum().e()/m_energyUnit;
      break;
    case M:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->momentum().M()/m_energyUnit;
      break;
    case P:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->momentum().Vect().R()/m_energyUnit;
      break;
    case Pt:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->momentum().Pt()/m_energyUnit;
      break;
    case Px:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->momentum().px()/m_energyUnit;
      break;
    case Py:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->momentum().py()/m_energyUnit;
      break;
    case Pz:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->momentum().pz()/m_energyUnit;
      break;
    case Vx:
      if( origin )
        log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
            << origin->position().x()/m_lengthUnit;
      else
        log << std::setw(m_fWidth) << " N/A ";
      break;
    case Vy:
      if( origin )
        log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
            << origin->position().y()/m_lengthUnit;
      else
        log << std::setw(m_fWidth) << " N/A ";
      break;
    case Vz:
      if( origin )
        log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
            << origin->position().z()/m_lengthUnit;
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
          << part->momentum().Eta();
      break;
    case idcl:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << 1.0;
      break;
    case chi2:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << 1.0;
      break;
    default:
      break;
    }

  if( assoc )
  {
    const LHCb::Particle* reco = assoc->firstP( part );
    if( reco )
    {
      const std::string pLoc = tesLocation(reco);
      const std::string ppLoc = tesLocation(reco->proto());
      std::ostringstream mess;
      for( std::vector<InfoKeys>::iterator i = m_keys.begin(); i!= m_keys.end(); ++i )
      {
        switch( *i ) {
        case Name:
          {
            p = m_ppSvc->find( reco->particleID() );
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
              << reco->momentum().e()/m_energyUnit;
          break;
        case M:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->momentum().M()/m_energyUnit;
          break;
        case P:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->momentum().Vect().R()/m_energyUnit;
          break;
        case Pt:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->momentum().Pt()/m_energyUnit;
          break;
        case Px:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->momentum().px()/m_energyUnit;
          break;
        case Py:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->momentum().py()/m_energyUnit;
          break;
        case Pz:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->momentum().pz()/m_energyUnit;
          break;
        case Vx:
          if( origin )
            log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
                << reco->referencePoint().x()/m_lengthUnit;
          else
            log << std::setw(m_fWidth) << " N/A ";
          break;
        case Vy:
          if( origin )
            log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
                << reco->referencePoint().y()/m_lengthUnit;
          else
            log << std::setw(m_fWidth) << " N/A ";
          break;
        case Vz:
          if( origin )
            log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
                << reco->referencePoint().z()/m_lengthUnit;
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
              << reco->momentum().Eta();
          break;
        case idcl:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->confLevel();
          break;
        case chi2:
          if ( 0!=reco->proto() ){
            if ( 0!=reco->proto()->track()) {
              log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
                  << reco->proto()->track()->chi2PerDoF() ;
            } else { log << std::setw(m_fWidth) << std::setprecision(m_fPrecision) << -1.; }
          } else if ( 0!=reco->endVertex() ){
            log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
                << reco->endVertex()->chi2()/reco->endVertex()->nDoF()  ;
          }
          else { log << std::setw(m_fWidth) << std::setprecision(m_fPrecision) << -2; }
          break;
        case PK:
          m_usedTesLocs.insert( pLoc );
          mess.str("");
          mess << tesCode(pLoc) << "/" << reco->key();
          log << boost::format("%"+boost::lexical_cast<std::string>(m_fWidth)+"s") % mess.str();
          break;
        case PPK:
          if ( reco->proto() )
          {
            m_usedTesLocs.insert(ppLoc);
            mess.str("");
            mess << tesCode(ppLoc) << "/" << reco->proto()->key();
            log << boost::format("%"+boost::lexical_cast<std::string>(m_fWidth)+"s") % mess.str();
          }
          else { log << boost::format("%"+boost::lexical_cast<std::string>(m_fWidth)+"s") % "N/A"; }
          break;
        default:
          break;
        }
      }
    }
    else
    {
      log << "  No associated particle";
    }
  }
  log << endmsg;
}
//=============================================================================
void PrintDecayTreeTool::printInfo( const std::string& prefix,
                                    const LHCb::Particle* reco,
                                    Particle2MCLinker* assoc,
                                    MsgStream& log )
{
  // get Particle info
  const LHCb::ParticleProperty* p = m_ppSvc->find( reco->particleID() );

  const std::string pLoc = tesLocation(reco);
  const std::string ppLoc = tesLocation(reco->proto());
  std::ostringstream mess;

  for( std::vector<InfoKeys>::iterator i = m_keys.begin(); i != m_keys.end(); ++i )
  {
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
          << reco->momentum().e()/m_energyUnit;
      break;
    case M:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->momentum().M()/m_energyUnit;
      break;
    case P:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->momentum().Vect().R()/m_energyUnit;
      break;
    case Pt:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->momentum().Pt()/m_energyUnit;
      break;
    case Px:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->momentum().px()/m_energyUnit;
      break;
    case Py:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->momentum().py()/m_energyUnit;
      break;
    case Pz:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->momentum().pz()/m_energyUnit;
      break;
    case Vx:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->referencePoint().x()/m_lengthUnit;
      break;
    case Vy:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->referencePoint().y()/m_lengthUnit;
      break;
    case Vz:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->referencePoint().z()/m_lengthUnit;
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
          << reco->momentum().Eta();
      break;
    case idcl:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << reco->confLevel();
      break;
    case chi2:
      if ( 0!=reco->proto() ){
        if ( 0!=reco->proto()->track()) {
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << reco->proto()->track()->chi2PerDoF() ;
        } else log << std::setw(m_fWidth) << std::setprecision(m_fPrecision) << -1. ;
      } else if ( 0!=reco->endVertex() ){
        log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
            << reco->endVertex()->chi2()/reco->endVertex()->nDoF()  ;
      }
      else log << std::setw(m_fWidth) << std::setprecision(m_fPrecision) << -2  ;
      break;
    case PK:
      m_usedTesLocs.insert( pLoc );
      mess.str("");
      mess << tesCode(pLoc) << "/" << reco->key();
      log << boost::format("%"+boost::lexical_cast<std::string>(m_fWidth)+"s") % mess.str();
      break;
    case PPK:
      if ( reco->proto() )
      {
        m_usedTesLocs.insert(ppLoc);
        mess.str("");
        mess << tesCode(ppLoc) << "/" << reco->proto()->key();
        log << boost::format("%"+boost::lexical_cast<std::string>(m_fWidth)+"s") % mess.str();
      }
      else { log << boost::format("%"+boost::lexical_cast<std::string>(m_fWidth)+"s") % "N/A"; }
      break;
    default:
      break;
    }
  }

  if( assoc )
  {
    const LHCb::MCParticle* part = assoc->firstMCP( reco );
    if( part )
    {
      for( std::vector<InfoKeys>::iterator i = m_keys.begin(); i!= m_keys.end(); ++i )
      {
        switch( *i ) {
        case Name:
          {
            p = m_ppSvc->find( reco->particleID() );
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
              << part->momentum().e()/m_energyUnit;
          break;
        case M:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << part->momentum().M()/m_energyUnit;
          break;
        case P:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << part->momentum().Vect().R()/m_energyUnit;
          break;
        case Pt:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << part->momentum().Pt()/m_energyUnit;
          break;
        case Px:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << part->momentum().px()/m_energyUnit;
          break;
        case Py:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << part->momentum().py()/m_energyUnit;
          break;
        case Pz:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << part->momentum().pz()/m_energyUnit;
          break;
        case Vx:
          {
            const LHCb::MCVertex *origin = part->originVertex();
            if( origin )
              log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
                  << origin->position().x()/m_lengthUnit;
            else
              log << std::setw(m_fWidth) << " N/A ";
          }
          break;
        case Vy:
          {
            const LHCb::MCVertex *origin = part->originVertex();
            if( origin ){
              log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
                  << origin->position().y()/m_lengthUnit;
            } else{
              log << std::setw(m_fWidth) << " N/A ";
            }
          }
          break;
        case Vz:
          {
            const LHCb::MCVertex *origin = part->originVertex();
            if( origin ){
              log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
                  << origin->position().z()/m_lengthUnit;
            } else {
              log << std::setw(m_fWidth) << " N/A ";
            }
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
              << part->momentum().Eta();
          break;
        case idcl:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << 1.0;
          break;
        case chi2:
          log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
              << 1.0;
          break;
        default:
          break;
        }
      }
    }
    else
    {
      log << "  No associated particle";
    }
  }
  log << endmsg;
}
//=============================================================================
void PrintDecayTreeTool::printTree( const LHCb::MCParticle *mother,
                                    Particle2MCLinker* assoc,
                                    int maxDepth )
{
  if ( maxDepth == -1 ) { maxDepth = m_depth; }

  if ( !mother )
  {
    Error( "printTree called with NULL MCParticle" ).ignore();
    return;
  }

  m_usedTesLocs.clear();

  printHeader( info(), true, assoc != NULL );

  info().setf(std::ios::fixed,std::ios::floatfield);
  printDecayTree( mother, assoc, "", maxDepth, info() );
  info() << endmsg;
  printUsedContainers( info() );
  info() << endmsg;
}
//=============================================================================
void PrintDecayTreeTool::printAsTree( const LHCb::MCParticle::ConstVector &particles,
                                      Particle2MCLinker* assoc )
{
  printHeader( info(), true, assoc != NULL );

  info().setf(std::ios::fixed,std::ios::floatfield);
  for ( LHCb::MCParticle::ConstVector::const_iterator i = particles.begin();
        i != particles.end(); ++i )
  {
    if( ((*i)->originVertex() == NULL) ||
        ((*i)->originVertex()->mother() == NULL) )
      printDecayTree( *i, assoc, "", m_depth, info() );
  }
  info() << endmsg;
}
//=============================================================================
void PrintDecayTreeTool::printAsTree( const LHCb::MCParticles &particles,
                                      Particle2MCLinker* assoc )
{
  printHeader( info(), true, assoc != NULL );

  info().setf(std::ios::fixed,std::ios::floatfield);
  for( LHCb::MCParticles::const_iterator i = particles.begin();
       i != particles.end(); ++i )
  {
    if( ((*i)->originVertex() == NULL) ||
        ((*i)->originVertex()->mother() == NULL) )
      printDecayTree( *i, assoc, "", m_depth, info() );
  }
  info() << endmsg;
}
//=============================================================================
void PrintDecayTreeTool::printDecayTree( const LHCb::MCParticle* mother,
                                         Particle2MCLinker* assoc,
                                         const std::string& prefix,
                                         int depth,
                                         MsgStream &log )
{
  m_usedTesLocs.insert( tesLocation(mother) );

  printInfo( prefix, mother, assoc, log );

  if( depth )
  {
    for ( SmartRefVector<LHCb::MCVertex>::const_iterator iv = mother->endVertices().begin();
          iv != mother->endVertices().end(); ++iv )
    {
      for ( SmartRefVector<LHCb::MCParticle>::const_iterator idau = (*iv)->products().begin();
            idau != (*iv)->products().end(); ++idau )
      {
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
void PrintDecayTreeTool::printTree( const LHCb::Particle* mother,
                                    int maxDepth )
{
  printTree( mother, NULL, maxDepth );
}
//=============================================================================
void PrintDecayTreeTool::printTree( const LHCb::Particle *mother,
                                    Particle2MCLinker* assoc,
                                    int maxDepth  )
{
  if ( maxDepth == -1 ) { maxDepth = m_depth; }

  if( !mother )
  {
    err() << "printTree called with NULL Particle" << endmsg;
    return;
  }

  m_usedTesLocs.clear();

  printHeader( info(), false, assoc != NULL );

  info().setf(std::ios::fixed,std::ios::floatfield);
  printDecayTree( mother, assoc, "", maxDepth, info() );
  info() << endmsg;
  printUsedContainers( info() );
  info() << endmsg;
}
//=============================================================================
void PrintDecayTreeTool::printDecayTree( const LHCb::Particle *mother,
                                         Particle2MCLinker* assoc,
                                         const std::string &prefix,
                                         int depth, MsgStream &log )
{
  printInfo( prefix, mother, assoc, log );

  if( depth )
  {
    if( !mother->endVertex() )
      return;
    for ( SmartRefVector<LHCb::Particle>::const_iterator iprod = mother->daughters().begin();
          iprod != mother->daughters().end(); iprod++ )
    {
      if ( (*iprod) != mother->daughters().back() )
        printDecayTree( *iprod, assoc, prefix+'|', depth-1, log );
      else
        printDecayTree( *iprod, assoc, prefix+' ', depth-1, log );
    }
  }
}
//=============================================================================
void PrintDecayTreeTool::printAsList( const LHCb::Particle::ConstVector& particles )
{
  printAsList( particles, NULL );
}
//=============================================================================
void PrintDecayTreeTool::printAsList( const LHCb::Particle::ConstVector& particles,
                                      Particle2MCLinker* assoc )
{
  printHeader( info(), false, assoc != NULL );

  for( LHCb::Particle::ConstVector::const_iterator i = particles.begin();
       i != particles.end(); ++i )
  {
    printInfo( "", *i, assoc, info() );
  }
  info() << endmsg;
}
//=============================================================================
void PrintDecayTreeTool::printAsList( const LHCb::MCParticle::ConstVector& particles,
                                      Particle2MCLinker* assoc )
{
  printHeader( info(), true, assoc != NULL );

  int c = 0;
  for( LHCb::MCParticle::ConstVector::const_iterator i = particles.begin();
       i != particles.end(); i++, c++ )
  {
    printInfo( "", *i, assoc, info() );
  }
  info() << endmsg;
}
//=============================================================================
void PrintDecayTreeTool::printAsList( const LHCb::Particles& particles )
{
  printAsList( particles, NULL );
}
//=============================================================================
void PrintDecayTreeTool::printAsList( const LHCb::Particles& particles,
                                      Particle2MCLinker* assoc )
{
  printHeader( info(), false, assoc != NULL );

  for( LHCb::Particles::const_iterator i = particles.begin();
       i != particles.end(); ++i )
  {
    printInfo( "", *i, assoc, info() );
  }
  info() << endmsg;
}
//=============================================================================
void PrintDecayTreeTool::printAsList( const LHCb::MCParticles& particles,
                                      Particle2MCLinker* assoc )
{
  printHeader( info(), true, assoc != NULL );

  int c = 0;
  for ( LHCb::MCParticles::const_iterator i = particles.begin();
        i != particles.end(); ++i, ++c )
  {
    printInfo( "", *i, assoc, info() );
  }
  info() << endmsg;
}
//=============================================================================
void PrintDecayTreeTool::printUsedContainers( MsgStream &log )
{
  log << "Used TES locations :-" << endmsg;
  for ( std::set<std::string>::const_iterator i = m_usedTesLocs.begin();
        i != m_usedTesLocs.end(); ++i )
  {
    log << " " << boost::format("%6u") % tesCode(*i) << " = '" << *i << "'" << endmsg;
  }
  m_usedTesLocs.clear();
}
//=============================================================================
