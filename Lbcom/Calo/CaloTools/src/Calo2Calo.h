// $Id: Calo2Calo.h,v 1.6 2009-08-05 17:41:46 ibelyaev Exp $
#ifndef CALO2CALO_H 
#define CALO2CALO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloInterfaces/ICalo2Calo.h"            // Interface
#include "CaloDet/DeCalorimeter.h"
#include  "CaloInterfaces/ICaloGetterTool.h"
/** @class Calo2Calo Calo2Calo.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-05-29
 */

class Calo2Calo : public GaudiTool, virtual public ICalo2Calo {
public: 
  /// Standard constructor
  Calo2Calo( const std::string& type, 
             const std::string& name,
             const IInterface* parent);

  virtual ~Calo2Calo( ); ///< Destructor
  StatusCode initialize();
  // setting
  void setCalos ( const std::string& fromCalo , 
                  const std::string& toCalo   ) ;
  // CaloCellIDs
  const std::vector<LHCb::CaloCellID>& 
  cellIDs ( const LHCb::CaloCluster& fromCluster , 
            const std::string&       toCalo      ) ;
  const std::vector<LHCb::CaloCellID>& 
  cellIDs ( const LHCb::CaloCellID&  fromId     , 
            const std::string&       toCalo, bool init=true);
  const std::vector<LHCb::CaloCellID>& cellIDs(){return m_cells;};
  // Digits
  const std::vector<LHCb::CaloDigit*>& digits
  ( const LHCb::CaloCellID& fromId     , 
    const std::string&      toCalo     ) ;
  const std::vector<LHCb::CaloDigit*>& digits
  ( const LHCb::CaloCluster& fromCluster , 
    const std::string&       toCalo    ) ;
  const std::vector<LHCb::CaloDigit*>& digits(){return m_digits;};
  // Energy
  double energy ( const LHCb::CaloCellID&  fromId , 
                  const std::string&       toCalo );
  double energy ( const LHCb::CaloCluster& fromCluster ,
                  const std::string&       toCalo );
  double energy (){return m_energy;};
  // multiplicity
  int multiplicity( const LHCb::CaloCellID&  fromId ,
                    const std::string&       toCalo ) ;
  int multiplicity( const LHCb::CaloCluster& fromCluster ,
                    const std::string&       toCalo ) ;
  int multiplicity(){return m_count;};
  // Additional
  bool isLocalMax ( const LHCb::CaloDigit& digit);

protected:
  // 
  void reset();
  const std::vector<LHCb::CaloCellID>& addCell
  ( const LHCb::CaloCellID& id, const std::string& toCalo);
  // Calo Maps
  std::map<std::string,DeCalorimeter*> m_det;
  std::map<std::string,std::string> m_loc;
  std::map<std::string,double>m_refSize;
  std::map<std::string,Gaudi::Plane3D> m_plane;
  //
  std::string m_fromCalo;
  std::string m_toCalo;
  std::vector<LHCb::CaloCellID> m_cells;
  std::vector<LHCb::CaloDigit*>  m_digits;
  double m_energy;
  int m_count;
  DeCalorimeter* m_fromDet;
  DeCalorimeter* m_toDet;
  double m_fromSize;
  double m_toSize;
  std::string m_toLoc;
  Gaudi::Plane3D m_toPlane;
  LHCb::CaloDigits* m_digs;
  ICaloGetterTool* m_getter;
private:
  bool m_geo;
  std::string m_getterName;
};
#endif // CALO2CALO_H





