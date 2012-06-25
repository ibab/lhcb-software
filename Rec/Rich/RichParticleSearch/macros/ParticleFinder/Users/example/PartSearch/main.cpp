#include <iostream>
#include "ChainTuples.h"
#include "RichParticleFinder.h"
#include <utility>




using namespace std;


int main(int argc, char** argv)
{
	cout << argv[1] << endl;
	cout << argc << endl;
	if (argc != 3)
	{
		cout << "Incorrect number of arguements";
		cout << "List of files";
		cout << "Followed by list of variables"<<endl;
		return -1;
	}
	string files = argv[1];
	string vars = argv[2];

//	const char* cuts =  "(MinimumTrackSeperation > 150) && (PhotonsPerTrack < 30)";
	const char* cuts =  "(PhotonsPerTrack < 30) && (MinimumTrackSeperation > 150 )";

	cout << files << endl;
	ChainTuples* ChainT = new ChainTuples(files, "RICH/RichParticleSearchR1Gas/TrackTuple");
	MPC::RichParticleFinder PartFind(cuts,ChainT);
	PartFind.SetVariables(argv[2]);
	PartFind.SetHistoLevel(2);
	PartFind.SetBranchAddress();
	PartFind.CreateNtuple("Rich1ParticleTuple.root");
	PartFind.WriteHistograms("Rich1ParticleHistos.root");

	return 0;
	
}
