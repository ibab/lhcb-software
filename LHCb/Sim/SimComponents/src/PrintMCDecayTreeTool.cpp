// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PhysicalConstants.h"

// From PartProp
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// from Event
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

// local
#include "PrintMCDecayTreeTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrintMCDecayTreeTool
//
// 29/03/2001 : Olivier Dormond
//-----------------------------------------------------------------------------

// Declaration of the AlgTool Factory

DECLARE_TOOL_FACTORY( PrintMCDecayTreeTool )

using namespace Gaudi::Units;

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
PrintMCDecayTreeTool::PrintMCDecayTreeTool( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : GaudiTool( type, name, parent ), m_ppSvc(0), m_keys(0), 
    m_energyUnitName("MeV"), m_lengthUnitName("mm")
{

  declareInterface<IPrintMCDecayTreeTool>(this);

  declareProperty( "PrintDepth", m_depth = 999 );
  declareProperty( "TreeWidth", m_treeWidth = 20 );
  declareProperty( "FieldWidth", m_fWidth = 10 );
  declareProperty( "FieldPrecision", m_fPrecision = 2 );
  declareProperty( "Arrow", m_arrow = "+-->" );
  declareProperty( "Informations", m_informationsDeprecated = "" );
  declareProperty( "Information", m_information = "Name E M P Pt phi Vz" );
  declareProperty( "EnergyUnit", m_energyUnit = MeV );
  declareProperty( "LengthUnit", m_lengthUnit = mm );
}

//=============================================================================
// initialise
//=============================================================================
StatusCode PrintMCDecayTreeTool::initialize( void ){

  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;

  m_ppSvc = svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc",true);

  if ( m_informationsDeprecated != "" ){
    warning() << "You are using the deprecated option ``informations''." << endmsg ;
    warning() << "Use ``Information'' instead." << endmsg ;
    m_information = m_informationsDeprecated ;
  }
  

  std::size_t oldpos = 0, pos;
  do {
    pos=m_information.find( ' ', oldpos );
    std::string tok(m_information, oldpos, pos-oldpos);
    if( tok=="Name" )       m_keys.push_back(Name);
    else if( tok=="PID" )   m_keys.push_back(PID);
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
    else if( tok=="flags" ) m_keys.push_back(flags);
    else if( tok=="fromSignal") m_keys.push_back(fromSignal);
    else if( tok=="IDCL" )  m_keys.push_back(idcl);
    else
      err() << "Unknown output key '" << tok << "'. Ignoring it."
            << endmsg;
    if( pos != std::string::npos ) oldpos = pos+1;
    else                           oldpos = pos;
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

  return StatusCode::SUCCESS;
}

//=============================================================================
// printHeader
//=============================================================================
void PrintMCDecayTreeTool::printHeader( MsgStream& log )
{
  static const std::string mctitle = " MCParticle ";
  const std::string* title;
  title = &mctitle;

  bool name_key = false;
  std::vector<InfoKeys>::iterator i;
  for( i = m_keys.begin(); i!= m_keys.end(); i++ ) {
    if( *i == Name ) name_key = true;
  }

  int n_keys = m_keys.size() - (name_key ? 1 : 0);
  int width = n_keys*m_fWidth + (name_key ? m_treeWidth : 0);
  int left  = (width - title->length() - 2 )/2;
  int right =  width - title->length() - 2 - left;

  if( left >= 0 )
    log << '<' << std::string(left,'-');
  log << *title;
  if( right >= 0 )
    log<< std::string(right,'-') << '>';

  log << std::endl;

  for( i = m_keys.begin(); i!= m_keys.end(); i++ )
    switch( *i ) {
    case Name:      log << std::setw(m_treeWidth) << "Name";   break;
    case PID:       log << std::setw(m_fWidth) << "PID";         break;
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
    case flags:     log << std::setw(m_fWidth) << "Flags ";    break;
    case fromSignal:  log <<  std::setw(m_fWidth) << "fromSignal";     break; 
    case idcl:      log << std::setw(m_fWidth) << "ID CL";     break;
    }

  log << std::endl;

  for( i = m_keys.begin(); i!= m_keys.end(); i++ )
    switch( *i ) {
    case Name:      log << std::setw(m_treeWidth) << " ";      break;
    case PID:       log << std::setw(m_fWidth)    << " ";       break;
    case E:         log << std::setw(m_fWidth) << m_energyUnitName;       break;
    case M:         log << std::setw(m_fWidth) << m_energyUnitName;       break;
    case P:         log << std::setw(m_fWidth) << m_energyUnitName;       break;
    case Pt:        log << std::setw(m_fWidth) << m_energyUnitName;       break;
    case Px:        log << std::setw(m_fWidth) << m_energyUnitName;       break;
    case Py:        log << std::setw(m_fWidth) << m_energyUnitName;       break;
    case Pz:        log << std::setw(m_fWidth) << m_energyUnitName;       break;
    case Vx:        log << std::setw(m_fWidth) << m_lengthUnitName;        break;
    case Vy:        log << std::setw(m_fWidth) << m_lengthUnitName;        break;
    case Vz:        log << std::setw(m_fWidth) << m_lengthUnitName;        break;
    case theta:     log << std::setw(m_fWidth) << "mrad";      break;
    case phi:       log << std::setw(m_fWidth) << "mrad";      break;
    case eta:       log << std::setw(m_fWidth) << "prap";      break;
    case flags:     log << std::setw(m_fWidth) << " ";     break;
    case fromSignal:  log <<  std::setw(m_fWidth) << " ";     break; 
    case idcl:      log << std::setw(m_fWidth) << " ";         break;
    }

  log << std::endl;
}
//=============================================================================
// printInfo (MCParticle)
//=============================================================================
void PrintMCDecayTreeTool::printInfo( const std::string &prefix, 
                                      const LHCb::MCParticle *part,
                                      MsgStream &log )
{
  const LHCb::ParticleProperty* p = m_ppSvc->find( part->particleID() );
  const LHCb::MCVertex *origin = part->originVertex();

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
    case PID:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->particleID().pid() ;
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
    case flags:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->flags();
      break;
    case fromSignal:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << part->fromSignal();
      break;
    case idcl:
      log << std::setw(m_fWidth) << std::setprecision(m_fPrecision)
          << 1.0;
      break;
    default:
      break;
    }

  log << std::endl;
}
//=============================================================================
// Print decay tree for a given MCparticle
//=============================================================================
void PrintMCDecayTreeTool::printTree( const LHCb::MCParticle* mother, 
                                      int maxDepth )
{
  if( maxDepth == -1 )
    maxDepth = m_depth;
  
  MsgStream log(msgSvc(), name());

  if( !mother ) {
    err() << "printTree called with NULL MCParticle" << endmsg;
    return;
  }

  log << MSG::INFO << std::endl;
  printHeader( log );

  log.setf(std::ios::fixed,std::ios::floatfield);
  printDecayTree( mother, "", maxDepth, log );
  log << endmsg;
}
//=============================================================================
// printAncestor (MCParticle)
//=============================================================================
void PrintMCDecayTreeTool::printAncestor( const LHCb::MCParticle *child )
{
  const LHCb::ParticleProperty *p = m_ppSvc->find(child->particleID());
  std::string decay = p ? p->particle() : "N/A";
  const LHCb::MCVertex *origin = child->originVertex();
  while( origin && (child = origin->mother()) ) {
    p = m_ppSvc->find(child->particleID());
    decay = (p ? p->particle() : std::string("N/A")) + " -> "+ decay;
    origin = child->originVertex();
  }
  info() << decay << endmsg;
}
//=============================================================================
// printAsTree (MCParticle)
//=============================================================================
void PrintMCDecayTreeTool::printAsTree( const LHCb::MCParticle::ConstVector &event)
{
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << std::endl;
  printHeader( log );

  log.setf(std::ios::fixed,std::ios::floatfield);
  LHCb::MCParticle::ConstVector::const_iterator i;
  for( i=event.begin(); i!=event.end(); i++ ) {
    if( ((*i)->originVertex() == NULL) ||
        ((*i)->originVertex()->mother() == NULL) )
      printDecayTree( *i, "", m_depth, log );
  }
  log << endmsg;
}
//=============================================================================
// printAsTree (MCParticle) (KeyedContainer)
//=============================================================================
void PrintMCDecayTreeTool::printAsTree( const LHCb::MCParticles& event )
{
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << std::endl;
  printHeader( log );

  log.setf(std::ios::fixed,std::ios::floatfield);
  LHCb::MCParticles::const_iterator i;
  for( i=event.begin(); i!=event.end(); i++ ) {
    if( ((*i)->originVertex() == NULL) ||
        ((*i)->originVertex()->mother() == NULL) )
      printDecayTree( *i, "", m_depth, log );
  }
  log << endmsg;
}
//=============================================================================
// printDecayTree (MCParticle)
//=============================================================================
void PrintMCDecayTreeTool::printDecayTree( const LHCb::MCParticle *mother, 
                                           const std::string &prefix,
                                           int depth,
                                           MsgStream &log )
{
  printInfo( prefix, mother, log );

  if( depth ) {
    SmartRefVector<LHCb::MCVertex>::const_iterator iv;
    for ( iv = mother->endVertices().begin();
          iv != mother->endVertices().end(); iv++ ) {
      SmartRefVector<LHCb::MCParticle>::const_iterator idau;
      for ( idau = (*iv)->products().begin();
            idau != (*iv)->products().end(); idau++ ) {
        if ( (*idau == (*iv)->products().back())
             && (*iv == mother->endVertices().back()) )
          printDecayTree( *idau, prefix+' ', depth-1, log );
        else
          printDecayTree( *idau, prefix+'|', depth-1, log );
      }
    }
  }
}
//=============================================================================
// printAsList (MCParticle)
//=============================================================================
void PrintMCDecayTreeTool::printAsList( const LHCb::MCParticle::ConstVector &event)
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << std::endl;
  printHeader( log );
  
  int c = 0;
  LHCb::MCParticle::ConstVector::const_iterator i;
  for( i=event.begin(); i!=event.end(); i++, c++ )
    printInfo( "", *i, log );
  log << endmsg;
}
//=============================================================================
// printAsList (MCParticle) (KeyedContainer)
//=============================================================================
void PrintMCDecayTreeTool::printAsList( const LHCb::MCParticles &event)
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << std::endl;
  printHeader( log );
  
  int c = 0;
  LHCb::MCParticles::const_iterator i;
  for( i=event.begin(); i!=event.end(); i++, c++ )
    printInfo( "", *i, log );
  log << endmsg;
}
//=============================================================================
//=============================================================================
