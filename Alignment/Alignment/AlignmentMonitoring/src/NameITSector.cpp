#include <iostream>
#include <algorithm>
#include <fstream>
#include "AlignmentMonitoring/NameITSector.h"

NameITSector::NameITSector(const std::string sector){
  m_sector = sector;
  m_station  = sector.substr(0,3);
  m_stationID= m_station.back()-'0';
  m_sectorID = sector.back()-'0';

  m_box = "";
  if( sector.find("ASide")!=std::string::npos ) m_box = "ASide";
  if( sector.find("CSide")!=std::string::npos ) m_box = "CSide";
  if( sector.find("Top")!=std::string::npos   ) m_box = "Top";
  if( sector.find("Bottom")!=std::string::npos) m_box = "Bottom";

  m_layer  = "";
  if( sector.find("X1")!=std::string::npos ) m_layer = "X1";
  if( sector.find("X2")!=std::string::npos ) m_layer = "X2";
  if( sector.find("U")!=std::string::npos  ) m_layer = "U";
  if( sector.find("V")!=std::string::npos  ) m_layer = "V";

  if( m_box.compare("ASide")==0 || m_box.compare("CSide")==0 ) m_numberOfSensors = 2;
  else m_numberOfSensors = 1;

  m_uniqueBox = 0;
  if( m_stationID==1 ) m_uniqueBox += 1000;
  else if( m_stationID==2 ) m_uniqueBox += 2000;
  else if( m_stationID==3 ) m_uniqueBox += 3000;

  if( m_box.compare("ASide")==0 ) m_uniqueBox += 100;
  else if( m_box.compare("CSide")==0 ) m_uniqueBox += 200;
  else if( m_box.compare("Top")==0 ) m_uniqueBox += 300;
  else if( m_box.compare("Bottom")==0 ) m_uniqueBox += 400;

  m_uniqueLayer = m_uniqueBox;
  if( m_layer.compare("X1")==0 ) m_uniqueLayer += 10;
  else if( m_layer.compare("U")==0 ) m_uniqueLayer += 20;
  else if( m_layer.compare("V")==0 ) m_uniqueLayer += 30;
  else if( m_layer.compare("X2")==0 ) m_uniqueLayer += 40;

  m_uniqueSector = m_uniqueLayer;
  m_uniqueSector += m_sectorID;

  ConvertName2Channel();
}

void NameITSector::ConvertName2Channel(){
  m_channel = -1;

  if (m_sector == "IT3CSideX2Sector1") m_channel = 308;
  if (m_sector == "IT3CSideX2Sector2") m_channel = 309;
  if (m_sector == "IT3CSideX2Sector3") m_channel = 310;
  if (m_sector == "IT3CSideX2Sector4") m_channel = 311;
  if (m_sector == "IT3CSideX2Sector5") m_channel = 312;
  if (m_sector == "IT3CSideX2Sector6") m_channel = 313;
  if (m_sector == "IT3CSideX2Sector7") m_channel = 314;
  if (m_sector == "IT3CSideVSector1") m_channel = 280;
  if (m_sector == "IT3CSideVSector2") m_channel = 281;
  if (m_sector == "IT3CSideVSector3") m_channel = 282;
  if (m_sector == "IT3CSideVSector4") m_channel = 283;
  if (m_sector == "IT3CSideVSector5") m_channel = 284;
  if (m_sector == "IT3CSideVSector6") m_channel = 285;
  if (m_sector == "IT3CSideVSector7") m_channel = 286;
  if (m_sector == "IT3CSideUSector1") m_channel = 252;
  if (m_sector == "IT3CSideUSector2") m_channel = 253;
  if (m_sector == "IT3CSideUSector3") m_channel = 254;
  if (m_sector == "IT3CSideUSector4") m_channel = 255;
  if (m_sector == "IT3CSideUSector5") m_channel = 256;
  if (m_sector == "IT3CSideUSector6") m_channel = 257;
  if (m_sector == "IT3CSideUSector7") m_channel = 258;
  if (m_sector == "IT3CSideX1Sector1") m_channel = 224;
  if (m_sector == "IT3CSideX1Sector2") m_channel = 225;
  if (m_sector == "IT3CSideX1Sector3") m_channel = 226;
  if (m_sector == "IT3CSideX1Sector4") m_channel = 227;
  if (m_sector == "IT3CSideX1Sector5") m_channel = 228;
  if (m_sector == "IT3CSideX1Sector6") m_channel = 229;
  if (m_sector == "IT3CSideX1Sector7") m_channel = 230;
  if (m_sector == "IT3ASideX2Sector1") m_channel = 315;
  if (m_sector == "IT3ASideX2Sector2") m_channel = 316;
  if (m_sector == "IT3ASideX2Sector3") m_channel = 317;
  if (m_sector == "IT3ASideX2Sector4") m_channel = 318;
  if (m_sector == "IT3ASideX2Sector5") m_channel = 319;
  if (m_sector == "IT3ASideX2Sector6") m_channel = 320;
  if (m_sector == "IT3ASideX2Sector7") m_channel = 321;
  if (m_sector == "IT3ASideVSector1") m_channel = 287;
  if (m_sector == "IT3ASideVSector2") m_channel = 288;
  if (m_sector == "IT3ASideVSector3") m_channel = 289;
  if (m_sector == "IT3ASideVSector4") m_channel = 290;
  if (m_sector == "IT3ASideVSector5") m_channel = 291;
  if (m_sector == "IT3ASideVSector6") m_channel = 292;
  if (m_sector == "IT3ASideVSector7") m_channel = 293;
  if (m_sector == "IT3ASideUSector1") m_channel = 259;
  if (m_sector == "IT3ASideUSector2") m_channel = 260;
  if (m_sector == "IT3ASideUSector3") m_channel = 261;
  if (m_sector == "IT3ASideUSector4") m_channel = 262;
  if (m_sector == "IT3ASideUSector5") m_channel = 263;
  if (m_sector == "IT3ASideUSector6") m_channel = 264;
  if (m_sector == "IT3ASideUSector7") m_channel = 265;
  if (m_sector == "IT3ASideX1Sector1") m_channel = 231;
  if (m_sector == "IT3ASideX1Sector2") m_channel = 232;
  if (m_sector == "IT3ASideX1Sector3") m_channel = 233;
  if (m_sector == "IT3ASideX1Sector4") m_channel = 234;
  if (m_sector == "IT3ASideX1Sector5") m_channel = 235;
  if (m_sector == "IT3ASideX1Sector6") m_channel = 236;
  if (m_sector == "IT3ASideX1Sector7") m_channel = 237;
  if (m_sector == "IT3BottomX2Sector1") m_channel = 322;
  if (m_sector == "IT3BottomX2Sector2") m_channel = 323;
  if (m_sector == "IT3BottomX2Sector3") m_channel = 324;
  if (m_sector == "IT3BottomX2Sector4") m_channel = 325;
  if (m_sector == "IT3BottomX2Sector5") m_channel = 326;
  if (m_sector == "IT3BottomX2Sector6") m_channel = 327;
  if (m_sector == "IT3BottomX2Sector7") m_channel = 328;
  if (m_sector == "IT3BottomVSector1") m_channel = 294;
  if (m_sector == "IT3BottomVSector2") m_channel = 295;
  if (m_sector == "IT3BottomVSector3") m_channel = 296;
  if (m_sector == "IT3BottomVSector4") m_channel = 297;
  if (m_sector == "IT3BottomVSector5") m_channel = 298;
  if (m_sector == "IT3BottomVSector6") m_channel = 299;
  if (m_sector == "IT3BottomVSector7") m_channel = 300;
  if (m_sector == "IT3BottomUSector1") m_channel = 266;
  if (m_sector == "IT3BottomUSector2") m_channel = 267;
  if (m_sector == "IT3BottomUSector3") m_channel = 268;
  if (m_sector == "IT3BottomUSector4") m_channel = 269;
  if (m_sector == "IT3BottomUSector5") m_channel = 270;
  if (m_sector == "IT3BottomUSector6") m_channel = 271;
  if (m_sector == "IT3BottomUSector7") m_channel = 272;
  if (m_sector == "IT3BottomX1Sector1") m_channel = 238;
  if (m_sector == "IT3BottomX1Sector2") m_channel = 239;
  if (m_sector == "IT3BottomX1Sector3") m_channel = 240;
  if (m_sector == "IT3BottomX1Sector4") m_channel = 241;
  if (m_sector == "IT3BottomX1Sector5") m_channel = 242;
  if (m_sector == "IT3BottomX1Sector6") m_channel = 243;
  if (m_sector == "IT3BottomX1Sector7") m_channel = 244;
  if (m_sector == "IT3TopX2Sector1") m_channel = 329;
  if (m_sector == "IT3TopX2Sector2") m_channel = 330;
  if (m_sector == "IT3TopX2Sector3") m_channel = 331;
  if (m_sector == "IT3TopX2Sector4") m_channel = 332;
  if (m_sector == "IT3TopX2Sector5") m_channel = 333;
  if (m_sector == "IT3TopX2Sector6") m_channel = 334;
  if (m_sector == "IT3TopX2Sector7") m_channel = 335;
  if (m_sector == "IT3TopVSector1") m_channel = 301;
  if (m_sector == "IT3TopVSector2") m_channel = 302;
  if (m_sector == "IT3TopVSector3") m_channel = 303;
  if (m_sector == "IT3TopVSector4") m_channel = 304;
  if (m_sector == "IT3TopVSector5") m_channel = 305;
  if (m_sector == "IT3TopVSector6") m_channel = 306;
  if (m_sector == "IT3TopVSector7") m_channel = 307;
  if (m_sector == "IT3TopUSector1") m_channel = 273;
  if (m_sector == "IT3TopUSector2") m_channel = 274;
  if (m_sector == "IT3TopUSector3") m_channel = 275;
  if (m_sector == "IT3TopUSector4") m_channel = 276;
  if (m_sector == "IT3TopUSector5") m_channel = 277;
  if (m_sector == "IT3TopUSector6") m_channel = 278;
  if (m_sector == "IT3TopUSector7") m_channel = 279;
  if (m_sector == "IT3TopX1Sector1") m_channel = 245;
  if (m_sector == "IT3TopX1Sector2") m_channel = 246;
  if (m_sector == "IT3TopX1Sector3") m_channel = 247;
  if (m_sector == "IT3TopX1Sector4") m_channel = 248;
  if (m_sector == "IT3TopX1Sector5") m_channel = 249;
  if (m_sector == "IT3TopX1Sector6") m_channel = 250;
  if (m_sector == "IT3TopX1Sector7") m_channel = 251;
  if (m_sector == "IT2CSideX2Sector1") m_channel = 196;
  if (m_sector == "IT2CSideX2Sector2") m_channel = 197;
  if (m_sector == "IT2CSideX2Sector3") m_channel = 198;
  if (m_sector == "IT2CSideX2Sector4") m_channel = 199;
  if (m_sector == "IT2CSideX2Sector5") m_channel = 200;
  if (m_sector == "IT2CSideX2Sector6") m_channel = 201;
  if (m_sector == "IT2CSideX2Sector7") m_channel = 202;
  if (m_sector == "IT2CSideVSector1") m_channel = 168;
  if (m_sector == "IT2CSideVSector2") m_channel = 169;
  if (m_sector == "IT2CSideVSector3") m_channel = 170;
  if (m_sector == "IT2CSideVSector4") m_channel = 171;
  if (m_sector == "IT2CSideVSector5") m_channel = 172;
  if (m_sector == "IT2CSideVSector6") m_channel = 173;
  if (m_sector == "IT2CSideVSector7") m_channel = 174;
  if (m_sector == "IT2CSideUSector1") m_channel = 140;
  if (m_sector == "IT2CSideUSector2") m_channel = 141;
  if (m_sector == "IT2CSideUSector3") m_channel = 142;
  if (m_sector == "IT2CSideUSector4") m_channel = 143;
  if (m_sector == "IT2CSideUSector5") m_channel = 144;
  if (m_sector == "IT2CSideUSector6") m_channel = 145;
  if (m_sector == "IT2CSideUSector7") m_channel = 146;
  if (m_sector == "IT2CSideX1Sector1") m_channel = 112;
  if (m_sector == "IT2CSideX1Sector2") m_channel = 113;
  if (m_sector == "IT2CSideX1Sector3") m_channel = 114;
  if (m_sector == "IT2CSideX1Sector4") m_channel = 115;
  if (m_sector == "IT2CSideX1Sector5") m_channel = 116;
  if (m_sector == "IT2CSideX1Sector6") m_channel = 117;
  if (m_sector == "IT2CSideX1Sector7") m_channel = 118;
  if (m_sector == "IT2ASideX2Sector1") m_channel = 203;
  if (m_sector == "IT2ASideX2Sector2") m_channel = 204;
  if (m_sector == "IT2ASideX2Sector3") m_channel = 205;
  if (m_sector == "IT2ASideX2Sector4") m_channel = 206;
  if (m_sector == "IT2ASideX2Sector5") m_channel = 207;
  if (m_sector == "IT2ASideX2Sector6") m_channel = 208;
  if (m_sector == "IT2ASideX2Sector7") m_channel = 209;
  if (m_sector == "IT2ASideVSector1") m_channel = 175;
  if (m_sector == "IT2ASideVSector2") m_channel = 176;
  if (m_sector == "IT2ASideVSector3") m_channel = 177;
  if (m_sector == "IT2ASideVSector4") m_channel = 178;
  if (m_sector == "IT2ASideVSector5") m_channel = 179;
  if (m_sector == "IT2ASideVSector6") m_channel = 180;
  if (m_sector == "IT2ASideVSector7") m_channel = 181;
  if (m_sector == "IT2ASideUSector1") m_channel = 147;
  if (m_sector == "IT2ASideUSector2") m_channel = 148;
  if (m_sector == "IT2ASideUSector3") m_channel = 149;
  if (m_sector == "IT2ASideUSector4") m_channel = 150;
  if (m_sector == "IT2ASideUSector5") m_channel = 151;
  if (m_sector == "IT2ASideUSector6") m_channel = 152;
  if (m_sector == "IT2ASideUSector7") m_channel = 153;
  if (m_sector == "IT2ASideX1Sector1") m_channel = 119;
  if (m_sector == "IT2ASideX1Sector2") m_channel = 120;
  if (m_sector == "IT2ASideX1Sector3") m_channel = 121;
  if (m_sector == "IT2ASideX1Sector4") m_channel = 122;
  if (m_sector == "IT2ASideX1Sector5") m_channel = 123;
  if (m_sector == "IT2ASideX1Sector6") m_channel = 124;
  if (m_sector == "IT2ASideX1Sector7") m_channel = 125;
  if (m_sector == "IT2BottomX2Sector1") m_channel = 210;
  if (m_sector == "IT2BottomX2Sector2") m_channel = 211;
  if (m_sector == "IT2BottomX2Sector3") m_channel = 212;
  if (m_sector == "IT2BottomX2Sector4") m_channel = 213;
  if (m_sector == "IT2BottomX2Sector5") m_channel = 214;
  if (m_sector == "IT2BottomX2Sector6") m_channel = 215;
  if (m_sector == "IT2BottomX2Sector7") m_channel = 216;
  if (m_sector == "IT2BottomX1Sector1") m_channel = 126;
  if (m_sector == "IT2BottomX1Sector2") m_channel = 127;
  if (m_sector == "IT2BottomX1Sector3") m_channel = 128;
  if (m_sector == "IT2BottomX1Sector4") m_channel = 129;
  if (m_sector == "IT2BottomX1Sector5") m_channel = 130;
  if (m_sector == "IT2BottomX1Sector6") m_channel = 131;
  if (m_sector == "IT2BottomX1Sector7") m_channel = 132;
  if (m_sector == "IT2BottomVSector1") m_channel = 182;
  if (m_sector == "IT2BottomVSector2") m_channel = 183;
  if (m_sector == "IT2BottomVSector3") m_channel = 184;
  if (m_sector == "IT2BottomVSector4") m_channel = 185;
  if (m_sector == "IT2BottomVSector5") m_channel = 186;
  if (m_sector == "IT2BottomVSector6") m_channel = 187;
  if (m_sector == "IT2BottomVSector7") m_channel = 188;
  if (m_sector == "IT2BottomUSector1") m_channel = 154;
  if (m_sector == "IT2BottomUSector2") m_channel = 155;
  if (m_sector == "IT2BottomUSector3") m_channel = 156;
  if (m_sector == "IT2BottomUSector4") m_channel = 157;
  if (m_sector == "IT2BottomUSector5") m_channel = 158;
  if (m_sector == "IT2BottomUSector6") m_channel = 159;
  if (m_sector == "IT2BottomUSector7") m_channel = 160;
  if (m_sector == "IT1CSideX2Sector1") m_channel = 84;
  if (m_sector == "IT1CSideX2Sector2") m_channel = 85;
  if (m_sector == "IT1CSideX2Sector3") m_channel = 86;
  if (m_sector == "IT1CSideX2Sector4") m_channel = 87;
  if (m_sector == "IT1CSideX2Sector5") m_channel = 88;
  if (m_sector == "IT1CSideX2Sector6") m_channel = 89;
  if (m_sector == "IT1CSideX2Sector7") m_channel = 90;
  if (m_sector == "IT1ASideX2Sector1") m_channel = 91;
  if (m_sector == "IT1ASideX2Sector2") m_channel = 92;
  if (m_sector == "IT1ASideX2Sector3") m_channel = 93;
  if (m_sector == "IT1ASideX2Sector4") m_channel = 94;
  if (m_sector == "IT1ASideX2Sector5") m_channel = 95;
  if (m_sector == "IT1ASideX2Sector6") m_channel = 96;
  if (m_sector == "IT1ASideX2Sector7") m_channel = 97;
  if (m_sector == "IT2TopX2Sector1") m_channel = 217;
  if (m_sector == "IT2TopX2Sector2") m_channel = 218;
  if (m_sector == "IT2TopX2Sector3") m_channel = 219;
  if (m_sector == "IT2TopX2Sector4") m_channel = 220;
  if (m_sector == "IT2TopX2Sector5") m_channel = 221;
  if (m_sector == "IT2TopX2Sector6") m_channel = 222;
  if (m_sector == "IT2TopX2Sector7") m_channel = 223;
  if (m_sector == "IT2TopX1Sector1") m_channel = 133;
  if (m_sector == "IT2TopX1Sector2") m_channel = 134;
  if (m_sector == "IT2TopX1Sector3") m_channel = 135;
  if (m_sector == "IT2TopX1Sector4") m_channel = 136;
  if (m_sector == "IT2TopX1Sector5") m_channel = 137;
  if (m_sector == "IT2TopX1Sector6") m_channel = 138;
  if (m_sector == "IT2TopX1Sector7") m_channel = 139;
  if (m_sector == "IT1CSideVSector1") m_channel = 56;
  if (m_sector == "IT1CSideVSector2") m_channel = 57;
  if (m_sector == "IT1CSideVSector3") m_channel = 58;
  if (m_sector == "IT1CSideVSector4") m_channel = 59;
  if (m_sector == "IT1CSideVSector5") m_channel = 60;
  if (m_sector == "IT1CSideVSector6") m_channel = 61;
  if (m_sector == "IT1CSideVSector7") m_channel = 62;
  if (m_sector == "IT1CSideUSector1") m_channel = 28;
  if (m_sector == "IT1CSideUSector2") m_channel = 29;
  if (m_sector == "IT1CSideUSector3") m_channel = 30;
  if (m_sector == "IT1CSideUSector4") m_channel = 31;
  if (m_sector == "IT1CSideUSector5") m_channel = 32;
  if (m_sector == "IT1CSideUSector6") m_channel = 33;
  if (m_sector == "IT1CSideUSector7") m_channel = 34;
  if (m_sector == "IT1CSideX1Sector1") m_channel = 0;
  if (m_sector == "IT1CSideX1Sector2") m_channel = 1;
  if (m_sector == "IT1CSideX1Sector3") m_channel = 2;
  if (m_sector == "IT1CSideX1Sector4") m_channel = 3;
  if (m_sector == "IT1CSideX1Sector5") m_channel = 4;
  if (m_sector == "IT1CSideX1Sector6") m_channel = 5;
  if (m_sector == "IT1CSideX1Sector7") m_channel = 6;
  if (m_sector == "IT2TopVSector1") m_channel = 189;
  if (m_sector == "IT2TopVSector2") m_channel = 190;
  if (m_sector == "IT2TopVSector3") m_channel = 191;
  if (m_sector == "IT2TopVSector4") m_channel = 192;
  if (m_sector == "IT2TopVSector5") m_channel = 193;
  if (m_sector == "IT2TopVSector6") m_channel = 194;
  if (m_sector == "IT2TopVSector7") m_channel = 195;
  if (m_sector == "IT2TopUSector1") m_channel = 161;
  if (m_sector == "IT2TopUSector2") m_channel = 162;
  if (m_sector == "IT2TopUSector3") m_channel = 163;
  if (m_sector == "IT2TopUSector4") m_channel = 164;
  if (m_sector == "IT2TopUSector5") m_channel = 165;
  if (m_sector == "IT2TopUSector6") m_channel = 166;
  if (m_sector == "IT2TopUSector7") m_channel = 167;
  if (m_sector == "IT1ASideVSector1") m_channel = 63;
  if (m_sector == "IT1ASideVSector2") m_channel = 64;
  if (m_sector == "IT1ASideVSector3") m_channel = 65;
  if (m_sector == "IT1ASideVSector4") m_channel = 66;
  if (m_sector == "IT1ASideVSector5") m_channel = 67;
  if (m_sector == "IT1ASideVSector6") m_channel = 68;
  if (m_sector == "IT1ASideVSector7") m_channel = 69;
  if (m_sector == "IT1ASideUSector1") m_channel = 35;
  if (m_sector == "IT1ASideUSector2") m_channel = 36;
  if (m_sector == "IT1ASideUSector3") m_channel = 37;
  if (m_sector == "IT1ASideUSector4") m_channel = 38;
  if (m_sector == "IT1ASideUSector5") m_channel = 39;
  if (m_sector == "IT1ASideUSector6") m_channel = 40;
  if (m_sector == "IT1ASideUSector7") m_channel = 41;
  if (m_sector == "IT1ASideX1Sector1") m_channel = 7;
  if (m_sector == "IT1ASideX1Sector2") m_channel = 8;
  if (m_sector == "IT1ASideX1Sector3") m_channel = 9;
  if (m_sector == "IT1ASideX1Sector4") m_channel = 10;
  if (m_sector == "IT1ASideX1Sector5") m_channel = 11;
  if (m_sector == "IT1ASideX1Sector6") m_channel = 12;
  if (m_sector == "IT1ASideX1Sector7") m_channel = 13;
  if (m_sector == "IT1BottomX2Sector1") m_channel = 98;
  if (m_sector == "IT1BottomX2Sector2") m_channel = 99;
  if (m_sector == "IT1BottomX2Sector3") m_channel = 100;
  if (m_sector == "IT1BottomX2Sector4") m_channel = 101;
  if (m_sector == "IT1BottomX2Sector5") m_channel = 102;
  if (m_sector == "IT1BottomX2Sector6") m_channel = 103;
  if (m_sector == "IT1BottomX2Sector7") m_channel = 104;
  if (m_sector == "IT1BottomVSector1") m_channel = 70;
  if (m_sector == "IT1BottomVSector2") m_channel = 71;
  if (m_sector == "IT1BottomVSector3") m_channel = 72;
  if (m_sector == "IT1BottomVSector4") m_channel = 73;
  if (m_sector == "IT1BottomVSector5") m_channel = 74;
  if (m_sector == "IT1BottomVSector6") m_channel = 75;
  if (m_sector == "IT1BottomVSector7") m_channel = 76;
  if (m_sector == "IT1BottomUSector1") m_channel = 42;
  if (m_sector == "IT1BottomUSector2") m_channel = 43;
  if (m_sector == "IT1BottomUSector3") m_channel = 44;
  if (m_sector == "IT1BottomUSector4") m_channel = 45;
  if (m_sector == "IT1BottomUSector5") m_channel = 46;
  if (m_sector == "IT1BottomUSector6") m_channel = 47;
  if (m_sector == "IT1BottomUSector7") m_channel = 48;
  if (m_sector == "IT1BottomX1Sector1") m_channel = 14;
  if (m_sector == "IT1BottomX1Sector2") m_channel = 15;
  if (m_sector == "IT1BottomX1Sector3") m_channel = 16;
  if (m_sector == "IT1BottomX1Sector4") m_channel = 17;
  if (m_sector == "IT1BottomX1Sector5") m_channel = 18;
  if (m_sector == "IT1BottomX1Sector6") m_channel = 19;
  if (m_sector == "IT1BottomX1Sector7") m_channel = 20;
  if (m_sector == "IT1TopX2Sector1") m_channel = 105;
  if (m_sector == "IT1TopX2Sector2") m_channel = 106;
  if (m_sector == "IT1TopX2Sector3") m_channel = 107;
  if (m_sector == "IT1TopX2Sector4") m_channel = 108;
  if (m_sector == "IT1TopX2Sector5") m_channel = 109;
  if (m_sector == "IT1TopX2Sector6") m_channel = 110;
  if (m_sector == "IT1TopX2Sector7") m_channel = 111;
  if (m_sector == "IT1TopVSector1") m_channel = 77;
  if (m_sector == "IT1TopVSector2") m_channel = 78;
  if (m_sector == "IT1TopVSector3") m_channel = 79;
  if (m_sector == "IT1TopVSector4") m_channel = 80;
  if (m_sector == "IT1TopVSector5") m_channel = 81;
  if (m_sector == "IT1TopVSector6") m_channel = 82;
  if (m_sector == "IT1TopVSector7") m_channel = 83;
  if (m_sector == "IT1TopUSector1") m_channel = 49;
  if (m_sector == "IT1TopUSector2") m_channel = 50;
  if (m_sector == "IT1TopUSector3") m_channel = 51;
  if (m_sector == "IT1TopUSector4") m_channel = 52;
  if (m_sector == "IT1TopUSector5") m_channel = 53;
  if (m_sector == "IT1TopUSector6") m_channel = 54;
  if (m_sector == "IT1TopUSector7") m_channel = 55;
  if (m_sector == "IT1TopX1Sector1") m_channel = 21;
  if (m_sector == "IT1TopX1Sector2") m_channel = 22;
  if (m_sector == "IT1TopX1Sector3") m_channel = 23;
  if (m_sector == "IT1TopX1Sector4") m_channel = 24;
  if (m_sector == "IT1TopX1Sector5") m_channel = 25;
  if (m_sector == "IT1TopX1Sector6") m_channel = 26;
  if (m_sector == "IT1TopX1Sector7") m_channel = 27;

}

int NameITSector::GetUniqueSector(){
  return m_uniqueSector;
}

int NameITSector::GetUniqueLayer(){
  return m_uniqueLayer;
}

int NameITSector::GetUniqueBox(){
  return m_uniqueBox;
}

int NameITSector::GetNumberOfSensors(){
  return m_numberOfSensors;
}

int NameITSector::GetSectorID(){
  return m_sectorID;
}

int NameITSector::GetStationID(){
  return m_stationID;
}

std::string NameITSector::GetBoxName(){
  return m_box;
}

std::string NameITSector::GetLayerName(){
  return m_layer;
}

int NameITSector::GetChannel(){
  return m_channel;
}


