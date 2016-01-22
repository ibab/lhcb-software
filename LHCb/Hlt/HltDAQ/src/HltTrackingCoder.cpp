// Include files
#include <functional>
#include <algorithm>
#include <iostream>

// local
#include "HltTrackingCoder.h"
#include "Event/State.h"
#include "Event/StandardPacker.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for Track en/decoding functions
// to be used by the HltTrackingWriter and HltTrackingDecoder
// Both functions are implemented here in the same file in order to
// keep them in synch easily.
//
// Unit tests for these functions are in utestTrackingCoder.cxx
//
// Track rawbank format:
//
//  For each track:
//  (nLhHCbIDs, ID0, ID1, ... IDn)
//
//  tracks are just concatenated
//  The RawBank is terminated with an entry nLHCbIDS=0
//
// 20014-01-14 : Sebastian Neubert
//-----------------------------------------------------------------------------

// It is stupid to have to instantiate this
const StandardPacker pac = { };

void encodeTracks( const LHCb::Tracks& tracks, std::vector<unsigned int>& rawBank, bool writeStates )
{
    auto out = std::back_inserter(rawBank);

    auto nWords = std::accumulate( std::begin(tracks), std::end(tracks), rawBank.size(),
                                   [writeStates](std::vector<unsigned int>::size_type s, const LHCb::Track* t) {
                                     return writeStates ? s+3+t->nLHCbIDs()+t->states().size()*22 : s+3+t->nLHCbIDs() ;
    });
    rawBank.reserve(nWords);

    // std::cout << "Encoding "<<tracks->size()<<" tracks."<<std::endl;
    for ( const LHCb::Track* Tr : tracks ) {
        // write meta information
        // flags
        *out++ =  Tr->flags();
        //
        unsigned int nhits = Tr->nLHCbIDs();
        *out++ =  nhits;

        // write LHCbIDs
        // behold the awesomness of C++11 functional programming
        // here use the C++ "map" functional transform to fill the LHCbIDs into the bank
        assert( nhits == Tr->lhcbIDs().size() );
        out = std::transform( std::begin( Tr->lhcbIDs() ), std::end( Tr->lhcbIDs() ),
                              out,
                              std::mem_fn( &LHCb::LHCbID::lhcbID ) );
        // write states
        // check number of states on track
        const std::vector<LHCb::State*>& states = Tr->states();

        if (!writeStates) {
          // Do not write states to save disk space
          *out++ =  0;
          continue;
        }
        // TODO: remove the states we're not interested in...
        *out++ =  states.size();

        // loop over states and encode locations, parameters and covs
        for ( const LHCb::State* state : states ) {
            // store the state location -- a bit of overkill to store this as 32 bit int...
            *out++ = state->location();
            *out++ = pac.position( state->z() );
            const Gaudi::TrackVector& par = state->stateVector();
            *out++ = pac.position( par[0] );
            *out++ = pac.position( par[1] );
            *out++ = pac.slope( par[2] );
            *out++ = pac.slope( par[3] );

            double p = state->qOverP();
            if (p!=0) p = 1./p;
            auto pp = pac.energy(p);
            *out++ =  pp;
            //== Get the coded value in case of saturation, to pack the momentum error
            //consistently later
            p = pac.energy( pp );

            // store covariance matrix
            // the method is analogous to the one descibed
            // in
            // http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/davinci/latest_doxygen/d2/dcf/class_l_h_cb_1_1_track_packer.html#ad854eb5fff1c364a97efe32990e41a5f
            // note that the off-diagonals are short ints
            // --> in principle we can put 2 covs in one uint

            // get errors for scaling
            std::array<double, 5> err { {
                std::sqrt( state->errX2() ),     std::sqrt( state->errY2() ),
                std::sqrt( state->errTx2() ),    std::sqrt( state->errTy2() ),
                std::sqrt( state->errQOverP2() ) } };
            // first store the diagonal then row wise the rest
            *out++ = pac.position( err[0] );
            *out++ = pac.position( err[1] );
            *out++ = pac.slope( err[2] );
            *out++ = pac.slope( err[3] );
            *out++ = pac.energy( 1.e5 * fabs( p ) * err[4] ); //== 1.e5 * dp/p (*1.e2)
            for ( unsigned i = 1; i < 5; ++i ) for ( unsigned j = 0; j < i; ++j ) {
                *out++ = pac.fraction( state->covariance()( i, j ) / err[i] / err[j] );
            } 
        }//  end loop over states
    }

    if (nWords!=rawBank.size()) {
        std::cout << "estimated size: " << nWords << " actual size: " << rawBank.size() << std::endl;
    }
}

// returns number of decoded tracks
unsigned int decodeTracks( const unsigned int* rawit, unsigned int nentries,
                           LHCb::Tracks& tracks )
{
    // due to the way the RawBank presents ist data we have
    // to loop over the data in the old fashioned way
    // so we use a pointer"iterator"
    // std::cout << "RawBank size = " << nentries << std::endl;
    unsigned int k = 0;
    while ( k < nentries ) {
        // Start a new track
        std::unique_ptr<Track> track{ new Track() };
        // read flags; this also contains the type
        track->setFlags( rawit[k++] );

        // read number of IDs in track
        auto nid = rawit[k++];
        for ( unsigned int i = 0; i < nid; i++ ) {
            track->addToLhcbIDs( LHCbID{ rawit[k++] } );
        }

        // read number of states
        auto nstates = rawit[k];
        if ( k + nstates * 22 > nentries ) {
            std::cout << "TOO MANY STATES IN TRACK. ABORTING DECODING. " << std::endl;
            std::cout << "This happens at track " << tracks.size() << " in the event"
                      << std::endl;
            std::cout << "Track so far:\n " << *track << std::endl;
            unsigned int nhits = track->nLHCbIDs();
            for ( unsigned int i = 0; i < nhits; ++i ) {
                std::cout << track->lhcbIDs()[i] << ",\n";
            }
            std::cout << "State of the the bank with " << nentries << " entries"
                      << std::endl;
            for ( unsigned int i = 0; i < 60; ++i ) {
                if ( i == 30 ) std::cout << "****" << std::endl;
                std::cout << rawit[i - 30 + k] << std::endl;
            }
            return tracks.size();
        }
        ++k;
        for ( unsigned int istate = 0; istate < nstates; ++istate ) {
            // add location
            LHCb::State::Location loc{ static_cast<LHCb::State::Location>(rawit[k++]) };
            // z coordinate
            double z = pac.position( int(rawit[k++]) );
            // add parameters
            Gaudi::TrackVector par;
            par[0] = pac.position( int(rawit[k++]) );
            par[1] = pac.position( int(rawit[k++]) );
            par[2] = pac.slope( int(rawit[k++]) );
            par[3] = pac.slope( int(rawit[k++]) );
            int p = (int)rawit[k++];
            par[4] = ( p != 0 ? 1.0 / pac.energy( p ) : 0.0 );
            Gaudi::TrackSymMatrix stateCov;

            //== Fill covariance matrix
            std::array<double, 5> err;
            err[0] = pac.position( (int)rawit[k++] );
            err[1] = pac.position( (int)rawit[k++] );
            err[2] = pac.slope( (int)rawit[k++] ); 
            err[3] = pac.slope( (int)rawit[k++] );
            err[4] = pac.energy( (int)rawit[k++] ) * fabs( par[4] ) * 1.e-5; // par[4]=1/p

            for ( unsigned i = 0; i < 5; ++i ) {
                for ( unsigned j = 0; j < i; ++j ) {
                    stateCov( i, j ) =
                        err[i] * err[j] * pac.fraction( (short int)rawit[k++] );
                }
                stateCov( i, i ) = err[i] * err[i];
            }
            track->addToStates( LHCb::State{ par, stateCov, z, loc } );
        } // end loop over states
        tracks.add( track.release() );
    }
    return tracks.size();
}
