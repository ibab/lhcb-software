#include <iostream>
#include <algorithm>
#include <fstream>
#include "AlignmentMonitoring/NameTTSector.h"

NameTTSector::NameTTSector(const std::string sector){
  m_sector   = sector;
  m_station  = sector.substr(2,1);
  m_layer    = sector.substr(3,1);
  m_region   = sector.substr(10,1);
  std::string key = "Sector";
  m_sectorID = std::stoi(sector.substr(sector.find(key)+key.length()));

  if( m_station.compare("a")==0 ){
    if( m_region.compare("A")==0 || m_region.compare("C")==0 ){
      if( (m_sectorID-1)%4==0 || (m_sectorID-1)%4==3 ) m_numberOfSensors = 4;
      else m_numberOfSensors = 3;
    }else{
      if( (m_sectorID-1)%6==0 || (m_sectorID-1)%6==5 ) m_numberOfSensors = 4;
      else if( (m_sectorID-1)%6==1 || (m_sectorID-1)%6==4 ) m_numberOfSensors = 2;
      else m_numberOfSensors = 1;  
    }
  }else{
    if( m_sectorID-1<4 ){
      if( (m_sectorID-1)%4==0 || (m_sectorID-1)%4==3 ) m_numberOfSensors = 4;
      else m_numberOfSensors = 3;
    }else if( m_sectorID-1>21 ){
      if( (m_sectorID-23)%4==0 || (m_sectorID-23)%4==3 ) m_numberOfSensors = 4;
      else m_numberOfSensors = 3;
    }else{
      if( (m_sectorID-5)%6==0 || (m_sectorID-5)%6==5 ) m_numberOfSensors = 4;
      else if( (m_sectorID-5)%6==1 || (m_sectorID-5)%6==4 ) m_numberOfSensors = 2;
      else m_numberOfSensors = 1;  
    }
  }

  m_uniqueSector = 0;

  if( m_station.compare("a")==0 && m_layer.compare("X")==0 ) m_uniqueSector += 1000;
  else if( m_station.compare("a")==0 && (m_layer.compare("U")==0||m_layer.compare("V")==0) ) m_uniqueSector += 2000;
  else if( m_station.compare("b")==0 && (m_layer.compare("U")==0||m_layer.compare("V")==0) ) m_uniqueSector += 3000;
  else if( m_station.compare("b")==0 &&  m_layer.compare("X")==0 ) m_uniqueSector += 4000;
 
  if( m_region.compare("A")==0 ) m_uniqueSector += 100;
  else if( m_region.compare("B")==0 ) m_uniqueSector += 200;
  else if( m_region.compare("C")==0 ) m_uniqueSector += 300;

  m_uniqueSector += m_sectorID;

  ConvertName2Channel();
}

void NameTTSector::ConvertName2Channel(){
  m_channel = -1;
  if (m_sector == "TTbVRegionCSector4") m_channel =  153;
  if (m_sector == "TTbVRegionCSector3") m_channel =  152;
  if (m_sector == "TTbVRegionCSector1") m_channel =  150;
  if (m_sector == "TTbVRegionCSector2") m_channel =  151;
  if (m_sector == "TTbVRegionCSector8") m_channel =  157;
  if (m_sector == "TTbVRegionCSector7") m_channel =  156;
  if (m_sector == "TTbVRegionCSector5") m_channel =  154;
  if (m_sector == "TTbVRegionCSector6") m_channel =  155;
  if (m_sector == "TTbVRegionCSector12") m_channel =  161;
  if (m_sector == "TTbVRegionCSector11") m_channel =  160;
  if (m_sector == "TTbVRegionCSector10") m_channel =  159;
  if (m_sector == "TTbVRegionCSector16") m_channel =  165;
  if (m_sector == "TTbVRegionCSector15") m_channel =  164;
  if (m_sector == "TTbVRegionCSector13") m_channel =  162;
  if (m_sector == "TTbVRegionCSector14") m_channel =  163;
  if (m_sector == "TTbVRegionCSector20") m_channel =  169;
  if (m_sector == "TTbVRegionCSector19") m_channel =  168;
  if (m_sector == "TTbVRegionCSector17") m_channel =  166;
  if (m_sector == "TTbVRegionCSector18") m_channel =  167;
  if (m_sector == "TTbVRegionCSector24") m_channel =  173;
  if (m_sector == "TTbVRegionCSector23") m_channel =  172;
  if (m_sector == "TTbVRegionCSector21") m_channel =  170;
  if (m_sector == "TTbVRegionCSector22") m_channel =  171;
  if (m_sector == "TTbVRegionBSector10") m_channel =  184;
  if (m_sector == "TTbVRegionBSector16") m_channel =  190;
  if (m_sector == "TTbVRegionBSector15") m_channel =  189;
  if (m_sector == "TTbVRegionBSector14") m_channel =  188;
  if (m_sector == "TTbVRegionBSector11") m_channel =  185;
  if (m_sector == "TTbVRegionBSector12") m_channel =  186;
  if (m_sector == "TTbVRegionBSector13") m_channel =  187;
  if (m_sector == "TTbVRegionBSector22") m_channel =  196;
  if (m_sector == "TTbVRegionBSector21") m_channel =  195;
  if (m_sector == "TTbVRegionBSector20") m_channel =  194;
  if (m_sector == "TTbVRegionBSector17") m_channel =  191;
  if (m_sector == "TTbVRegionBSector18") m_channel =  192;
  if (m_sector == "TTbVRegionBSector19") m_channel =  193;
  if (m_sector == "TTbVRegionBSector26") m_channel =  200;
  if (m_sector == "TTbVRegionBSector25") m_channel =  199;
  if (m_sector == "TTbVRegionBSector23") m_channel =  197;
  if (m_sector == "TTbVRegionBSector24") m_channel =  198;
  if (m_sector == "TTbVRegionCSector9") m_channel =  158;
  if (m_sector == "TTbVRegionBSector9") m_channel =  183;
  if (m_sector == "TTbVRegionBSector8") m_channel =  182;
  if (m_sector == "TTbVRegionBSector5") m_channel =  179;
  if (m_sector == "TTbVRegionBSector6") m_channel =  180;
  if (m_sector == "TTbVRegionBSector7") m_channel =  181;
  if (m_sector == "TTbVRegionBSector4") m_channel =  178;
  if (m_sector == "TTbVRegionBSector3") m_channel =  177;
  if (m_sector == "TTbVRegionBSector1") m_channel =  175;
  if (m_sector == "TTbVRegionBSector2") m_channel =  176;
  if (m_sector == "TTbVRegionASector4") m_channel =  203;
  if (m_sector == "TTbVRegionASector3") m_channel =  202;
  if (m_sector == "TTbVRegionASector1") m_channel =  200;
  if (m_sector == "TTbVRegionASector2") m_channel =  201;
  if (m_sector == "TTbVRegionASector8") m_channel =  207;
  if (m_sector == "TTbVRegionASector7") m_channel =  206;
  if (m_sector == "TTbVRegionASector5") m_channel =  204;
  if (m_sector == "TTbVRegionASector6") m_channel =  205;
  if (m_sector == "TTbVRegionASector9") m_channel =  208;
  if (m_sector == "TTbXRegionCSector4") m_channel =  228;
  if (m_sector == "TTbXRegionCSector3") m_channel =  227;
  if (m_sector == "TTbXRegionCSector1") m_channel =  225;
  if (m_sector == "TTbXRegionCSector2") m_channel =  226;
  if (m_sector == "TTbXRegionCSector8") m_channel =  232;
  if (m_sector == "TTbXRegionCSector7") m_channel =  231;
  if (m_sector == "TTbXRegionCSector5") m_channel =  229;
  if (m_sector == "TTbXRegionCSector6") m_channel =  230;
  if (m_sector == "TTbXRegionCSector9") m_channel =  233;
  if (m_sector == "TTbVRegionASector12") m_channel =  211;
  if (m_sector == "TTbVRegionASector11") m_channel =  210;
  if (m_sector == "TTbVRegionASector10") m_channel =  209;
  if (m_sector == "TTbVRegionASector16") m_channel =  215;
  if (m_sector == "TTbVRegionASector15") m_channel =  214;
  if (m_sector == "TTbVRegionASector13") m_channel =  212;
  if (m_sector == "TTbVRegionASector14") m_channel =  213;
  if (m_sector == "TTbVRegionASector20") m_channel =  219;
  if (m_sector == "TTbVRegionASector19") m_channel =  218;
  if (m_sector == "TTbVRegionASector17") m_channel =  216;
  if (m_sector == "TTbVRegionASector18") m_channel =  217;
  if (m_sector == "TTbVRegionASector24") m_channel =  223;
  if (m_sector == "TTbVRegionASector23") m_channel =  222;
  if (m_sector == "TTbVRegionASector21") m_channel =  220;
  if (m_sector == "TTbVRegionASector22") m_channel =  221;
  if (m_sector == "TTbXRegionCSector12") m_channel =  236;
  if (m_sector == "TTbXRegionCSector11") m_channel =  235;
  if (m_sector == "TTbXRegionCSector10") m_channel =  234;
  if (m_sector == "TTbXRegionCSector16") m_channel =  240;
  if (m_sector == "TTbXRegionCSector15") m_channel =  239;
  if (m_sector == "TTbXRegionCSector13") m_channel =  237;
  if (m_sector == "TTbXRegionCSector14") m_channel =  238;
  if (m_sector == "TTbXRegionCSector20") m_channel =  244;
  if (m_sector == "TTbXRegionCSector19") m_channel =  243;
  if (m_sector == "TTbXRegionCSector17") m_channel =  241;
  if (m_sector == "TTbXRegionCSector18") m_channel =  242;
  if (m_sector == "TTbXRegionCSector24") m_channel =  248;
  if (m_sector == "TTbXRegionCSector23") m_channel =  247;
  if (m_sector == "TTbXRegionCSector21") m_channel =  245;
  if (m_sector == "TTbXRegionCSector22") m_channel =  246;
  if (m_sector == "TTbXRegionBSector10") m_channel =  259;
  if (m_sector == "TTbXRegionBSector16") m_channel =  265;
  if (m_sector == "TTbXRegionBSector15") m_channel =  264;
  if (m_sector == "TTbXRegionBSector14") m_channel =  263;
  if (m_sector == "TTbXRegionBSector11") m_channel =  260;
  if (m_sector == "TTbXRegionBSector12") m_channel =  261;
  if (m_sector == "TTbXRegionBSector13") m_channel =  262;
  if (m_sector == "TTbXRegionBSector22") m_channel =  271;
  if (m_sector == "TTbXRegionBSector21") m_channel =  270;
  if (m_sector == "TTbXRegionBSector20") m_channel =  269;
  if (m_sector == "TTbXRegionBSector17") m_channel =  266;
  if (m_sector == "TTbXRegionBSector18") m_channel =  267;
  if (m_sector == "TTbXRegionBSector19") m_channel =  268;
  if (m_sector == "TTbXRegionBSector26") m_channel =  275;
  if (m_sector == "TTbXRegionBSector25") m_channel =  274;
  if (m_sector == "TTbXRegionBSector23") m_channel =  272;
  if (m_sector == "TTbXRegionBSector24") m_channel =  273;
  if (m_sector == "TTbXRegionBSector9") m_channel =  258;
  if (m_sector == "TTbXRegionBSector8") m_channel =  257;
  if (m_sector == "TTbXRegionBSector5") m_channel =  254;
  if (m_sector == "TTbXRegionBSector6") m_channel =  255;
  if (m_sector == "TTbXRegionBSector7") m_channel =  256;
  if (m_sector == "TTbXRegionBSector4") m_channel =  253;
  if (m_sector == "TTbXRegionBSector3") m_channel =  252;
  if (m_sector == "TTbXRegionBSector1") m_channel =  250;
  if (m_sector == "TTbXRegionBSector2") m_channel =  251;
  if (m_sector == "TTbXRegionASector4") m_channel =  278;
  if (m_sector == "TTbXRegionASector3") m_channel =  277;
  if (m_sector == "TTbXRegionASector1") m_channel =  275;
  if (m_sector == "TTbXRegionASector2") m_channel =  276;
  if (m_sector == "TTbXRegionASector8") m_channel =  282;
  if (m_sector == "TTbXRegionASector7") m_channel =  281;
  if (m_sector == "TTbXRegionASector5") m_channel =  279;
  if (m_sector == "TTbXRegionASector6") m_channel =  280;
  if (m_sector == "TTbXRegionASector9") m_channel =  283;
  if (m_sector == "TTbXRegionASector12") m_channel =  286;
  if (m_sector == "TTbXRegionASector11") m_channel =  285;
  if (m_sector == "TTbXRegionASector10") m_channel =  284;
  if (m_sector == "TTbXRegionASector16") m_channel =  290;
  if (m_sector == "TTbXRegionASector15") m_channel =  289;
  if (m_sector == "TTbXRegionASector13") m_channel =  287;
  if (m_sector == "TTbXRegionASector14") m_channel =  288;
  if (m_sector == "TTbXRegionASector20") m_channel =  294;
  if (m_sector == "TTbXRegionASector19") m_channel =  293;
  if (m_sector == "TTbXRegionASector17") m_channel =  291;
  if (m_sector == "TTbXRegionASector18") m_channel =  292;
  if (m_sector == "TTbXRegionASector24") m_channel =  298;
  if (m_sector == "TTbXRegionASector23") m_channel =  297;
  if (m_sector == "TTbXRegionASector21") m_channel =  295;
  if (m_sector == "TTbXRegionASector22") m_channel =  296;
  if (m_sector == "TTaURegionCSector4") m_channel =  78;
  if (m_sector == "TTaURegionCSector3") m_channel =  77;
  if (m_sector == "TTaURegionCSector1") m_channel =  75;
  if (m_sector == "TTaURegionCSector2") m_channel =  76;
  if (m_sector == "TTaURegionCSector8") m_channel =  82;
  if (m_sector == "TTaURegionCSector7") m_channel =  81;
  if (m_sector == "TTaURegionCSector5") m_channel =  79;
  if (m_sector == "TTaURegionCSector6") m_channel =  80;
  if (m_sector == "TTaURegionCSector9") m_channel =  83;
  if (m_sector == "TTaURegionCSector12") m_channel =  86;
  if (m_sector == "TTaURegionCSector11") m_channel =  85;
  if (m_sector == "TTaURegionCSector10") m_channel =  84;
  if (m_sector == "TTaURegionCSector16") m_channel =  90;
  if (m_sector == "TTaURegionCSector15") m_channel =  89;
  if (m_sector == "TTaURegionCSector13") m_channel =  87;
  if (m_sector == "TTaURegionCSector14") m_channel =  88;
  if (m_sector == "TTaURegionCSector20") m_channel =  94;
  if (m_sector == "TTaURegionCSector19") m_channel =  93;
  if (m_sector == "TTaURegionCSector17") m_channel =  91;
  if (m_sector == "TTaURegionCSector18") m_channel =  92;
  if (m_sector == "TTaURegionCSector24") m_channel =  98;
  if (m_sector == "TTaURegionCSector23") m_channel =  97;
  if (m_sector == "TTaURegionCSector21") m_channel =  95;
  if (m_sector == "TTaURegionCSector22") m_channel =  96;
  if (m_sector == "TTaURegionBSector6") m_channel =  105;
  if (m_sector == "TTaURegionBSector5") m_channel =  104;
  if (m_sector == "TTaURegionBSector4") m_channel =  103;
  if (m_sector == "TTaURegionBSector1") m_channel =  100;
  if (m_sector == "TTaURegionBSector2") m_channel =  101;
  if (m_sector == "TTaURegionBSector3") m_channel =  102;
  if (m_sector == "TTaURegionBSector7") m_channel =  106;
  if (m_sector == "TTaURegionBSector8") m_channel =  107;
  if (m_sector == "TTaURegionBSector9") m_channel =  108;
  if (m_sector == "TTaURegionASector4") m_channel =  128;
  if (m_sector == "TTaURegionASector3") m_channel =  127;
  if (m_sector == "TTaURegionASector1") m_channel =  125;
  if (m_sector == "TTaURegionASector2") m_channel =  126;
  if (m_sector == "TTaURegionASector8") m_channel =  132;
  if (m_sector == "TTaURegionASector7") m_channel =  131;
  if (m_sector == "TTaURegionASector5") m_channel =  129;
  if (m_sector == "TTaURegionASector6") m_channel =  130;
  if (m_sector == "TTaURegionASector9") m_channel =  133;
  if (m_sector == "TTaURegionASector12") m_channel =  136;
  if (m_sector == "TTaURegionASector11") m_channel =  135;
  if (m_sector == "TTaURegionBSector12") m_channel =  111;
  if (m_sector == "TTaURegionBSector11") m_channel =  110;
  if (m_sector == "TTaURegionBSector10") m_channel =  109;
  if (m_sector == "TTaURegionBSector18") m_channel =  117;
  if (m_sector == "TTaURegionBSector17") m_channel =  116;
  if (m_sector == "TTaURegionBSector16") m_channel =  115;
  if (m_sector == "TTaURegionBSector13") m_channel =  112;
  if (m_sector == "TTaURegionBSector14") m_channel =  113;
  if (m_sector == "TTaURegionBSector15") m_channel =  114;
  if (m_sector == "TTaURegionASector10") m_channel =  134;
  if (m_sector == "TTaURegionASector16") m_channel =  140;
  if (m_sector == "TTaURegionASector15") m_channel =  139;
  if (m_sector == "TTaURegionASector13") m_channel =  137;
  if (m_sector == "TTaURegionASector14") m_channel =  138;
  if (m_sector == "TTaURegionASector20") m_channel =  144;
  if (m_sector == "TTaURegionASector19") m_channel =  143;
  if (m_sector == "TTaURegionASector17") m_channel =  141;
  if (m_sector == "TTaURegionASector18") m_channel =  142;
  if (m_sector == "TTaURegionASector24") m_channel =  148;
  if (m_sector == "TTaURegionASector23") m_channel =  147;
  if (m_sector == "TTaURegionASector21") m_channel =  145;
  if (m_sector == "TTaURegionASector22") m_channel =  146;
  if (m_sector == "TTaXRegionCSector4") m_channel =  3;
  if (m_sector == "TTaXRegionCSector3") m_channel =  2;
  if (m_sector == "TTaXRegionCSector1") m_channel =  0;
  if (m_sector == "TTaXRegionCSector2") m_channel =  1;
  if (m_sector == "TTaXRegionCSector8") m_channel =  7;
  if (m_sector == "TTaXRegionCSector7") m_channel =  6;
  if (m_sector == "TTaXRegionCSector5") m_channel =  4;
  if (m_sector == "TTaXRegionCSector6") m_channel =  5;
  if (m_sector == "TTaXRegionCSector9") m_channel =  8;
  if (m_sector == "TTaXRegionCSector10") m_channel =  9;
  if (m_sector == "TTaXRegionCSector12") m_channel =  11;
  if (m_sector == "TTaXRegionCSector11") m_channel =  10;
  if (m_sector == "TTaXRegionCSector16") m_channel =  15;
  if (m_sector == "TTaXRegionCSector15") m_channel =  14;
  if (m_sector == "TTaXRegionCSector13") m_channel =  12;
  if (m_sector == "TTaXRegionCSector14") m_channel =  13;
  if (m_sector == "TTaXRegionCSector20") m_channel =  19;
  if (m_sector == "TTaXRegionCSector19") m_channel =  18;
  if (m_sector == "TTaXRegionCSector17") m_channel =  16;
  if (m_sector == "TTaXRegionCSector18") m_channel =  17;
  if (m_sector == "TTaXRegionCSector24") m_channel =  23;
  if (m_sector == "TTaXRegionCSector23") m_channel =  22;
  if (m_sector == "TTaXRegionCSector21") m_channel =  20;
  if (m_sector == "TTaXRegionCSector22") m_channel =  21;
  if (m_sector == "TTaXRegionBSector12") m_channel =  36;
  if (m_sector == "TTaXRegionBSector11") m_channel =  35;
  if (m_sector == "TTaXRegionBSector10") m_channel =  34;
  if (m_sector == "TTaXRegionBSector18") m_channel =  42;
  if (m_sector == "TTaXRegionBSector17") m_channel =  41;
  if (m_sector == "TTaXRegionBSector16") m_channel =  40;
  if (m_sector == "TTaXRegionBSector13") m_channel =  37;
  if (m_sector == "TTaXRegionBSector14") m_channel =  38;
  if (m_sector == "TTaXRegionBSector15") m_channel =  39;
  if (m_sector == "TTaXRegionASector12") m_channel =  61;
  if (m_sector == "TTaXRegionASector11") m_channel =  60;
  if (m_sector == "TTaXRegionASector10") m_channel =  59;
  if (m_sector == "TTaXRegionASector16") m_channel =  65;
  if (m_sector == "TTaXRegionASector15") m_channel =  64;
  if (m_sector == "TTaXRegionASector13") m_channel =  62;
  if (m_sector == "TTaXRegionASector14") m_channel =  63;
  if (m_sector == "TTaXRegionASector20") m_channel =  69;
  if (m_sector == "TTaXRegionASector19") m_channel =  68;
  if (m_sector == "TTaXRegionASector17") m_channel =  66;
  if (m_sector == "TTaXRegionASector18") m_channel =  67;
  if (m_sector == "TTaXRegionASector24") m_channel =  73;
  if (m_sector == "TTaXRegionASector23") m_channel =  72;
  if (m_sector == "TTaXRegionASector21") m_channel =  70;
  if (m_sector == "TTaXRegionASector22") m_channel =  71;
  if (m_sector == "TTaXRegionBSector6") m_channel =  30;
  if (m_sector == "TTaXRegionBSector5") m_channel =  29;
  if (m_sector == "TTaXRegionBSector4") m_channel =  28;
  if (m_sector == "TTaXRegionBSector1") m_channel =  25;
  if (m_sector == "TTaXRegionBSector2") m_channel =  26;
  if (m_sector == "TTaXRegionBSector3") m_channel =  27;
  if (m_sector == "TTaXRegionBSector7") m_channel =  31;
  if (m_sector == "TTaXRegionBSector8") m_channel =  32;
  if (m_sector == "TTaXRegionBSector9") m_channel =  33;
  if (m_sector == "TTaXRegionASector4") m_channel =  53;
  if (m_sector == "TTaXRegionASector3") m_channel =  52;
  if (m_sector == "TTaXRegionASector1") m_channel =  50;
  if (m_sector == "TTaXRegionASector2") m_channel =  51;
  if (m_sector == "TTaXRegionASector8") m_channel =  57;
  if (m_sector == "TTaXRegionASector7") m_channel =  56;
  if (m_sector == "TTaXRegionASector5") m_channel =  54;
  if (m_sector == "TTaXRegionASector6") m_channel =  55;
  if (m_sector == "TTaXRegionASector9") m_channel =  58;
}


int NameTTSector::GetUniqueSector(){
  return m_uniqueSector;
}

int NameTTSector::GetNumberOfSensors(){
  return m_numberOfSensors;
}

int NameTTSector::GetSectorID(){
  return m_sectorID;
}

int NameTTSector::GetChannel(){
  return m_channel;
}

std::string NameTTSector::GetStation(){
  return m_station;
}

std::string NameTTSector::GetLayer(){
  return m_layer;
}

std::string NameTTSector::GetRegion(){
  return m_region;
}
