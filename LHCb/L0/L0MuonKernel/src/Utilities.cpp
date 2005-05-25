#include "L0MuonKernel/Utilities.h"

#define  L0BUFFER_SIZE    32*16
#define  OL_SIZE          18*16  
/*
  Read an external file describing the register 
  and return the corresponding L0MTileList.
  The function takes the pu and the type of the register as argument.
  The pu is used to build the name of the input file.
  The type (either 0 for a L0buffer and 1 for OL) is used
  to build the file name as well as to determine the size of the 
  output tile list.
*/
std::vector<L0MTile>  L0Muon::tileListFromMap(const MuonTileID & pu, int type, std::string /*path*/){
  std::vector<L0MTile> ltiles;

  // Open the map file 
  char name[4096];
  if (type == 0) {
    // for the L0Buffer
    //     char * format = "%s/config/map_l0buffer_R%d%d%d.txt";
    //     sprintf(name,format,path.c_str(),pu.quarter()+1,pu.region()+1,pu.nX(),pu.nY());
    char * format = "/lhcb/users/cogan/cmtuser/L0/L0MuonKernelTest/v1r0/config/map_l0buffer_R%d%d%d.txt";
    sprintf(name,format,pu.region()+1,pu.nX(),pu.nY());
  } else  if (type == 1){
    // for the OLs
    //     char * format = "%s/config/map_ol_R%d%d%d.txt";
    //     sprintf(name,format,path.c_str(),pu.quarter()+1,pu.region()+1,pu.nX(),pu.nY());
    char * format = "/lhcb/users/cogan/cmtuser/L0/L0MuonKernelTest/v1r0/config/map_ol_R%d%d%d.txt";
    sprintf(name,format,pu.region()+1,pu.nX(),pu.nY());
  }
  FILE * mapfile = fopen(name,"r");
  if (mapfile==NULL) return ltiles;

  // Build the list of L0MTile
  // Fix the size of the list
  int size = L0BUFFER_SIZE;
  if (type == 0){
    size = L0BUFFER_SIZE;
  } else if (type == 1) {
    size = OL_SIZE;
  }

  ltiles = readTileListFromMap(mapfile,size);

  fclose(mapfile);

  return ltiles;
}


/*
  Generic method to extract a tile list from the map
*/
std::vector<L0MTile>  L0Muon::readTileListFromMap(FILE *mapfile,int max){
  std::vector<L0MTile> ltiles(max);
  
  while (1)
    {

      // Read line
      int ibit;
      char type[8];
      char smid[20];

      if (fscanf(mapfile,"%d %s %s",&ibit,smid,type)==EOF) break;

      L0MBase::L0MTileType tag; 
      if      (type == "Pad")     { tag = L0MBase::Pad; }
      else if (type == "HS")      { tag = L0MBase::StripH; }
      else if (type == "VS")      { tag = L0MBase::StripV; }
      else { tag = L0MBase::Unknown; }
 
      // Build the L0MTile
  
      MuonTileID mid(smid);
    
      L0MTile tile(mid,tag);
      ltiles[ibit]=tile;
    }

  return ltiles;
}


/*

*/
void L0Muon::splitTileListInfo(const std::vector<L0MTile> & l0mtilelist,
			       TileRegister * reg){


  std::vector<MuonTileID> muontilelist;
  boost::dynamic_bitset<> tilestag;
  boost::dynamic_bitset<> stripstag;
 
  splitTileListInfo(l0mtilelist,muontilelist,tilestag,stripstag);

  reg->setTileVector(muontilelist);
  reg->setTilesTagVector(tilestag);
  reg->setStripsTagVector(stripstag);
  
}

/*

*/
void L0Muon::splitTileListInfo(const std::vector<L0MTile> & l0mtilelist,
			       std::vector<MuonTileID> & muontilelist,
			       boost::dynamic_bitset<> & tilestag,
			       boost::dynamic_bitset<> & stripstag){
  
  muontilelist.clear();
  tilestag.clear();
  stripstag.clear();

  for (std::vector<L0MTile>::const_iterator it = l0mtilelist.begin(); it != l0mtilelist.end(); it++){
    muontilelist.push_back(*it);
    if ((*it).isValid()){
    
      if ( (*it).tag()==L0MBase::StripH ||
	   (*it).tag()==L0MBase::StripV){
	tilestag.push_back(0);
	if ((*it).tag()==L0MBase::StripH) stripstag.push_back(0);
	if ((*it).tag()==L0MBase::StripV) stripstag.push_back(1);

      } else if ((*it).tag()==L0MBase::Pad || (*it).tag()==L0MBase::YCorner ||
		 (*it).tag()==L0MBase::YBand) {
	tilestag.push_back(1);
	stripstag.push_back(0);

      } else if ( (*it).tag()==L0MBase::XBand ){
	std::vector<L0MTile> tmp = (*it).tiles();
	std::vector<L0MTile>::iterator itmp;
	itmp = tmp.begin();
	if ( (*itmp).tag()==L0MBase::Pad) {
	  tilestag.push_back(1);
	  stripstag.push_back(0);
	} else if ( (*itmp).tag()==L0MBase::StripH){
	  tilestag.push_back(0);
	  stripstag.push_back(0);
	} else if ( (*itmp).tag()==L0MBase::StripV){
	  tilestag.push_back(0);
	  stripstag.push_back(1);
	}     
      } else {
	tilestag.push_back(0);
	stripstag.push_back(0);
      }    
    } else {
      tilestag.push_back(0);
      stripstag.push_back(0);
    } 
  }
  
}
