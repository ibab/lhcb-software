#include "TMath.h"
// Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "Event/RawEvent.h"

// Local
#include "PrPixelHitManager.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrPixelHitManager
//
// 2012-01-05 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY(PrPixelHitManager)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrPixelHitManager::PrPixelHitManager(const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent)
  : GaudiTool(type, name, parent)
  , m_useSlopeCorrection(true)
  , m_maxClusterSize(4)
{

  declareInterface<PrPixelHitManager>(this);

}

//=============================================================================
// Destructor
//=============================================================================
PrPixelHitManager::~PrPixelHitManager() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrPixelHitManager::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  m_isDebug = msgLevel(MSG::DEBUG);
  if (m_isDebug) debug() << "==> Initialize" << endmsg;
  // Get detector element.
  m_vp = getDet<DeVP>(DeVPLocation::Default);
  // Make sure we are up-to-date on populated VELO stations
  registerCondition((*(m_vp->leftSensorsBegin()))->geometry(), &PrPixelHitManager::rebuildGeometry);
  registerCondition((*(m_vp->rightSensorsBegin()))->geometry(), &PrPixelHitManager::rebuildGeometry);
  // First update
  sc = updMgrSvc()->update(this);
  if (!sc.isSuccess()) {
    return Error("Failed to update station structure.");
  }

  // Invalidate measurements at the beginning of each event
  incSvc()->addListener(this, IncidentType::BeginEvent);

  // zero out all buffers
  memset(m_buffer,0,SENSOR_PIXELS*sizeof(unsigned char));
  memset(m_sp_patterns,0,256*sizeof(unsigned char));
  memset(m_sp_sizes,0,256*sizeof(unsigned char));
  memset(m_sp_fx,0,512*sizeof(double));
  memset(m_sp_fy,0,512*sizeof(double));

  // create pattern buffer for isolated SPs
  cacheSPPatterns();

  // reserve a minimal stack
  m_stack.reserve(64);

  // Setup the hit pool.
  m_pool.resize(10000);
  m_maxSize = 0;
  m_nHits = 0;
  m_eventReady = false;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Finalize method.
//=========================================================================
StatusCode PrPixelHitManager::finalize() {

  info() << "Maximum number of Velo hits " << m_maxSize << endmsg;
  return GaudiTool::finalize();

}

//=========================================================================
// Cache Super Pixel cluster patterns.
//=========================================================================
void PrPixelHitManager::cacheSPPatterns() {
  
  // create a cache for all super pixel cluster patterns.
  // this is an unoptimized 8-way flood fill on the 8 pixels
  // in the super pixel. 
  // no point in optimizing as this is called once in
  // initialize() and only takes about 20 us. 

  // define deltas to 8-connectivity neighbours
  const int dx[] = { -1,  0,  1, -1,  0,  1, -1,  1};
  const int dy[] = { -1, -1, -1,  1,  1,  1,  0,  0};

  // clustering buffer for isolated superpixels.
  unsigned char sp_buffer[8];

  // SP index buffer and its size for single SP clustering
  unsigned char sp_idx[8];
  unsigned char sp_idx_size = 0;

  // stack and stack pointer for single SP clustering
  unsigned char sp_stack[8];
  unsigned char sp_stack_ptr = 0;

  // loop over all possible SP patterns
  for (unsigned int sp=0; sp < 256; ++sp) {
    sp_idx_size = 0;
    for(unsigned int shift=0; shift<8; ++shift) {
      const unsigned char p = sp & (1 << shift);
      sp_buffer[shift] = p;
      if (p) {
        sp_idx[sp_idx_size++] = shift;
      }
    }

    // loop over pixels in this SP and use them as
    // cluster seeds.
    // note that there are at most two clusters
    // in a single super pixel! 
    unsigned char clu_idx = 0;
    for (unsigned int ip=0; ip<sp_idx_size; ++ip) {
      unsigned char idx = sp_idx[ip];
      
      if (0 == sp_buffer[idx]) { continue; } // pixel is used
      
      sp_stack_ptr = 0;
      sp_stack[sp_stack_ptr++] = idx;
      sp_buffer[idx] = 0;
      unsigned char x = 0;
      unsigned char y = 0;
      unsigned char n = 0;

      while(sp_stack_ptr) {
        idx = sp_stack[--sp_stack_ptr];
        const unsigned char row = idx%4;
        const unsigned char col = idx/4;
        x += col;
        y += row;
        ++n;

        for (unsigned int ni=0; ni<8; ++ni) {
          const char ncol = col + dx[ni];
          if (ncol < 0 || ncol > 1) continue;
          const char nrow = row + dy[ni];
          if (nrow < 0 || nrow > 3) continue;
          const unsigned char nidx = (ncol << 2) | nrow;
          if (0 == sp_buffer[nidx]) continue;
          sp_stack[sp_stack_ptr++] = nidx;
          sp_buffer[nidx] = 0; 
        }
      }

      const uint32_t cx = x/n; 
      const uint32_t cy = y/n; 
      const double fx = x/static_cast<double>(n) - cx;
      const double fy = y/static_cast<double>(n) - cy;

      m_sp_sizes[sp] |= n << (4*clu_idx);

      // store the centroid pixel
      m_sp_patterns[sp] |= ((cx << 2) | cy) << 4*clu_idx;

      // set the two cluster flag if this is the second cluster
      m_sp_patterns[sp] |= clu_idx << 3;

      // set the pixel fractions
      m_sp_fx[2*sp + clu_idx] = fx;
      m_sp_fy[2*sp + clu_idx] = fy;

      // increment cluster count. note that this can only become 0 or 1!
      ++clu_idx;
    }
  }

  if (m_isDebug) {
    debug() << "==== SP Patterns Start ====" << endmsg;
    for (unsigned int i=0; i<256; ++i) {
      unsigned char pattern = m_sp_patterns[i];
      debug() << "-- pattern " << i << " --" << endmsg;
      for (int shift=3; shift>=0; --shift) {
        debug() << ((i>>shift) & 1) << (i>>(shift+4) & 1) << endmsg;
      }
      debug() << (pattern & 3) << " " << ((pattern>>2) & 1);
      if (pattern & 8) {
        debug() << "\t" << ((pattern>>4) & 3) << " " << ((pattern>>6) & 1) << endmsg; 
      } else {
        debug() << "\tn/a" << endmsg;
      }
    }
    debug() << "==== SP Patterns End   ====" << endmsg;
  } 
  return;
}



//=========================================================================
// Rebuild the geometry. Needed in case something changes in the Velo during the run...
//=========================================================================
StatusCode PrPixelHitManager::rebuildGeometry() {

  // Delete the existing modules.
  std::vector<PrPixelModule*>::iterator itm;
  m_modules.clear();
  m_module_pool.clear();
  m_firstModule = 999;
  m_lastModule  = 0;

  int previousLeft = -1;
  int previousRight = -1;
  std::vector<DeVPSensor*>::const_iterator its = m_vp->sensorsBegin();
  
  // get pointers to local x coordinates and pitches
  m_local_x = (*its)->xLocal();
  m_x_pitch = (*its)->xPitch();
  m_pixel_size = (*its)->pixelSize(LHCb::VPChannelID(0,0,0,0)).second;

  double ltg_rot_components[9];
  int idx = 0;
  for ( ; m_vp->sensorsEnd() != its; ++its) {
    //== TO BE DONE === 
    // if (!(*its)->isReadOut()) continue;

    // get the local to global transformation matrix and
    // store it in a flat float array of sixe 12.
    Gaudi::Rotation3D ltg_rot;
    Gaudi::TranslationXYZ ltg_trans;
    (*its)->geometry()->toGlobalMatrix().GetDecomposition(ltg_rot,ltg_trans);
    ltg_rot.GetComponents(ltg_rot_components);
    idx = 16*(*its)->sensorNumber();
    m_ltg[idx++] = ltg_rot_components[0];
    m_ltg[idx++] = ltg_rot_components[1];
    m_ltg[idx++] = ltg_rot_components[2];
    m_ltg[idx++] = ltg_rot_components[3];
    m_ltg[idx++] = ltg_rot_components[4];
    m_ltg[idx++] = ltg_rot_components[5];
    m_ltg[idx++] = ltg_rot_components[6];
    m_ltg[idx++] = ltg_rot_components[7];
    m_ltg[idx++] = ltg_rot_components[8];
    m_ltg[idx++] = ltg_trans.X(); 
    m_ltg[idx++] = ltg_trans.Y(); 
    m_ltg[idx++] = ltg_trans.Z(); 

    // Get the number of the module this sensor is on.
    const unsigned int number = (*its)->module();
    if (number < m_modules.size()) {
      // Check if this module has already been setup.
      if (m_modules[number]) continue;
    } else {
      m_modules.resize(number+1,0);
      //while (number > m_modules.size() + 1) {
      //  m_modules.push_back(0);
      //}
    }
    // Create a new module and add it to the list.
    m_module_pool.push_back(PrPixelModule(number, (*its)->isRight()));
    PrPixelModule* module = &m_module_pool.back();
    module->setZ((*its)->z());
    if ((*its)->isRight()) {
      module->setPrevious(previousRight);
      previousRight = number;
    } else {
      module->setPrevious(previousLeft);
      previousLeft = number;
    }
    m_modules[number]= module;
    if (m_firstModule > number) m_firstModule = number;
    if (m_lastModule  < number) m_lastModule  = number;
  }
  // the module pool might have been resized -- make sure
  // all module pointers are valid.
  for (unsigned int i=0; i<m_module_pool.size(); ++i) {
    PrPixelModule *module = &m_module_pool[i];
    m_modules[module->number()] = module;
  }
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Found modules from " << m_firstModule << " to " << m_lastModule << endmsg;
    for (itm = m_modules.begin(); m_modules.end() != itm; ++itm) {
      if (*itm) {
        debug() << "  Module " << (*itm)->number() << " prev " << (*itm)->previous() << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;

}

//=========================================================================
// Incident handler
//=========================================================================
void PrPixelHitManager::handle(const Incident& incident) {
  if (IncidentType::BeginEvent == incident.type()) {
    this->clearHits();
  }
}

//=========================================================================
// Clear all the hits from a previous event
//=========================================================================
void PrPixelHitManager::clearHits() {
  if (m_nHits > m_maxSize) m_maxSize = m_nHits;
  std::vector<PrPixelModule>::iterator itm;
  for (itm = m_module_pool.begin(); m_module_pool.end() != itm; ++itm) {
    (*itm).reset();
  }
  m_eventReady = false;
}

//=========================================================================
// Build PrPixelHits from Super Pixel Raw Bank
//=========================================================================
void PrPixelHitManager::buildHitsFromSPRawBank() 
{

  if (m_eventReady) return;
  m_eventReady = true;

  // Retrieve the RawEvent:
  LHCb::RawEvent *rawEvent = GaudiTool::getIfExists<LHCb::RawEvent>(LHCb::RawEventLocation::Default);
  if (NULL == rawEvent) { return; }
 
  const std::vector<LHCb::RawBank*>& tBanks = rawEvent->banks(LHCb::RawBank::VP);
  if(0 == tBanks.size()) { return; }
 
  // Assume binary resolution of hit position. This is the weight.
  const double w = 12.0/(0.055*0.055);

  // clear all cluster storage
  m_nHits = 0;

#include "PrPixelClustering.icpp"

  for (unsigned int i=0; i<m_nHits; ++i) {
    PrPixelHit *hit = &(m_pool[i]);
    m_modules[hit->module()]->addHit(hit);
  }

}

//=========================================================================
//  Convert the LiteClusters to PrPixelHits
//=========================================================================
void PrPixelHitManager::buildHits() {

  if (m_eventReady) return;
  m_eventReady = true;

  m_nHits = 0;

  // Get the clusters.
  LHCb::VPLiteCluster::VPLiteClusters* liteClusters =
    GaudiTool::get<LHCb::VPLiteCluster::VPLiteClusters>(LHCb::VPLiteClusterLocation::Default);
  // If necessary adjust the size of the hit pool.
  if (liteClusters->size() > m_pool.size()) {
    m_pool.resize(liteClusters->size() + 100);
  }
  // Assume binary resolution of hit position. This is the weight.
  const double w = 12.0/(0.055*0.055);
  // Loop over clusters.
  LHCb::VPLiteCluster::VPLiteClusters::const_iterator itc; 
  LHCb::VPLiteCluster::VPLiteClusters::const_iterator itc_end(liteClusters->end());
  for (itc = liteClusters->begin(); itc_end != itc; ++itc) {
    const unsigned int module = itc->channelID().module();
    if (module >= m_modules.size()) break;

    // store 3D point
    LHCb::VPChannelID cid = (*itc).channelID();
    const unsigned int sensor_chip = cid.chip()%SENSOR_CHIPS;
    const unsigned int sensor = cid.module()*MODULE_SENSORS + cid.chip()/SENSOR_CHIPS;
    const unsigned int cy = cid.row();
    const unsigned int cx = cid.col() + CHIP_COLUMNS*sensor_chip;
    const double dx = (*itc).interPixelFractionX()*m_x_pitch[cx];
    const double dy = (*itc).interPixelFractionY()*m_pixel_size;
    double local_x = m_local_x[cx] + dx;
    double local_y = (cy + 0.5)*m_pixel_size + dy;
    const double *ltg = m_ltg + sensor*16;
    double gx = ltg[0]*local_x + ltg[1]*local_y + ltg[ 9];
    double gy = ltg[3]*local_x + ltg[4]*local_y + ltg[10];
    double gz = ltg[6]*local_x + ltg[7]*local_y + ltg[11];

    if (m_useSlopeCorrection && (0.5 != dx || 0.5 != dy)) {
      double dx_prime = dx, dy_prime = dy;
      double delta_x = fabs(dx - 0.5);
      double delta_y = fabs(dy - 0.5);
      if (dx != 0.5) {
        double slx = fabs(gx / gz);
        double p_offset = 0.31172471 + 0.15879833 * TMath::Erf(-6.78928312 * slx + 0.73019077);
        double t_factor = 0.43531842 + 0.3776611 * TMath::Erf(6.84465914 * slx - 0.75598833);
        dx_prime = 0.5 + (dx - 0.5) / delta_x * (p_offset + t_factor * delta_x); 
      }
      if (dy != 0.5) {
        double sly = fabs(gx / gz);
        double p_offset = 0.35829374 - 0.20900493 * TMath::Erf(5.67571733 * sly -0.40270243);
        double t_factor = 0.29798696 + 0.47414641 * TMath::Erf(5.84419802 * sly -0.40472057);
        dy_prime = 0.5 + (dy - 0.5) / delta_y * (p_offset + t_factor * delta_y);
      }
      local_x = m_local_x[cx] + dx_prime;
      local_y = (cy + 0.5)*m_pixel_size + dy_prime;
      gx = ltg[0]*local_x + ltg[1]*local_y + ltg[ 9];
      gy = ltg[3]*local_x + ltg[4]*local_y + ltg[10];
      gz = ltg[6]*local_x + ltg[7]*local_y + ltg[11];
    }

    m_pool[m_nHits].setHit(LHCb::LHCbID(cid), gx, gy, gz, w, w, module);
    m_modules[module]->addHit(&(m_pool[m_nHits++]));
  }

}

//=========================================================================
// Sort hits by X within every module to speed up the track search
//=========================================================================
void PrPixelHitManager::sortByX() {
  std::vector<PrPixelModule*>::iterator itm;
  for (itm = m_modules.begin(); m_modules.end() != itm; ++itm) {
    if (*itm) {
      if (!((*itm)->empty())) {
        std::sort((*itm)->hits().begin(), (*itm)->hits().end(), 
                  PrPixelHit::LowerByX());
        (*itm)->setLastHitX((*itm)->hits().back()->x());
      }
    }
  }
}
