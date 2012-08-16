// Include files 
#include "Event/Track.h"
//#include "Kernel/HitPattern.h"


// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "Event/OTTime.h"
#include "Event/STCluster.h"
#include "Event/VeloCluster.h"

#include "Event/KalmanFitResult.h"

#include "TMVA/TMVAClassification_MLP_fittedVelo.C"
#include "TMVA/TMVAClassification_MLP_fittedUpstream.C"
#include "TMVA/TMVAClassification_MLP_fittedDownstream.C"
#include "TMVA/TMVAClassification_MLP_fittedTtrack.C"
#include "TMVA/TMVAClassification_MLP_fittedLong.C"
#include "TMVA/TMVAClassification_MLP_fittedLong_MC2012.C"

// local
#include "TrackNNGhostId.h"


#include "Kernel/HitPattern.h"

#include "TrackInterfaces/IHitExpectation.h"
#include "TrackInterfaces/IVeloExpectation.h"

//boost
#include <boost/assign/list_of.hpp>

//#include "Event/RecSummary.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackNNGhostId
//
// 2009-10-06 : Johannes Albrecht
// 2012-08-16 : JA: implement new tuning on MC2012
//
//  Tool to calculate a track quality variable using a trained (TMVA) netork
//
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TrackNNGhostId )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  TrackNNGhostId::TrackNNGhostId( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
    : GaudiTool ( type, name , parent ),
      m_otdecoder(0),
      m_readerVelo(0),
      m_readerUpstream(0),
      m_readerDownstream(0),
      m_readerTtrack(0),
      m_readerLong(0),
      m_readerLong12(0),
      m_ttExpectation(0),
      m_itExpectation(0),
      m_otExpectation(0),
      m_veloExpectation(0),
      m_FlattenLookupTable(0)
{
  declareInterface<ITrackManipulator>(this);
  declareProperty("IsMC2012Tuning",m_tuningMC12=true);
}

//=============================================================================
// Destructor
//=============================================================================
TrackNNGhostId::~TrackNNGhostId() {}

//=============================================================================

StatusCode TrackNNGhostId::initialize()
{

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure())
  {
    return Error("Failed to initialize", sc);
  }

  /**
   *   initialize input variable names as used for training
   *   and class readers for all track types
   *
   */
  //velo tracks
  m_inNames.clear();
  m_inNames.push_back( "track_probChi2" );
  m_inNames.push_back( "nVeloHits+nTTHits+nITHits+nOTHits" );
  m_inNames.push_back( "track_veloHits" );
  m_readerVelo = new ReadMLP_fittedVelo( m_inNames );

  //upstream tracks
  m_inNames.clear();
  m_inNames.push_back( "track_probChi2" );
  m_inNames.push_back( "track_ttHits" );
  m_inNames.push_back( "nVeloHits+nTTHits+nITHits+nOTHits" );
  m_inNames.push_back( "track_veloHits" );
  m_readerUpstream = new ReadMLP_fittedUpstream( m_inNames );

  //downstream tracks
  m_inNames.clear();
  m_inNames.push_back( "track_probChi2" );
  m_inNames.push_back( "track_ttHits" );
  m_inNames.push_back( "nVeloHits+nTTHits+nITHits+nOTHits" );
  m_inNames.push_back( "track_itHits" );
  m_inNames.push_back( "track_otHits" );
  m_readerDownstream = new ReadMLP_fittedDownstream( m_inNames );

  //Ttracks
  m_inNames.clear();
  m_inNames.push_back( "track_probChi2" );
  m_inNames.push_back( "nVeloHits+nTTHits+nITHits+nOTHits" );
  m_inNames.push_back( "track_itHits" );
  m_inNames.push_back( "track_otHits" );
  m_readerTtrack = new ReadMLP_fittedTtrack( m_inNames );

  //long tracks
  m_inNames.clear();
  m_inNames.push_back( "track_probChi2" );
  m_inNames.push_back( "track_fitMatchChi2" );
  m_inNames.push_back( "track_ttHits" );
  m_inNames.push_back( "track_nCandCommonHits" );
  m_inNames.push_back( "nVeloHits+nTTHits+nITHits+nOTHits" );
  m_inNames.push_back( "track_veloHits" );
  m_inNames.push_back( "track_itHits" );
  m_inNames.push_back( "track_otHits" );
  m_readerLong = new ReadMLP_fittedLong( m_inNames );

  //MC2012 tuning for long tracks
  m_inNames.clear();
  m_inNames.push_back("track_Chi2");
  m_inNames.push_back("track_Dof");
  m_inNames.push_back("track_MatchChi2");
  m_inNames.push_back("Vchi*Vdof");
  m_inNames.push_back("Vdof");
  m_inNames.push_back("Tchi*Tdof");
  m_inNames.push_back("Tdof");
  m_inNames.push_back("observedV");
  m_inNames.push_back("observedIT");
  m_inNames.push_back("observedTT");
  m_inNames.push_back("observedOT");
  m_inNames.push_back("(observedV-expectedV)");
  m_inNames.push_back("(observedIT-expectedIT)");
  m_inNames.push_back("(observedTT-expectedTT)");
  m_inNames.push_back("(observedOT-expectedOT)");
  m_inNames.push_back("OThits");
  m_inNames.push_back("IThits");
  m_inNames.push_back("TThits");
  m_inNames.push_back("VELOhits");
  m_inNames.push_back("(OTbaddrifttime+OToutliers)");
  m_inNames.push_back("eta");
  m_inNames.push_back("pt");
  m_readerLong12 = new ReadMLP_fittedLong12( m_inNames );

  m_inputVec = new std::vector<double>;

  m_otdecoder = tool<IOTRawBankDecoder>("OTRawBankDecoder");

  m_veloExpectation = tool<IVeloExpectation>("VeloExpectation");
  m_ttExpectation = tool<IHitExpectation>("TTHitExpectation");
  m_itExpectation = tool<IHitExpectation>("ITHitExpectation");
  m_otExpectation = tool<IHitExpectation>("OTHitExpectation");

   // Look up table for flattening ...
  // Initialisation (should do this just once, slow)
  const double input[1000] = {-0.2, -0.1986, -0.1972, -0.1958, -0.1944, -0.193, -0.1916, -0.1902, -0.1888, -0.1874, -0.186, -0.1846, -0.1832, -0.1818, -0.1804, -0.179, -0.1776, -0.1762, -0.1748, -0.1734, -0.172, -0.1706, -0.1692, -0.1678, -0.1664, -0.165, -0.1636, -0.1622, -0.1608, -0.1594, -0.158, -0.1566, -0.1552, -0.1538, -0.1524, -0.151, -0.1496, -0.1482, -0.1468, -0.1454, -0.144, -0.1426, -0.1412, -0.1398, -0.1384, -0.137, -0.1356, -0.1342, -0.1328, -0.1314, -0.13, -0.1286, -0.1272, -0.1258, -0.1244, -0.123, -0.1216, -0.1202, -0.1188, -0.1174, -0.116, -0.1146, -0.1132, -0.1118, -0.1104, -0.109, -0.1076, -0.1062, -0.1048, -0.1034, -0.102, -0.1006, -0.0992, -0.0978, -0.0964, -0.095, -0.0936, -0.0922, -0.0908, -0.0894, -0.088, -0.0866, -0.0852, -0.0838, -0.0824, -0.081, -0.0796, -0.0782, -0.0768, -0.0754, -0.074, -0.0726, -0.0712, -0.0698, -0.0684, -0.067, -0.0656, -0.0642, -0.0628, -0.0614, -0.06, -0.0586, -0.0572, -0.0558, -0.0544, -0.053, -0.0516, -0.0502, -0.0488, -0.0474, -0.046, -0.0446, -0.0432, -0.0418, -0.0404, -0.039, -0.0376, -0.0362, -0.0348, -0.0334, -0.032, -0.0306, -0.0292, -0.0278, -0.0264, -0.025, -0.0236, -0.0222, -0.0208, -0.0194, -0.018, -0.0166, -0.0152, -0.0138, -0.0124, -0.011, -0.0096, -0.0082, -0.0068, -0.0054, -0.004, -0.0026, -0.0012, 0.0002, 0.0016, 0.003, 0.0044, 0.0058, 0.0072, 0.0086, 0.01, 0.0114, 0.0128, 0.0142, 0.0156, 0.017, 0.0184, 0.0198, 0.0212, 0.0226, 0.024, 0.0254, 0.0268, 0.0282, 0.0296, 0.031, 0.0324, 0.0338, 0.0352, 0.0366, 0.038, 0.0394, 0.0408, 0.0422, 0.0436, 0.045, 0.0464, 0.0478, 0.0492, 0.0506, 0.052, 0.0534, 0.0548, 0.0562, 0.0576, 0.059, 0.0604, 0.0618, 0.0632, 0.0646, 0.066, 0.0674, 0.0688, 0.0702, 0.0716, 0.073, 0.0744, 0.0758, 0.0772, 0.0786, 0.08, 0.0814, 0.0828, 0.0842, 0.0856, 0.087, 0.0884, 0.0898, 0.0912, 0.0926, 0.094, 0.0954, 0.0968, 0.0982, 0.0996, 0.101, 0.1024, 0.1038, 0.1052, 0.1066, 0.108, 0.1094, 0.1108, 0.1122, 0.1136, 0.115, 0.1164, 0.1178, 0.1192, 0.1206, 0.122, 0.1234, 0.1248, 0.1262, 0.1276, 0.129, 0.1304, 0.1318, 0.1332, 0.1346, 0.136, 0.1374, 0.1388, 0.1402, 0.1416, 0.143, 0.1444, 0.1458, 0.1472, 0.1486, 0.15, 0.1514, 0.1528, 0.1542, 0.1556, 0.157, 0.1584, 0.1598, 0.1612, 0.1626, 0.164, 0.1654, 0.1668, 0.1682, 0.1696, 0.171, 0.1724, 0.1738, 0.1752, 0.1766, 0.178, 0.1794, 0.1808, 0.1822, 0.1836, 0.185, 0.1864, 0.1878, 0.1892, 0.1906, 0.192, 0.1934, 0.1948, 0.1962, 0.1976, 0.199, 0.2004, 0.2018, 0.2032, 0.2046, 0.206, 0.2074, 0.2088, 0.2102, 0.2116, 0.213, 0.2144, 0.2158, 0.2172, 0.2186, 0.22, 0.2214, 0.2228, 0.2242, 0.2256, 0.227, 0.2284, 0.2298, 0.2312, 0.2326, 0.234, 0.2354, 0.2368, 0.2382, 0.2396, 0.241, 0.2424, 0.2438, 0.2452, 0.2466, 0.248, 0.2494, 0.2508, 0.2522, 0.2536, 0.255, 0.2564, 0.2578, 0.2592, 0.2606, 0.262, 0.2634, 0.2648, 0.2662, 0.2676, 0.269, 0.2704, 0.2718, 0.2732, 0.2746, 0.276, 0.2774, 0.2788, 0.2802, 0.2816, 0.283, 0.2844, 0.2858, 0.2872, 0.2886, 0.29, 0.2914, 0.2928, 0.2942, 0.2956, 0.297, 0.2984, 0.2998, 0.3012, 0.3026, 0.304, 0.3054, 0.3068, 0.3082, 0.3096, 0.311, 0.3124, 0.3138, 0.3152, 0.3166, 0.318, 0.3194, 0.3208, 0.3222, 0.3236, 0.325, 0.3264, 0.3278, 0.3292, 0.3306, 0.332, 0.3334, 0.3348, 0.3362, 0.3376, 0.339, 0.3404, 0.3418, 0.3432, 0.3446, 0.346, 0.3474, 0.3488, 0.3502, 0.3516, 0.353, 0.3544, 0.3558, 0.3572, 0.3586, 0.36, 0.3614, 0.3628, 0.3642, 0.3656, 0.367, 0.3684, 0.3698, 0.3712, 0.3726, 0.374, 0.3754, 0.3768, 0.3782, 0.3796, 0.381, 0.3824, 0.3838, 0.3852, 0.3866, 0.388, 0.3894, 0.3908, 0.3922, 0.3936, 0.395, 0.3964, 0.3978, 0.3992, 0.4006, 0.402, 0.4034, 0.4048, 0.4062, 0.4076, 0.409, 0.4104, 0.4118, 0.4132, 0.4146, 0.416, 0.4174, 0.4188, 0.4202, 0.4216, 0.423, 0.4244, 0.4258, 0.4272, 0.4286, 0.43, 0.4314, 0.4328, 0.4342, 0.4356, 0.437, 0.4384, 0.4398, 0.4412, 0.4426, 0.444, 0.4454, 0.4468, 0.4482, 0.4496, 0.451, 0.4524, 0.4538, 0.4552, 0.4566, 0.458, 0.4594, 0.4608, 0.4622, 0.4636, 0.465, 0.4664, 0.4678, 0.4692, 0.4706, 0.472, 0.4734, 0.4748, 0.4762, 0.4776, 0.479, 0.4804, 0.4818, 0.4832, 0.4846, 0.486, 0.4874, 0.4888, 0.4902, 0.4916, 0.493, 0.4944, 0.4958, 0.4972, 0.4986, 0.5, 0.5014, 0.5028, 0.5042, 0.5056, 0.507, 0.5084, 0.5098, 0.5112, 0.5126, 0.514, 0.5154, 0.5168, 0.5182, 0.5196, 0.521, 0.5224, 0.5238, 0.5252, 0.5266, 0.528, 0.5294, 0.5308, 0.5322, 0.5336, 0.535, 0.5364, 0.5378, 0.5392, 0.5406, 0.542, 0.5434, 0.5448, 0.5462, 0.5476, 0.549, 0.5504, 0.5518, 0.5532, 0.5546, 0.556, 0.5574, 0.5588, 0.5602, 0.5616, 0.563, 0.5644, 0.5658, 0.5672, 0.5686, 0.57, 0.5714, 0.5728, 0.5742, 0.5756, 0.577, 0.5784, 0.5798, 0.5812, 0.5826, 0.584, 0.5854, 0.5868, 0.5882, 0.5896, 0.591, 0.5924, 0.5938, 0.5952, 0.5966, 0.598, 0.5994, 0.6008, 0.6022, 0.6036, 0.605, 0.6064, 0.6078, 0.6092, 0.6106, 0.612, 0.6134, 0.6148, 0.6162, 0.6176, 0.619, 0.6204, 0.6218, 0.6232, 0.6246, 0.626, 0.6274, 0.6288, 0.6302, 0.6316, 0.633, 0.6344, 0.6358, 0.6372, 0.6386, 0.64, 0.6414, 0.6428, 0.6442, 0.6456, 0.647, 0.6484, 0.6498, 0.6512, 0.6526, 0.654, 0.6554, 0.6568, 0.6582, 0.6596, 0.661, 0.6624, 0.6638, 0.6652, 0.6666, 0.668, 0.6694, 0.6708, 0.6722, 0.6736, 0.675, 0.6764, 0.6778, 0.6792, 0.6806, 0.682, 0.6834, 0.6848, 0.6862, 0.6876, 0.689, 0.6904, 0.6918, 0.6932, 0.6946, 0.696, 0.6974, 0.6988, 0.7002, 0.7016, 0.703, 0.7044, 0.7058, 0.7072, 0.7086, 0.71, 0.7114, 0.7128, 0.7142, 0.7156, 0.717, 0.7184, 0.7198, 0.7212, 0.7226, 0.724, 0.7254, 0.7268, 0.7282, 0.7296, 0.731, 0.7324, 0.7338, 0.7352, 0.7366, 0.738, 0.7394, 0.7408, 0.7422, 0.7436, 0.745, 0.7464, 0.7478, 0.7492, 0.7506, 0.752, 0.7534, 0.7548, 0.7562, 0.7576, 0.759, 0.7604, 0.7618, 0.7632, 0.7646, 0.766, 0.7674, 0.7688, 0.7702, 0.7716, 0.773, 0.7744, 0.7758, 0.7772, 0.7786, 0.78, 0.7814, 0.7828, 0.7842, 0.7856, 0.787, 0.7884, 0.7898, 0.7912, 0.7926, 0.794, 0.7954, 0.7968, 0.7982, 0.7996, 0.801, 0.8024, 0.8038, 0.8052, 0.8066, 0.808, 0.8094, 0.8108, 0.8122, 0.8136, 0.815, 0.8164, 0.8178, 0.8192, 0.8206, 0.822, 0.8234, 0.8248, 0.8262, 0.8276, 0.829, 0.8304, 0.8318, 0.8332, 0.8346, 0.836, 0.8374, 0.8388, 0.8402, 0.8416, 0.843, 0.8444, 0.8458, 0.8472, 0.8486, 0.85, 0.8514, 0.8528, 0.8542, 0.8556, 0.857, 0.8584, 0.8598, 0.8612, 0.8626, 0.864, 0.8654, 0.8668, 0.8682, 0.8696, 0.871, 0.8724, 0.8738, 0.8752, 0.8766, 0.878, 0.8794, 0.8808, 0.8822, 0.8836, 0.885, 0.8864, 0.8878, 0.8892, 0.8906, 0.892, 0.8934, 0.8948, 0.8962, 0.8976, 0.899, 0.9004, 0.9018, 0.9032, 0.9046, 0.906, 0.9074, 0.9088, 0.9102, 0.9116, 0.913, 0.9144, 0.9158, 0.9172, 0.9186, 0.92, 0.9214, 0.9228, 0.9242, 0.9256, 0.927, 0.9284, 0.9298, 0.9312, 0.9326, 0.934, 0.9354, 0.9368, 0.9382, 0.9396, 0.941, 0.9424, 0.9438, 0.9452, 0.9466, 0.948, 0.9494, 0.9508, 0.9522, 0.9536, 0.955, 0.9564, 0.9578, 0.9592, 0.9606, 0.962, 0.9634, 0.9648, 0.9662, 0.9676, 0.969, 0.9704, 0.9718, 0.9732, 0.9746, 0.976, 0.9774, 0.9788, 0.9802, 0.9816, 0.983, 0.9844, 0.9858, 0.9872, 0.9886, 0.99, 0.9914, 0.9928, 0.9942, 0.9956, 0.997, 0.9984, 0.9998, 1.0012, 1.0026, 1.004, 1.0054, 1.0068, 1.0082, 1.0096, 1.011, 1.0124, 1.0138, 1.0152, 1.0166, 1.018, 1.0194, 1.0208, 1.0222, 1.0236, 1.025, 1.0264, 1.0278, 1.0292, 1.0306, 1.032, 1.0334, 1.0348, 1.0362, 1.0376, 1.039, 1.0404, 1.0418, 1.0432, 1.0446, 1.046, 1.0474, 1.0488, 1.0502, 1.0516, 1.053, 1.0544, 1.0558, 1.0572, 1.0586, 1.06, 1.0614, 1.0628, 1.0642, 1.0656, 1.067, 1.0684, 1.0698, 1.0712, 1.0726, 1.074, 1.0754, 1.0768, 1.0782, 1.0796, 1.081, 1.0824, 1.0838, 1.0852, 1.0866, 1.088, 1.0894, 1.0908, 1.0922, 1.0936, 1.095, 1.0964, 1.0978, 1.0992, 1.1006, 1.102, 1.1034, 1.1048, 1.1062, 1.1076, 1.109, 1.1104, 1.1118, 1.1132, 1.1146, 1.116, 1.1174, 1.1188, 1.1202, 1.1216, 1.123, 1.1244, 1.1258, 1.1272, 1.1286, 1.13, 1.1314, 1.1328, 1.1342, 1.1356, 1.137, 1.1384, 1.1398, 1.1412, 1.1426, 1.144, 1.1454, 1.1468, 1.1482, 1.1496, 1.151, 1.1524, 1.1538, 1.1552, 1.1566, 1.158, 1.1594, 1.1608, 1.1622, 1.1636, 1.165, 1.1664, 1.1678, 1.1692, 1.1706, 1.172, 1.1734, 1.1748, 1.1762, 1.1776, 1.179, 1.1804, 1.1818, 1.1832, 1.1846, 1.186, 1.1874, 1.1888, 1.1902, 1.1916, 1.193, 1.1944, 1.1958, 1.1972, 1.1986};
  const double output[1000] = {0.999566, 0.999537, 0.999532, 0.999519, 0.999501, 0.999491, 0.999468, 0.999455, 0.999424, 0.999406, 0.999375, 0.999346, 0.999313, 0.999284, 0.999251, 0.999209, 0.999183, 0.999139, 0.999088, 0.999046, 0.999013, 0.99899, 0.998927, 0.998886, 0.998858, 0.998796, 0.998736, 0.998679, 0.99861, 0.99855, 0.998486, 0.998424, 0.998351, 0.998284, 0.998201, 0.998126, 0.998038, 0.997938, 0.997847, 0.997775, 0.997659, 0.997563, 0.99748, 0.997379, 0.997273, 0.997149, 0.996997, 0.996868, 0.996746, 0.99662, 0.996485, 0.996302, 0.996131, 0.995989, 0.995821, 0.99563, 0.995436, 0.995278, 0.995082, 0.994893, 0.994663, 0.994446, 0.994188, 0.99394, 0.993702, 0.993467, 0.993231, 0.992999, 0.992759, 0.992469, 0.992174, 0.991867, 0.991583, 0.991262, 0.990939, 0.990585, 0.990246, 0.989846, 0.989471, 0.989089, 0.988644, 0.988249, 0.987771, 0.987409, 0.986905, 0.986481, 0.986, 0.985483, 0.984941, 0.98431, 0.983716, 0.983111, 0.98247, 0.981803, 0.981002, 0.980294, 0.979573, 0.978738, 0.977927, 0.977053, 0.976048, 0.975012, 0.973957, 0.972851, 0.971631, 0.970447, 0.969093, 0.967783, 0.966256, 0.964633, 0.962824, 0.960968, 0.959035, 0.95696, 0.954471, 0.952008, 0.949292, 0.946327, 0.943001, 0.939458, 0.935693, 0.931413, 0.926916, 0.921845, 0.916377, 0.910546, 0.904116, 0.897053, 0.889153, 0.880735, 0.871186, 0.860805, 0.849821, 0.837597, 0.824714, 0.81075, 0.796332, 0.78111, 0.76536, 0.74905, 0.732862, 0.715998, 0.699063, 0.68244, 0.665463, 0.648796, 0.632582, 0.617494, 0.602975, 0.588988, 0.575188, 0.562186, 0.550005, 0.53837, 0.526984, 0.516476, 0.506363, 0.49686, 0.488114, 0.479782, 0.4719, 0.464413, 0.457451, 0.45076, 0.444539, 0.438592, 0.433087, 0.428004, 0.42305, 0.418178, 0.41388, 0.409675, 0.405863, 0.402147, 0.398625, 0.395118, 0.391996, 0.388866, 0.385904, 0.383139, 0.380459, 0.377872, 0.375393, 0.373055, 0.370669, 0.368434, 0.366377, 0.364356, 0.362236, 0.360347, 0.358409, 0.356507, 0.354649, 0.352943, 0.351134, 0.349467, 0.347834, 0.346169, 0.344588, 0.343078, 0.341478, 0.339907, 0.338444, 0.336966, 0.335532, 0.334064, 0.332661, 0.331257, 0.329833, 0.328438, 0.327029, 0.325636, 0.324463, 0.323165, 0.321816, 0.320597, 0.319302, 0.317981, 0.316756, 0.315578, 0.314399, 0.31313, 0.311856, 0.310618, 0.309466, 0.308303, 0.307171, 0.306036, 0.30486, 0.303803, 0.30264, 0.301552, 0.3005, 0.299386, 0.298283, 0.297187, 0.296133, 0.295078, 0.294112, 0.293194, 0.292101, 0.291013, 0.289881, 0.288775, 0.287759, 0.286671, 0.285674, 0.284681, 0.283733, 0.282735, 0.281709, 0.280779, 0.279823, 0.27889, 0.277843, 0.276926, 0.27599, 0.275166, 0.274245, 0.273367, 0.272488, 0.271584, 0.270798, 0.27002, 0.269118, 0.268221, 0.26733, 0.266492, 0.265598, 0.264748, 0.263908, 0.263006, 0.26206, 0.261199, 0.260388, 0.259533, 0.25868, 0.257871, 0.256979, 0.256111, 0.25524, 0.254439, 0.253676, 0.252847, 0.251991, 0.251051, 0.250205, 0.249394, 0.248673, 0.247851, 0.247158, 0.246339, 0.24551, 0.244773, 0.24406, 0.243272, 0.242494, 0.241744, 0.240969, 0.240219, 0.239478, 0.238775, 0.238079, 0.237358, 0.236635, 0.235909, 0.235138, 0.23434, 0.233673, 0.232973, 0.232303, 0.231611, 0.230843, 0.230145, 0.229471, 0.228763, 0.228062, 0.227393, 0.226664, 0.225935, 0.225292, 0.224623, 0.223979, 0.223297, 0.222599, 0.222023, 0.221338, 0.220671, 0.22009, 0.219441, 0.218802, 0.218115, 0.217528, 0.216921, 0.216368, 0.215755, 0.215125, 0.214505, 0.213882, 0.213316, 0.212685, 0.212055, 0.211471, 0.210871, 0.210318, 0.20976, 0.209059, 0.208434, 0.207837, 0.207255, 0.206726, 0.206111, 0.205498, 0.204914, 0.204369, 0.20381, 0.203195, 0.20264, 0.202125, 0.20157, 0.201063, 0.200531, 0.199965, 0.199363, 0.198802, 0.198215, 0.197693, 0.197135, 0.196587, 0.196016, 0.195476, 0.194879, 0.194326, 0.19377, 0.193279, 0.192736, 0.192196, 0.19162, 0.19109, 0.190664, 0.190206, 0.189679, 0.189147, 0.188656, 0.188092, 0.187609, 0.187064, 0.186593, 0.186063, 0.185547, 0.185053, 0.18458, 0.184017, 0.183554, 0.183089, 0.182541, 0.182011, 0.181489, 0.180959, 0.180463, 0.179962, 0.179486, 0.178998, 0.178602, 0.178176, 0.177662, 0.177212, 0.176736, 0.176281, 0.175832, 0.175398, 0.174927, 0.174459, 0.173997, 0.17356, 0.173048, 0.172606, 0.172165, 0.171728, 0.171291, 0.17088, 0.170433, 0.169973, 0.16957, 0.169092, 0.168619, 0.168229, 0.167794, 0.167365, 0.16691, 0.166502, 0.166143, 0.165784, 0.165396, 0.164923, 0.164481, 0.164096, 0.163659, 0.163238, 0.162858, 0.16246, 0.162054, 0.161703, 0.161276, 0.160858, 0.160447, 0.160031, 0.159674, 0.159199, 0.15878, 0.158392, 0.157943, 0.157534, 0.157175, 0.156759, 0.156345, 0.155898, 0.155529, 0.155133, 0.154671, 0.154291, 0.153934, 0.153565, 0.153172, 0.1528, 0.152384, 0.15198, 0.151619, 0.151246, 0.150885, 0.150531, 0.150117, 0.149706, 0.149321, 0.148964, 0.148616, 0.148241, 0.14784, 0.147525, 0.147181, 0.146819, 0.146509, 0.146153, 0.145755, 0.145357, 0.144977, 0.144594, 0.144165, 0.143765, 0.143442, 0.143031, 0.142687, 0.142346, 0.142023, 0.141664, 0.141315, 0.14095, 0.14056, 0.140139, 0.139805, 0.139472, 0.139082, 0.138761, 0.138446, 0.138113, 0.13774, 0.137438, 0.137066, 0.136743, 0.136368, 0.135999, 0.135606, 0.135293, 0.134944, 0.134626, 0.134321, 0.13398, 0.133567, 0.133213, 0.132866, 0.132535, 0.132184, 0.131882, 0.131522, 0.131179, 0.130869, 0.130486, 0.130135, 0.129796, 0.129473, 0.129137, 0.128804, 0.128504, 0.128132, 0.12777, 0.127408, 0.127054, 0.126754, 0.126418, 0.126118, 0.125733, 0.125387, 0.125033, 0.124663, 0.124335, 0.123989, 0.123627, 0.12333, 0.123027, 0.122684, 0.122348, 0.121986, 0.121689, 0.121368, 0.121053, 0.120686, 0.120342, 0.12003, 0.119691, 0.119345, 0.119006, 0.118657, 0.118355, 0.118102, 0.117791, 0.117419, 0.117104, 0.116719, 0.116391, 0.116008, 0.115683, 0.115372, 0.115044, 0.114675, 0.114303, 0.113943, 0.113636, 0.113248, 0.112904, 0.112514, 0.112191, 0.111876, 0.111501, 0.111155, 0.110808, 0.110485, 0.110155, 0.109816, 0.109516, 0.109216, 0.108891, 0.108547, 0.108219, 0.107886, 0.107583, 0.10725, 0.106911, 0.106588, 0.106247, 0.105808, 0.105472, 0.105074, 0.104743, 0.104386, 0.104066, 0.103787, 0.103479, 0.1032, 0.102931, 0.102624, 0.102321, 0.102016, 0.101686, 0.101375, 0.101034, 0.100742, 0.100427, 0.100117, 0.0998119, 0.0994707, 0.0991399, 0.098835, 0.0985145, 0.098163, 0.0977779, 0.0973877, 0.0970879, 0.0967649, 0.0963798, 0.0960257, 0.0957337, 0.0953925, 0.0950178, 0.0946301, 0.0943226, 0.0940047, 0.0936843, 0.0933638, 0.0930459, 0.0927332, 0.0923688, 0.0920561, 0.0917098, 0.0913997, 0.0910378, 0.0906683, 0.0903504, 0.0900403, 0.0897017, 0.0893425, 0.0890065, 0.0886266, 0.0883036, 0.0879883, 0.0876755, 0.0874145, 0.0870579, 0.0867426, 0.0863989, 0.0861146, 0.0857864, 0.0854504, 0.0850886, 0.0847345, 0.0844476, 0.0841608, 0.0838041, 0.0835095, 0.0831503, 0.0828608, 0.0825507, 0.082194, 0.0818891, 0.0815609, 0.0812223, 0.0808967, 0.0805478, 0.0802222, 0.0798655, 0.0795554, 0.0792246, 0.0789145, 0.0785966, 0.0782348, 0.0778807, 0.0775163, 0.0771519, 0.0768702, 0.0765135, 0.0761724, 0.0758416, 0.0755289, 0.0752679, 0.0749733, 0.0746424, 0.0743272, 0.0739808, 0.0736655, 0.0733167, 0.0729445, 0.0726163, 0.0723113, 0.0719598, 0.0716446, 0.0712827, 0.0709442, 0.0705927, 0.0703007, 0.069944, 0.0695796, 0.0691997, 0.0688327, 0.0684502, 0.068122, 0.0677214, 0.0673234, 0.0669306, 0.0666127, 0.0662509, 0.065915, 0.065535, 0.0651758, 0.0648218, 0.064447, 0.0641188, 0.0637441, 0.0633822, 0.0630618, 0.062669, 0.062333, 0.0619738, 0.0616197, 0.0612811, 0.060834, 0.0604541, 0.0600587, 0.0596969, 0.0593506, 0.0589888, 0.0585701, 0.0582393, 0.0579085, 0.0575596, 0.0571978, 0.0567972, 0.0564225, 0.0561098, 0.0557402, 0.0553215, 0.0549623, 0.0546005, 0.0542232, 0.0538148, 0.0534453, 0.0530731, 0.0527242, 0.0523366, 0.0519928, 0.0515742, 0.0512098, 0.0508376, 0.0504112, 0.0500158, 0.0496514, 0.0492379, 0.0488631, 0.0485091, 0.0481498, 0.0477389, 0.0473099, 0.0469042, 0.0465553, 0.0461573, 0.0458161, 0.0454104, 0.0449891, 0.0445937, 0.0441931, 0.0438287, 0.0434643, 0.0431387, 0.0426968, 0.0422393, 0.0418904, 0.0415364, 0.0410815, 0.0406654, 0.0402778, 0.0398462, 0.0394146, 0.0390218, 0.0386238, 0.0382542, 0.0378691, 0.0374737, 0.0370576, 0.0365485, 0.0360833, 0.0356026, 0.0351633, 0.0347032, 0.0342949, 0.0338297, 0.0334007, 0.0329588, 0.0325659, 0.0321576, 0.0316743, 0.0311833, 0.0306974, 0.0303201, 0.0298161, 0.0293458, 0.0289349, 0.0284412, 0.0280148, 0.027454, 0.0269423, 0.0264694, 0.02603, 0.0255597, 0.0250066, 0.0245, 0.0239987, 0.0234818, 0.0229649, 0.022417, 0.0218252, 0.0212954, 0.0207113, 0.0201789, 0.0195328, 0.01891, 0.0182639, 0.0176566, 0.0170699, 0.0165169, 0.0158914, 0.0152634, 0.0146638, 0.0140255, 0.0133406, 0.0126971, 0.0121001, 0.0113946, 0.0107588, 0.0101076, 0.00939427, 0.00871716, 0.00797027, 0.0072208, 0.00654885, 0.0058614, 0.00524373, 0.00465449, 0.00412986, 0.00366467, 0.00320982, 0.00279114, 0.0024681, 0.00213988, 0.00184267, 0.00159457, 0.00140333, 0.00126377, 0.00116298, 0.00106735, 0.000974316, 0.000901953, 0.000821837, 0.000767565, 0.000695202, 0.000656436, 0.000609917, 0.000573735, 0.000537554, 0.00051171, 0.000475528, 0.000444516, 0.000410919, 0.000390243, 0.000364399, 0.000338556, 0.000310127, 0.000294621, 0.000286868, 0.000255855, 0.000245517, 0.000237764, 0.000219673, 0.000204167, 0.000183492, 0.000167986, 0.000157648, 0.000144726, 0.000136973, 0.000131804, 0.000124051, 0.000113713, 9.82069e-05, 9.56226e-05, 8.78694e-05, 8.01162e-05, 7.75318e-05, 6.97786e-05, 6.71942e-05, 6.20254e-05, 5.68567e-05, 5.42723e-05, 5.42723e-05, 5.42723e-05, 5.42723e-05, 5.16879e-05, 4.91035e-05, 4.91035e-05, 4.65191e-05, 4.39347e-05, 3.87659e-05, 3.35971e-05, 3.35971e-05, 2.84283e-05, 2.84283e-05, 2.84283e-05, 2.32595e-05, 2.32595e-05, 1.80908e-05, 1.80908e-05, 1.80908e-05, 1.80908e-05, 1.80908e-05, 1.55064e-05, 1.55064e-05, 1.55064e-05, 1.55064e-05, 1.55064e-05, 1.2922e-05, 1.2922e-05, 1.2922e-05, 1.2922e-05, 1.2922e-05, 1.2922e-05, 1.2922e-05, 1.2922e-05, 1.03376e-05, 1.03376e-05, 7.75318e-06, 7.75318e-06, 7.75318e-06, 7.75318e-06, 5.16879e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 2.58439e-06, 0, 0, 0, 0};
  m_FlattenLookupTable = new Rich::TabulatedFunction1D( input, output, 1000, gsl_interp_cspline );
  if ( !m_FlattenLookupTable->valid() ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}


StatusCode TrackNNGhostId::execute(LHCb::Track& aTrack) const
{

  const bool isDebug   = msgLevel(MSG::DEBUG);
  double retval = 0;
  if ( UNLIKELY( isDebug ) ) debug() << "==> Execute" << endmsg;
  //always()<<endmsg;
  //always()<<"--> execute, m_tuningMC12 = "<<m_tuningMC12<<endmsg;

  int veloHits=0,ttHits=0,itHits=0,otHits=0;
  const std::vector<LHCb::LHCbID> ids = aTrack.lhcbIDs();
  for(unsigned int i = 0; i < ids.size(); ++i){
    if(ids[i].isVelo()) ++veloHits;
    if(ids[i].isTT()) ++ttHits;
    if(ids[i].isIT()) ++itHits;
    if(ids[i].isOT()) ++otHits;
  }


  int nVeloCont = 0;
  int nTTCont = 0;
  int nITCont = 0;


  if (exist<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default)){
    LHCb::VeloClusters* veloCont = get<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default);
    nVeloCont = veloCont->size();
  }
  if (exist<LHCb::STClusters>(LHCb::STClusterLocation::TTClusters)){
    LHCb::STClusters *ttCont = get<LHCb::STClusters>(LHCb::STClusterLocation::TTClusters);
    nTTCont = ttCont->size();
  }
  if (exist<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters)){
    LHCb::STClusters *itCont = get<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters);
    nITCont = itCont->size();
  }

  int nHitMult = nVeloCont + nTTCont + nITCont + m_otdecoder->totalNumberOfHits();



  /**
   *   now evaluate the MVA value for different track types
   *
   */
  if( aTrack.checkType(LHCb::Track::Velo) ){
    m_inputVec->clear();
    m_inputVec->reserve(3);
    m_inputVec->push_back( aTrack.probChi2() );
    m_inputVec->push_back( nHitMult );
    m_inputVec->push_back( veloHits );
    // retrive the classifier responses
    retval = m_readerVelo->GetMvaValue( *m_inputVec );
  }//end evaluate velo track

  if( aTrack.checkType(LHCb::Track::Upstream) ){
    m_inputVec->clear();
    m_inputVec->reserve(4);
    m_inputVec->push_back( aTrack.probChi2() );
    m_inputVec->push_back( ttHits );
    m_inputVec->push_back( nHitMult );
    m_inputVec->push_back( veloHits );
    // retrive the classifier responses
    retval = m_readerUpstream->GetMvaValue( *m_inputVec );
  }//end evaluate upstream track

  if( aTrack.checkType(LHCb::Track::Downstream) ){
    m_inputVec->clear();
    m_inputVec->reserve(5);
    m_inputVec->push_back( aTrack.probChi2() );
    m_inputVec->push_back( ttHits );
    m_inputVec->push_back( nHitMult );
    m_inputVec->push_back( itHits );
    m_inputVec->push_back( otHits );
    // retrive the classifier responses
    retval = m_readerDownstream->GetMvaValue( *m_inputVec );
  }//end evaluate downstream track

  if( aTrack.checkType(LHCb::Track::Ttrack) ){
    m_inputVec->clear();
    m_inputVec->reserve(4);
    m_inputVec->push_back( aTrack.probChi2() );
    m_inputVec->push_back( nHitMult );
    m_inputVec->push_back( itHits );
    m_inputVec->push_back( otHits );
    // retrive the classifier responses
    retval = m_readerTtrack->GetMvaValue( *m_inputVec );
  }//end evaluate Ttrack track

  if( !m_tuningMC12 && aTrack.checkType(LHCb::Track::Long) ){
    if ( UNLIKELY( isDebug ) ) debug()<<"run mc09 tuning"<<endmsg;

    m_inputVec->clear();
    m_inputVec->reserve(8);
    m_inputVec->push_back( aTrack.probChi2() );
    m_inputVec->push_back( aTrack.info(LHCb::Track::FitMatchChi2,-60) );
    m_inputVec->push_back( ttHits);
    m_inputVec->push_back( aTrack.info(LHCb::Track::NCandCommonHits,-60));
    m_inputVec->push_back( nHitMult);
    m_inputVec->push_back( veloHits);
    m_inputVec->push_back( itHits);
    m_inputVec->push_back( otHits);
    if(-60 != (*m_inputVec)[1]
       && -60 != (*m_inputVec)[3] ){
      // retrive the classifier responses
      retval = m_readerLong->GetMvaValue( *m_inputVec );
      if ( UNLIKELY( isDebug ) ) debug()<<"MC09:  NN output:  "<<retval<<endmsg;
    } else {
      if( UNLIKELY( isDebug ) ) debug()<<"warning: extra info of track not propetly filled ....  "<<endmsg;
    }
  }//end evaluate long track for Mc09 tuning

  /*
   *   transform MC09 classifier
   *   MC2012 classifier gets transformed separately
   */
  //normalize the output to the interval [0,1]
  if(retval<-1) retval=-1;
  if(retval>1) retval=1;
  retval = -0.5*double(retval-1.);

  if( UNLIKELY ( isDebug ) ) {
    for(unsigned int i=0;i<m_inputVec->size();i++)  debug()<<"input variable [ "<<i<<" ] :"<<(*m_inputVec)[i]<<endmsg;
    debug() << "direct NN output is: "<<retval<<endmsg;
    debug() << "normlized NN output is: "<<retval<<endmsg;
  }


  //MC2012 tuning
  if( m_tuningMC12 && aTrack.checkType(LHCb::Track::Long) ){

    aTrack.setGhostProbability(-888.);
    m_inputVec->clear();
    if (!aTrack.hasInfo(21)) {
      aTrack.setGhostProbability(-999.);
      if ( UNLIKELY( isDebug ) ) debug()<<"21.quit"<<endmsg;
      return Warning("Input variable 21 missing",StatusCode::SUCCESS, 1);
    }

    if (!aTrack.hasInfo(17)) {
      aTrack.setGhostProbability(-999.);
      if ( UNLIKELY( isDebug ) ) debug()<<"17.quit"<<endmsg;
      return Warning("Input variable 17 missing",StatusCode::SUCCESS, 1);
    }
    if (!aTrack.hasInfo(18)) {
      aTrack.setGhostProbability(-999.);
      if ( UNLIKELY( isDebug ) ) debug()<<"18.quit"<<endmsg;
      return Warning("Input variable 18 missing",StatusCode::SUCCESS, 1);
    }

    LHCb::HitPattern observedpattern(aTrack.lhcbIDs());
    LHCb::HitPattern expectedpattern;
    std::vector<LHCb::LHCbID> ids;
    {
      std::vector<LHCb::LHCbID> ids;
      ids.reserve(2 * aTrack.nLHCbIDs());

      std::bitset<23> velo[4];

      m_veloExpectation->expectedInfo(aTrack, velo);
      m_ttExpectation->collect(aTrack, ids);
      m_itExpectation->collect(aTrack, ids);
      m_otExpectation->collect(aTrack, ids);

      expectedpattern = LHCb::HitPattern(ids);
      expectedpattern.setVeloRA(velo[0]);
      expectedpattern.setVeloRC(velo[1]);
      expectedpattern.setVeloPhiA(velo[2]);
      expectedpattern.setVeloPhiC(velo[3]);

    }
    float expectedV;
    {
      std::bitset< 23 > ar = expectedpattern.veloRA()  .to_ulong();
      std::bitset< 23 > cr = expectedpattern.veloRC()  .to_ulong();
      std::bitset< 23 > ap = expectedpattern.veloPhiA().to_ulong();
      std::bitset< 23 > cp = expectedpattern.veloPhiC().to_ulong();
      float ret = 0.;
      for (int i = 0 ; i < 23 ; ++i) {
        if (ar[i]) ret+=1.;
        if (ap[i]) ret+=1.;
        if (cr[i]) ret+=1.;
        if (cp[i]) ret+=1.;
      }
      expectedV = ret;
      if ( UNLIKELY( isDebug ) ) debug() << "expectedV = " << ret << endmsg;
    }
    float observedV;
    {
      std::bitset< 23 > ar = observedpattern.veloRA()  .to_ulong();
      std::bitset< 23 > cr = observedpattern.veloRC()  .to_ulong();
      std::bitset< 23 > ap = observedpattern.veloPhiA().to_ulong();
      std::bitset< 23 > cp = observedpattern.veloPhiC().to_ulong();
      float ret = 0.;
      for (int i = 0 ; i < 23 ; ++i) {
        if (ar[i]) ret+=1.;
        if (ap[i]) ret+=1.;
        if (cr[i]) ret+=1.;
        if (cp[i]) ret+=1.;
      }
      observedV = ret;
      if ( UNLIKELY( isDebug ) ) debug() << "observedV = " << ret << endmsg;
    }


    float observedTT;
    {
      std::bitset< 4 > obs = observedpattern.tt().to_ulong();
      observedTT =  (obs[0]
                     +obs[1]
                     +obs[2]
                     +obs[3]);
      if ( UNLIKELY( isDebug ) ) debug() << "observedTT = " << observedTT << endmsg;
    }

    float expectedTT;
    {
      std::bitset< 4 > exp = expectedpattern.tt().to_ulong();
      expectedTT =  (exp[0]
                     +exp[1]
                     +exp[2]
                     +exp[3]);
      if ( UNLIKELY( isDebug ) ) debug() << "expectedTT = " << expectedTT << endmsg;
    }


    float observedIT;
    {
      std::bitset<12> dump = observedpattern.itAC().to_ulong();
      float ret = 0.;
      for (int i = 0 ; i < 12 ; ++i)
        if (dump[i]) ret+=1.;
      dump = observedpattern.itTopBottom().to_ulong();
      for (int i = 0 ; i < 12 ; ++i)
        if (dump[i]) ret+=1.;
      observedIT = ret;
      if ( UNLIKELY( isDebug ) ) debug() << "observedIT = " << ret << endmsg;
    }

    float expectedIT;
    {
      std::bitset<12> dump = expectedpattern.itAC().to_ulong();
      float ret = 0.;
      for (int i = 0 ; i < 12 ; ++i)
        if (dump[i]) ret+=1.;
      dump = expectedpattern.itTopBottom().to_ulong();
      for (int i = 0 ; i < 12 ; ++i)
        if (dump[i]) ret+=1.;
      expectedIT=  ret;
      if ( UNLIKELY( isDebug ) ) debug() << "expectedIT = " << ret << endmsg;
    }

    float observedOT;
    {
      std::bitset<12> dump = observedpattern.ot1stMonoLayer().to_ulong();
      float ret = 0.;
      for (int i = 0 ; i < 12 ; ++i)
        if (dump[i]) ret+=1.;
      dump = observedpattern.ot2ndMonoLayer().to_ulong();
      for (int i = 0 ; i < 12 ; ++i)
        if (dump[i]) ret+=1.;
      observedOT =  ret;
      if ( UNLIKELY( isDebug ) ) debug() << "observedOT = " << ret << endmsg;
    }

    float expectedOT;
    {
      std::bitset<12> dump = expectedpattern.ot1stMonoLayer().to_ulong();
      float ret = 0.;
      for (int i = 0 ; i < 12 ; ++i)
        if (dump[i]) ret+=1.;
      dump = expectedpattern.ot2ndMonoLayer().to_ulong();
      for (int i = 0 ; i < 12 ; ++i)
        if (dump[i]) ret+=1.;
      expectedOT =  ret;
      if ( UNLIKELY( isDebug ) ) debug() << "expectedOT = " << ret << endmsg;
    }

    const LHCb::KalmanFitResult* kalfit =static_cast<const LHCb::KalmanFitResult*>(aTrack.fitResult()) ;
    if( !kalfit ) {
      aTrack.setGhostProbability(-999.);
      if ( UNLIKELY( isDebug ) ) debug()<<"fit result not avialable... quit"<<endmsg;
      return Warning("kalman fit result not available ... end loop and set ghostProb = -999.",StatusCode::SUCCESS, 1);
    }

    float nMeas = kalfit->nMeasurements( LHCb::Measurement::OT );
    float nOTBad = nMeas-nMeas*aTrack.info(25 , 0 );// info 25 : FitFracUsedOTTimes

    if ( UNLIKELY( isDebug ) ) {
      debug()<<"nOTMeas: "<< nMeas<<endmsg;
      debug()<<"nOTBad: "<<nOTBad<<endmsg;
    }

    /*
     *    now fill the input vector to evaluate the NN
     */
    m_inputVec->reserve(22);
    m_inputVec->push_back( aTrack.chi2() );
    m_inputVec->push_back( aTrack.nDoF() );
    m_inputVec->push_back( aTrack.info(21,-1.) );
    m_inputVec->push_back( aTrack.info(17,-1.) );
    m_inputVec->push_back( aTrack.info(18,-1.) );
    m_inputVec->push_back( aTrack.info(19,-1.) );
    m_inputVec->push_back( aTrack.info(20,-1.) );
    m_inputVec->push_back( observedV );
    m_inputVec->push_back( observedIT );
    m_inputVec->push_back( observedTT );
    m_inputVec->push_back( observedOT );
    m_inputVec->push_back( (observedV-expectedV));
    m_inputVec->push_back( (observedIT-expectedIT));
    m_inputVec->push_back( (observedTT-expectedTT));
    m_inputVec->push_back( (observedOT-expectedOT));
    m_inputVec->push_back( m_otdecoder->totalNumberOfHits() );
    m_inputVec->push_back( nITCont );
    m_inputVec->push_back( nTTCont );
    m_inputVec->push_back( nVeloCont );
    m_inputVec->push_back( nOTBad );
    m_inputVec->push_back( aTrack.pseudoRapidity() );
    m_inputVec->push_back( aTrack.pt() );

    if ( UNLIKELY( isDebug ) ) {
      debug()<<"***   print all ***"<<endmsg;
      debug()<<"aTrack.chi2()           :"<< aTrack.chi2()<<endmsg;
      debug()<<"aTrack.nDoF()           :"<< aTrack.nDoF()<<endmsg;
      debug()<<"aTrack.info(21,-1.)     :"<< aTrack.info(21,-1.)<<endmsg;
      debug()<<"aTrack.info(17,-1.)     :"<< aTrack.info(17,-1.)<<endmsg;
      debug()<<"aTrack.info(18,-1.)     :"<<aTrack.info(18,-1.) <<endmsg;
      debug()<<"aTrack.info(19,-1.)     :"<<aTrack.info(19,-1.) <<endmsg;
      debug()<<"aTrack.info(20,-1.)     :"<< aTrack.info(20,-1.)<<endmsg;
      debug()<<"observedV               :"<< observedV<<endmsg;
      debug()<<"observedIT              :"<< observedIT<<endmsg;
      debug()<<"observedTT              :"<< observedTT<<endmsg;
      debug()<<"observedOT              :"<< observedOT<<endmsg;
      debug()<<"(observedV-expectedV)   :"<<(observedV-expectedV) <<endmsg;
      debug()<<"(observedIT-expectedIT) :"<< (observedIT-expectedIT)<<endmsg;
      debug()<<"(observedTT-expectedTT) :"<< (observedTT-expectedTT)<<endmsg;
      debug()<<"(observedOT-expectedOT) :"<< (observedOT-expectedOT)<<endmsg;
      debug()<<"OThits                  :"<<m_otdecoder->totalNumberOfHits() <<endmsg;
      debug()<<"IThits                  :"<<nITCont <<endmsg;
      debug()<<"TThits                  :"<<nTTCont <<endmsg;
      debug()<<"VELOhits                :"<<nVeloCont <<endmsg;
      debug()<<"nOTBad                  :"<<nOTBad <<endmsg;
      debug()<<"aTrack.pseudoRapidity() :"<<aTrack.pseudoRapidity() <<endmsg;
      debug()<<"aTrack.pt()             :"<<aTrack.pt() <<endmsg;
      debug()<<"***"<<endmsg;
    }

    retval = m_readerLong12->GetMvaValue( *m_inputVec );

    if ( UNLIKELY( isDebug ) )
    {
      debug()<<" NN output:  "<<retval<<endmsg;
      debug()<<"now tranform"<<endmsg;
    }

    // flatten
    retval = m_FlattenLookupTable->value(retval);

    if ( UNLIKELY( isDebug ) ) debug()<<"transformed value: "<<retval<<endmsg;

  }//end evaluate long track for MC2012 tuning

  aTrack.setGhostProbability(retval);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode TrackNNGhostId::finalize()
{
  delete m_readerVelo;
  delete m_readerUpstream;
  delete m_readerDownstream;
  delete m_readerTtrack;
  delete m_readerLong;
  delete m_readerLong12;
  delete m_inputVec;
  delete m_FlattenLookupTable;
  return GaudiTool::finalize();  // must be called after all other actions
}
