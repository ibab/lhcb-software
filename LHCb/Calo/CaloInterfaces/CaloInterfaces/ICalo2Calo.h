// $Id: ICalo2Calo.h,v 1.5 2009-08-05 17:29:52 ibelyaev Exp $
// ============================================================================
#ifndef ICALO2CALO_H 
#define ICALO2CALO_H 1
// ============================================================================
// Include files
// ============================================================================
// STL
#include <string>
// ============================================================================
#include "Kernel/CaloCellID.h"
// ============================================================================
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// forward declaration
// ============================================================================
static const InterfaceID IID_ICalo2Calo ( "ICalo2Calo", 3, 0 );
// ============================================================================
/** @class ICalo2Calo ICalo2Calo.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-05-29
 */
class ICalo2Calo : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICalo2Calo; }

  virtual StatusCode initialize()=0;
  virtual void setCalos( const std::string& from , 
                         const std::string& to   ) = 0 ;
  
  virtual const std::vector<LHCb::CaloCellID>& cellIDs
  ( const LHCb::CaloCluster& fromCluster, const std::string& toCalo)=0;
  virtual const std::vector<LHCb::CaloCellID>& cellIDs
  ( const LHCb::CaloCellID&  fromId     , const std::string& toCalo, bool init=true)=0;
  virtual const std::vector<LHCb::CaloCellID>& cellIDs()=0;
  virtual const std::vector<LHCb::CaloDigit*>& digits 
  ( const LHCb::CaloCellID&  fromId     , const std::string& toCalo)=0;
  virtual const std::vector<LHCb::CaloDigit*>& digits 
  ( const LHCb::CaloCluster& fromCluster, const std::string& toCalo)=0;
  virtual const std::vector<LHCb::CaloDigit*>& digits ()=0;
  //
  virtual double energy
  ( const LHCb::CaloCellID&  fromId     , const std::string& toCalo ) =0 ;
  virtual double energy
  ( const LHCb::CaloCluster& fromCluster, const std::string& toCalo)=0;
  virtual double energy()=0;
  virtual int multiplicity
  ( const LHCb::CaloCellID&  fromId      , const std::string& toCalo)=0;
  virtual int multiplicity
  ( const LHCb::CaloCluster& fromCluster , const std::string& toCalo)=0;
  virtual int multiplicity()=0;


  virtual bool isLocalMax( const LHCb::CaloDigit& digit)=0;

protected:

private:

};
#endif // ICALO2CALO_H
