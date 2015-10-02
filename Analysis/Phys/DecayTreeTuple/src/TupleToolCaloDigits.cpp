// $Id: TupleToolCaloDigits.cpp,v 0.1 2015-08-05 13:50:45 bursche $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolCaloDigits.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/ITupleTool.h"
//#include "GaudiKernel/Memory.h"
//nclude "GaudiAlg/ISequencerTimerTool.h"

#include "Event/CaloDigit.h"

#include "GaudiKernel/IRegistry.h" // IOpaqueAddress

#include  "CaloDet/DeCalorimeter.h"

//#include "TArrayL.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolCaloDigits
//
// 2015-08-05 Albert Bursche
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolCaloDigits )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolCaloDigits::TupleToolCaloDigits( const std::string& type,
				  const std::string& name,
				  const IInterface* parent )
  : TupleToolBase ( type, name , parent )
{
  declareInterface<IEventTupleTool>(this);
  declareProperty( "DigitLocation" ,  m_DigitLocation);
  declareProperty( "CaloName" , m_CaloName);
  declareProperty( "CaloLocation", m_CaloLocation);
  declareProperty( "MaxDigits", m_maxSize);
  declareProperty( "AutoConfigure",  m_auto_configure);
  if (m_auto_configure)
    {
      if (m_CaloName == "Spd")
	{
	  m_CaloLocation = DeCalorimeterLocation::Spd;
	  m_DigitLocation =  LHCb::CaloDigitLocation::Spd;
	}
       if (m_CaloName == "Prs")
	{
	  m_CaloLocation =DeCalorimeterLocation::Prs;
	  m_DigitLocation =  LHCb::CaloDigitLocation::Prs;
	}
      if (m_CaloName == "Ecal")
	{
	  m_CaloLocation = DeCalorimeterLocation::Ecal;
	  m_DigitLocation =  LHCb::CaloDigitLocation::Ecal;
	}
      if (m_CaloName == "Hcal")
	{
	  m_CaloLocation = DeCalorimeterLocation::Hcal;
	  m_DigitLocation =  LHCb::CaloDigitLocation::Hcal;
	}
      // Feel free to add more locations from Event/CaloDigit.h
      fatal() << "Invalid CaloName. Mus be one of  Spd,Prs,Ecal,Hcal if AutoConfigure is used (i.e. true)" <<endmsg;
      
    }
}
//=============================================================================

StatusCode TupleToolCaloDigits::initialize()
{
  const StatusCode sc = TupleToolBase::initialize(); 
 
  debug() << "Reserving buffers for " <<  m_maxSize << " digits." << endmsg;
  m_index =  new unsigned int[m_maxSize];
  
  m_calo =   new unsigned char[m_maxSize];
  m_area =   new unsigned char[m_maxSize];
  m_row =    new unsigned char[m_maxSize];
  m_column = new unsigned char[m_maxSize];
  
  m_es =     new float[m_maxSize];
  m_xs =     new float[m_maxSize];
  m_ys =     new float[m_maxSize];
  m_zs =     new float[m_maxSize];
      
  return sc ;
}

//=============================================================================

StatusCode TupleToolCaloDigits::fill( Tuples::Tuple& tuple )
{
  const std::string prefix=fullName();
  bool test = true;
  LHCb::CaloDigits * digits = getIfExists<LHCb::CaloDigits>(m_DigitLocation);
  unsigned long n = 0;
  if (!digits)
    return true;
  n= digits->size();
  /*
  if (m_maxSize<n) // increase buffers
    {
      if (m_maxSize) // no "new" on Constructor
	{
	  delete m_index;
	  delete m_calo;
	  delete m_area;
	  delete m_row;
	  delete m_column;
	  delete m_es;
	  delete m_xs;
	  delete m_ys;
	}

      m_maxSize = n+4096; // avoid increasing too often
      debug() << "increase buffers to " <<  m_maxSize << endmsg;
      m_index =  new unsigned int[m_maxSize];

      m_calo =   new unsigned char[m_maxSize];
      m_area =   new unsigned char[m_maxSize];
      m_row =    new unsigned char[m_maxSize];
      m_column = new unsigned char[m_maxSize];

      m_es =     new float[m_maxSize];
      m_xs =     new float[m_maxSize];
      m_ys =     new float[m_maxSize];
      
      //  m_CaloName = (*digits->begin())->cellID().caloName();
      }*/
  unsigned long i = 0;
  for( LHCb::CaloDigits::const_iterator digit = digits->begin();
       digit!=digits->end();++digit)
    {
      const LHCb::CaloCellID cellID = (*digit)->cellID();
      m_index[i] = cellID.index();
      m_calo[i]  = cellID.calo() ;
      m_area[i]  = cellID.area();
      m_row[i]   = cellID.row();
      m_column[i]= cellID.col();

      DeCalorimeter* m_calo = getDet<DeCalorimeter>( m_CaloLocation );    
      m_xs[i] = m_calo->cellCenter(cellID).X();
      m_ys[i] = m_calo->cellCenter(cellID).Y();
      m_zs[i] = m_calo->cellCenter(cellID).Z();
      
      m_es[i] = (*digit)->e();
      i++;
      
      if ( msgLevel(MSG::DEBUG) ) debug() << cellID.toString() << " has an energy of " << m_es[i] << " \n";
    }
  if ( msgLevel(MSG::DEBUG) ) debug() << " saved " << i <<" digits to n tuple "+m_CaloName + "Digit." << endmsg;
  if (n>m_maxSize)
    {
      n=m_maxSize;
      warning() << "Limited output to " << m_maxSize << " digits." << endmsg;
    }
  tuple->farray( m_extraName + m_CaloName + "DigitIndex"  ,m_index, m_index+n, m_extraName + m_CaloName + "DigitNi",m_maxSize);
  tuple->farray( m_extraName + m_CaloName + "DigitCalo"   ,m_calo,  m_calo+n,  m_extraName + m_CaloName + "DigitNc",m_maxSize);
  tuple->farray( m_extraName + m_CaloName + "DigitArea"   ,m_area,  m_area+n,  m_extraName + m_CaloName + "DigitNa",m_maxSize);
  tuple->farray( m_extraName + m_CaloName + "DigitRow"    ,m_row,   m_row+n,   m_extraName + m_CaloName + "DigitNr",m_maxSize);
  tuple->farray( m_extraName + m_CaloName + "DigitColumn" ,m_column,m_column+n,m_extraName + m_CaloName + "DigitNco",m_maxSize);
  tuple->farray( m_extraName + m_CaloName + "Digit_X"     ,m_xs,    m_xs+n,    m_extraName + m_CaloName + "DigitNx",m_maxSize);
  tuple->farray( m_extraName + m_CaloName + "Digit_Y"     ,m_ys,    m_ys+n,    m_extraName + m_CaloName + "DigitNy",m_maxSize);    
  tuple->farray( m_extraName + m_CaloName + "Digit_Z"     ,m_zs,    m_zs+n,    m_extraName + m_CaloName + "DigitNz",m_maxSize);    
  tuple->farray( m_extraName + m_CaloName + "DigitEnergy" ,m_es,    m_es+n,    m_extraName + m_CaloName + "DigitNe",m_maxSize);

  return StatusCode(test);
}
