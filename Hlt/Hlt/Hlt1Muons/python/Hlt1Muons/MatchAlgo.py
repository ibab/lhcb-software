## Python
from math import sqrt
from collections import defaultdict
from operator import itemgetter

## GaudiPython
from LoKiAlgo.decorators import Algo
from GaudiPython.Bindings import gbl as cpp
from GaudiPython.Bindings import SUCCESS
from LinkerInstances.eventassoc import linkedFrom, linkedTo

class MatchAlgoBase(Algo):
    def initialize(self):
        sc = Algo.initialize( self )
        if sc.isFailure():
            return sc

        self.vector = cpp.std.vector
        self.Double = cpp.Double
        self.muon_det = self.getDet("/dd/Structure/LHCb/DownstreamRegion/Muon")
        self.zs = [self.muon_det.getStationZ(i) for i in range(5)]

        self.isMuon = self.tool(cpp.ITracksFromTrack, 'IsMuonTool')
        self.Info("Retrieving " + self.MatchVeloMuon)
        self.matchMuon = self.tool(cpp.IMatchVeloMuon, self.MatchVeloMuon + ':PUBLIC')
        self.fwUpgrade = self.tool(cpp.ITracksFromTrack, 'PatForwardTool/LooseForward')
        self.stateProvider = self.tool(cpp.ITrackStateProvider, 'TrackStateProvider:PUBLIC')
        self.extrapolator = self.tool(cpp.ITrackExtrapolator, 'TrackMasterExtrapolator:PUBLIC')
        return SUCCESS

    def overlap(self, track, velo_track):
        track_key = track.key()
        if track_key not in self.track_ids:
            self.track_ids[track_key] = set( [ i.lhcbID() for i in track.lhcbIDs() if i.isVelo() ] )
        tIDs = self.track_ids[track_key]
        velo_key = velo_track.key()
        if velo_key not in self.velo_ids:
            self.velo_ids[velo_key] = set( [ i.lhcbID() for i in velo_track.lhcbIDs() ] )
        vIDs = self.velo_ids[velo_key]
        if not tIDs or not vIDs:
            return 0.
        return float( len( tIDs & vIDs ) ) / float( len( tIDs ) )

    def findVelo(self, veloTracks, muon):
        overlaps = []
        for veloTrack in veloTracks:
            if veloTrack.checkFlag(cpp.LHCb.Track.Backward):
                continue
            ov = self.overlap( muon.track() , veloTrack )
            if ov > 0.69:
                overlaps.append( ( veloTrack, ov ) )
        if len( overlaps ) == 0:
            return (None, None)

        overlaps = sorted( overlaps, key = itemgetter( 1 ) )
        return overlaps[ -1 ]

    def findMuon(self, velo_track, muons ):
        overlaps = []
        for muon in muons:
            ov = self.overlap( muon.track() , velo_track )
            if ov > 0.69:
                overlaps.append( ( muon, ov ) )
        if len( overlaps ) == 0:
            return None, 0.

        overlaps = sorted( overlaps, key = itemgetter( 1 ) )
        return overlaps[ -1 ]

    def to_vector(self, l):
        v = self.vector('double')()
        for e in l:
            v.push_back(e)
        return v

    def muon_overlap(self, ids, muon_ids):
        return float(len(ids & muon_ids)) / len(ids) if (muon_ids and ids) else 0.

    def true_positions(self, mcState, hits):
        zs = {e[0] : e[1][5] for e in hits.iteritems()}
        xs = {k : (mcState.x() + (z - mcState.z()) * mcState.tx()) for k, z in zs.iteritems()}
        ys = {k : (mcState.y() + (z - mcState.z()) * mcState.ty()) for k, z in zs.iteritems()}
        return xs, ys

    def muon_coords(self, muon, mcp, mcState):
        lf = linkedFrom(cpp.LHCb.MuonCoord, cpp.LHCb.MCParticle, "Raw/Muon/Coords")
        linkedCoords = set([c.key().key() for c in lf.range(mcp)])
        muids = defaultdict(list)
        for muid in muon.muonPID().muonTrack().lhcbIDs():
            # Filter true muon hits
            if not muid.muonID().key() in linkedCoords:
                continue
            muids[muid.muonID().station()].append(muid)
        # Ignore M1
        muids.pop(0, None)

        Double = self.Double
        mx = Double()
        mdx = Double()
        my = Double()
        mdy = Double()
        mz = Double()
        mdz = Double()
        if not muids:
            return {}

        min_ids = {}
        for station, ids in muids.iteritems():
            pull = None
            for mid in ids:
                r = self.muon_det.Tile2XYZ(mid.muonID(), mx, mdx, my, mdy, mz, mdz)
                x_ext = mcState.x() + (mz - mcState.z()) * mcState.tx()
                y_ext = mcState.y() + (mz - mcState.z()) * mcState.ty()
                p = sqrt((mx - x_ext) ** 2 + (my - y_ext) ** 2) / (2 * sqrt(mdx ** 2 + mdy ** 2))
                if pull == None or p < pull:
                    pull = p
                    min_ids[station] = (mid.muonID(), float(mx), float(my), float(mdx), float(mdy), float(mz))
        return min_ids

    def momentum(self, dtx):
        return 1255 / abs(dtx) + 175;

    def matched_muons(self, rec_muons):
        lt = linkedTo(cpp.LHCb.MCParticle, cpp.LHCb.Track, "Rec/Track/Best")
        matched_muons = {}
        for muon in rec_muons:
            mcps = []
            mcp = lt.first(muon.track())
            while mcp:
                if abs(mcp.particleID().pid()) == 13:
                    mcps.append((mcp, lt.weight()))
                mcp = lt.next()
            if mcps:
                matched_muons[muon] = sorted(mcps, key = itemgetter(1))[-1][0]
        return matched_muons

class MatchResidualAlgo(MatchAlgoBase):
    def fit_candidate(self, hits, veloState, slope):
        # Create candidate from veloState and add the magnet hit
        candidate = cpp.Candidate(veloState)
        for hit in hits:
            candidate.addHit(hit)

        # Fit the candidate to get the best slope estimate, it needs the initial
        # slope to estimate the charge.
        candidate.setSlope(slope)
        self.matchMuon.fitCandidate(candidate)
        return candidate

    def match_residuals(self, true_hits, veloState, xm, ym, zm):
        # Create magnet hit
        mid = cpp.LHCb.MuonTileID()

        # estimate momentum as in match tool
        #QUESTO FORSE E` SBAGLIATO!!!! zhit = true_hits[5]
        _, xm3, ym3, _, _, zhit = true_hits[2]
        slope = (xm3 - xm) / (zhit - zm)
        p_match = self.momentum(slope - veloState.tx())

        # The corrections to the magnet normally happen in the seed creation,
        # since that is skipped here, apply them explicitly
        cor = {"x" : [8.3, -33.0, 76.9],
               'y' : [6.15, 8.80]}
        correct = lambda p_inv, coeffs: sum([coeffs[i] * pow(p_inv, i) for i in range(len(coeffs))])
        dxm = 0.5 * correct(1000. / p_match, cor['x'])
        dym = 0.5 * correct(1000. / p_match, cor['y'])
        magnetHit = cpp.CommonMuonHit(mid, xm, dxm, ym, dym, zm, 0, False)

        search_res_x =[]
        search_res_y =[]
        
        # Hits to fit from, should be ordered the same as in the C++.
        hits = [magnetHit]
        order = {v : i for i, v in enumerate((2, 3, 4, 1))}
        for pos in [h[1] for h in sorted(true_hits.items(), key = lambda h: order[h[0]])]:
            x_search = xm + slope * (pos[5] - zm)
            y_search = veloState.y() + veloState.ty() * (pos[5] - veloState.z())

            search_res_x.append(pos[1] - x_search)
            search_res_y.append(pos[2] - y_search)

            ord_search_res_x = [search_res_x[-1]]+search_res_x[:-1]
            ord_search_res_y = [search_res_y[-1]]+search_res_y[:-1]

            #print pos[5], pos[1], x_search, pos[1] - x_search
            mh = cpp.CommonMuonHit(pos[0], pos[1], pos[3], pos[2], pos[4], pos[5], 0, False)
            hits.append(mh)
            
        candidate = self.fit_candidate(hits, veloState, slope)

        # Calculate the residuals with respect to the extrapolation obtained
        # from the candidate.
        y = self.Double()
        err = self.Double()

        res_x = {}
        res_y = {}
        for hit in hits[1:]:
            res_x[hit.station()] = hit.x() - (xm + (hit.z() - zm) * candidate.slope())
            candidate.yStraight(hit.z(), y, err)
            res_y[hit.station()] = hit.y() - float(y)

        return res_x, res_y, ord_search_res_x, ord_search_res_y, candidate.chi2DoF()

    def analyse(self):
        Double = cpp.Double
        LHCb = cpp.LHCb
        Gaudi = cpp.Gaudi
        Track = LHCb.Track
        TrackVector = cpp.std.vector( 'LHCb::Track*' )

        ntuple  = self.nTuple ('match_zerr')

        self.velo_ids = {}
        self.track_ids = {}
        odin = self.get('DAQ/ODIN')
        veloTracks = self.get('Hlt/Track/Velo')
        digits = self.get(LHCb.MuonDigitLocation.MuonDigit)
        mcDigits = self.get('MC/Muon/Digits')

        rec_muons = [pp for pp in self.get('Rec/ProtoP/Charged') if pp.muonPID() and pp.muonPID().IsMuon()]
        m2mcp = self.matched_muons(rec_muons)

        for muon, mcp in m2mcp.iteritems():
            muon_track = muon.muonPID().muonTrack()
            track = muon.track()
            if (not track.checkType(LHCb.Track.Long)) or track.checkFlag(LHCb.Track.Invalid):
                continue

            # Get the state in M3 from the MC particle
            mcVertex = mcp.originVertex()
            mcPos = mcVertex.position()
            mc4Mom = mcp.momentum()

            charge = int((mcp.particleID().threeCharge()) / 3.)

            # determine Q/P
            p = mc4Mom.P()
            trueQOverP = charge / p

            # construct true State
            pVec = LHCb.StateVector(mcPos, Gaudi.XYZVector(mc4Mom.Vect()), trueQOverP)
            mcState = LHCb.State()

            mcState.setState(pVec.parameters())
            mcState.setZ(pVec.z ())

            # set covariance matrix
            cov = Gaudi.TrackSymMatrix()
            cov[0][0] = 1
            cov[1][1] = 1
            cov[2][2] = 1
            cov[3][3] = 1
            cov[4][4] = 1
            mcState.setCovariance(cov)

            # States for later use
            mcVeloState = LHCb.State(mcState)
            yState = LHCb.State(mcState)

            # Propagate the MC state to M3
            zm3 = self.zs[2]
            sc = self.extrapolator.propagate(mcState, zm3, mcp.particleID())
            if (sc.isFailure()):
                continue

            # Track state in Velo
            veloState = track.stateAt(LHCb.State.EndVelo);
            if not veloState:
                veloState = LHCb.State()
                self.stateProvider.state(veloState, track, 770)

            # Propagate the MC Velo state to EndVelo
            sc = self.extrapolator.propagate(mcVeloState, 770, mcp.particleID())
            if (sc.isFailure()):
                continue

            # Calculated difference between true and extrapolated z using slopes in
            # X and our kick estimate
            cvx = mcVeloState.x() -  mcVeloState.tx() * mcVeloState.z()
            cmx = mcState.x() -  mcState.tx() * mcState.z()
            zTrue = (cmx - cvx) / (mcVeloState.tx() - mcState.tx())
            xTrue = mcVeloState.x() + (zTrue - mcVeloState.z()) * mcVeloState.tx()

            # Propagate the yState to the true z of the magnet hit
            sc = self.extrapolator.propagate(yState, zTrue, mcp.particleID())
            if sc.isFailure():
                continue

            # Initial slope and momentum estimate
            z0 =  5412
            z1 = -3567
            c0 =    25.17
            c1 =  -701.5
            zMagnet = z0 + z1 * veloState.ty() ** 2;
            xMagnet = veloState.x() + (zMagnet - veloState.z()) * veloState.tx()
            yMagnet = veloState.y() + (zMagnet - veloState.z()) * veloState.ty()

            hits = self.muon_coords(muon, mcp, mcState)
            # Check for hit in M3
            if 2 not in hits or len(hits) < 3:
                continue

            # Real seed hit in M3
            mid, xm3, ym3, dxm3, dym3, zSeed = hits[2]

            # estimate momentum as in match tool
            slope = (xm3 - xMagnet ) / ( zSeed - zMagnet );
            p_match = self.momentum( slope - veloState.tx() );

            # First set of columns
            ntuple.column('run_number', odin.runNumber())
            ntuple.column('event_number', odin.eventNumber())
            ntuple.column("p", track.p())
            ntuple.column("pTrue", p)
            ntuple.column("q", int(track.charge()))
            ntuple.column("pt", track.pt())
            ntuple.column("p_match", p_match)
            ntuple.column("tx", veloState.tx())
            ntuple.column("ty", veloState.ty())
            ntuple.column("zMagnet", zMagnet)
            ntuple.column("xMagnet", xMagnet)
            ntuple.column("yMagnet", yMagnet)

            # Momentum-dependent correction on zMagnet as a function of p in GeV;
            # use it to also update x and y of the magnet
            zMagnetUp = zMagnet + c0 + c1 * (1000. / p_match) ** 2
            xMagnetUp = veloState.x() + (zMagnetUp - veloState.z()) * veloState.tx()
            yMagnetUp = veloState.y() + (zMagnetUp - veloState.z()) * veloState.ty()

            ntuple.column("zMagnetUp", zMagnetUp)
            ntuple.column("xMagnetUp", xMagnetUp)
            ntuple.column("yMagnetUp", yMagnetUp)

            # state = LHCb.State(track.closestState(zm3))
            # pid = -13 if track.charge() < 0 else 13
            # pid = LHCb.ParticleID(pid)
            # self.extrapolator.propagate(state, zm3, pid)

            ntuple.column("zTrue", zTrue)
            ntuple.column("xTrue", xTrue)
            ntuple.column("yTrue", yState.y())
            ntuple.column("dx", xMagnet - xTrue)
            ntuple.column("dy", yMagnet - yState.y())

            # Also store difference in dy slope to see if we can/should use it
            ntuple.column("dty", mcState.ty() - veloState.ty())

            xs, ys = self.true_positions(mcState, hits)

            res_x = []
            res_dx = []
            res_y = []
            res_dy = []
            res_s = []
            hit_z = []
            for s, (mid, mux, muy, mudx, mudy, muz) in sorted(hits.items(), key = itemgetter(0)):
                hit_z.append(muz)
                res_s.append(s)
                res_x.append(mux - xs[s])
                res_dx.append(mudx)
                res_y.append(muy - ys[s])
                res_dy.append(mudy)

            # match_res_x, match_res_y, chi2 = self.match_residuals(hits, veloState, xMagnet, yMagnet, zMagnet)
            match_res_x, match_res_y, search_res_x, search_res_y, chi2 = self.match_residuals(hits, veloState, xMagnet, yMagnet, zMagnet)
            assert(len(match_res_x) == len(res_x))
            assert(len(match_res_y) == len(res_y))

            mry = []

            ntuple.farray("hit_z", self.to_vector(hit_z), "n_hits", 5)
            ntuple.farray("res_s", self.to_vector(res_s), "n_hits", 5)
            ntuple.farray("res_x", self.to_vector(res_x), "n_hits", 5)
            ntuple.farray("res_y", self.to_vector(res_y), "n_hits", 5)
            ntuple.farray("res_dx", self.to_vector(res_dx), "n_hits", 5)
            ntuple.farray("res_dy", self.to_vector(res_dy), "n_hits", 5)
            ntuple.farray("search_res_x", self.to_vector(search_res_x), "n_hits", 5)
            ntuple.farray("search_res_y", self.to_vector(search_res_y), "n_hits", 5)
            #ntuple.farray("search_res_x", self.to_vector([search_res_x[s] for s in sorted(search_res_x.keys())]), "n_hits", 5)
            #ntuple.farray("search_res_y", self.to_vector([search_res_y[s] for s in sorted(search_res_y.keys())]), "n_hits", 5)

            ntuple.farray("match_res_x", self.to_vector([match_res_x[s] for s in sorted(match_res_x.keys())]), "n_hits", 5)
            ntuple.farray("match_res_y", self.to_vector([match_res_y[s] for s in sorted(match_res_y.keys())]), "n_hits", 5)


            ntuple.column('match_chi2dof', chi2)
            ntuple.write()

        return SUCCESS

    def finalize(self):
        """
        Standard finalization
        """
        return Algo.finalize(self)

class MatchChi2Algo(MatchAlgoBase):
    # def initialize(self):
    #     sc = super(MatchChi2Algo, self).initialize(self)
    #     if sc.isFailure():
    #         return sc
    def chi2s(self, velo_seed):
        LHCb = cpp.LHCb

        self.matchMuon.clean()
        ## Seed M3
        seed_station = 2
        self.matchMuon.findSeeds(velo_seed, seed_station)
        seeds = self.matchMuon.seeds()
        good_candidates = []
        chi2s = {}
        for seed in seeds:
            self.matchMuon.addHits(seed)
            # The magnet hit is counted. Without it, there are 4 stations that
            # are taken into account.
            if seed.hits().size() <= 4:
                continue

            self.matchMuon.fitCandidate(seed)
            ids = frozenset(LHCb.LHCbID(h.tile()).lhcbID() for h in seed.hits()[1:])
            chi2s[ids] = seed

        return chi2s

    def analyse(self):
        Double = cpp.Double
        LHCb = cpp.LHCb
        Gaudi = cpp.Gaudi
        Track = LHCb.Track
        TrackVector = cpp.std.vector( 'LHCb::Track*' )

        ntuple  = self.nTuple ('match_chi2')

        self.velo_ids = {}
        self.track_ids = {}
        odin = self.get('DAQ/ODIN')
        veloTracks = self.get('Hlt/Track/Velo')
        digits = self.get(LHCb.MuonDigitLocation.MuonDigit)
        mcDigits = self.get('MC/Muon/Digits')

        rec_muons = [pp for pp in self.get('Rec/ProtoP/Charged') if pp.muonPID() and pp.muonPID().IsMuon()]
        m2mcp = self.matched_muons(rec_muons)

        for muon in rec_muons:
            muon_track = muon.muonPID().muonTrack()
            track = muon.track()
            if (not track.checkType(LHCb.Track.Long)) or track.checkFlag(LHCb.Track.Invalid):
                continue

            # Track state in Velo
            veloState = track.stateAt(LHCb.State.EndVelo);
            if not veloState:
                veloState = LHCb.State()
                self.stateProvider.state(veloState, track, 770)

            # Get the state in M3 from the MC particle
            true_hits = None
            mcp = m2mcp.get(muon, None)
            if mcp:
                mcVertex = mcp.originVertex()
                mcPos = mcVertex.position()
                mc4Mom = mcp.momentum()

                charge = int((mcp.particleID().threeCharge()) / 3.)

                # determine Q/P
                p = mc4Mom.P()
                trueQOverP = charge / p

                # construct true State
                pVec = LHCb.StateVector(mcPos, Gaudi.XYZVector(mc4Mom.Vect()), trueQOverP)
                mcState = LHCb.State()

                mcState.setState(pVec.parameters())
                mcState.setZ(pVec.z ())

                # set covariance matrix
                cov = Gaudi.TrackSymMatrix()
                cov[0][0] = 1
                cov[1][1] = 1
                cov[2][2] = 1
                cov[3][3] = 1
                cov[4][4] = 1
                mcState.setCovariance(cov)

                # Propagate the MC state to M3
                zm3 = self.zs[2]
                sc = self.extrapolator.propagate(mcState, zm3, mcp.particleID())
                if (sc.isFailure()):
                    continue

                true_hits = self.muon_coords(muon, mcp, mcState)


            # Initial slope and momentum estimate
            z0 =  5412
            z1 = -3567
            c0 =    25.17
            c1 =  -701.5
            zMagnet = z0 + z1 * veloState.ty() ** 2;
            xMagnet = veloState.x() + (zMagnet - veloState.z()) * veloState.tx()
            yMagnet = veloState.y() + (zMagnet - veloState.z()) * veloState.ty()

            veloTrack, velo_overlap = self.findVelo(veloTracks, muon)
            if not veloTrack:
                continue
            chi2s = self.chi2s(veloTrack)

            for seed_ids, seed in chi2s.iteritems():
                # The first hit is the one in the magnet, the second the seed hit
                seed_hit = seed.hits()[1]

                # estimate momentum as in match tool
                slope = (seed_hit.x() - xMagnet ) / ( seed_hit.z() - zMagnet );
                p_match = self.momentum( slope - veloState.tx() );

                overlap = 0.
                if true_hits:
                    true_ids = set([LHCb.LHCbID(v[0]).lhcbID() for v in true_hits.itervalues()])
                    overlap = float(len(seed_ids.intersection(true_ids))) / float(len(true_ids))

                # First set of columns
                ntuple.column('true_muon', bool(mcp))
                ntuple.column('muon_overlap', overlap)
                ntuple.column('run_number', odin.runNumber())
                ntuple.column('event_number', odin.eventNumber())
                ntuple.column("p", track.p())
                ntuple.column("q", int(track.charge()))
                ntuple.column("pt", track.pt())
                ntuple.column("p_match", seed.p())
                ntuple.column("tx", veloState.tx())
                ntuple.column("ty", veloState.ty())
                ntuple.column("zMagnet", zMagnet)
                ntuple.column("xMagnet", xMagnet)
                ntuple.column("yMagnet", yMagnet)
                ntuple.column('velo_overlap', velo_overlap)
                ntuple.column('chi2', seed.chi2())
                ntuple.column('nDoF', seed.nDoF())
                ntuple.column('chi2DoF', seed.chi2DoF())
                ntuple.write()

        return SUCCESS

    def finalize(self):
        """
        Standard finalization
        """
        return Algo.finalize(self)
