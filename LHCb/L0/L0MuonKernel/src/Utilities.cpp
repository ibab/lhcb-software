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
std::vector<L0MTile>  L0Muon::tileListFromMap(const MuonTileID & pu, int type){
  std::vector<L0MTile> ltiles;

  // Open the map file 
  char name[4096];
  if (type == 0) {
    // for the L0Buffer
    char * format = "/lhcb/users/cogan/public/TestBench/L0BufferMaps/map_l0buffer_PLL_Q%dR%d%d%d.txt";
    sprintf(name,format,pu.quarter()+1,pu.region()+1,pu.nX(),pu.nY());
  } else  if (type == 1){
    // for the OLs
    char * format = "/lhcb/users/cogan/public/TestBench/L0BufferMaps/map_OL_PLL_Q%dR%d%d%d.txt";
    sprintf(name,format,pu.quarter()+1,pu.region()+1,pu.nX(),pu.nY());
  } 
  FILE * mapfile = fopen(name,"r");
  if (mapfile==NULL) return ltiles;

  // Build the list of L0MTile
  // Fix the size of the list
  int size;
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
    int Q;
    int R; 
    int sta;
    char type[8];
    int gridX;
    int gridY;
    int nX;
    int nY;


    if (fscanf(mapfile,"%d %d %d %d %s %d %d %d %d",&ibit,&Q,&R,&sta,type,&gridX,&gridY,&nX,&nY)==EOF) break;

    L0MBase::L0MTileType tag; 
    if      (type == "Pad")     { tag = L0MBase::Pad; }
    else if (type == "HS")      { tag = L0MBase::StripH; }
    else if (type == "VS")      { tag = L0MBase::StripV; }
    else { tag = L0MBase::Unknown; }
    
    // Build the L0MTile
     
    MuonLayout layout(gridX,gridY);
    MuonTileID mid(sta,0,0,layout,R,Q,nX,nY);
    L0MTile tile(mid,tag);
    ltiles[ibit]=tile;
  }

  return ltiles;
}
