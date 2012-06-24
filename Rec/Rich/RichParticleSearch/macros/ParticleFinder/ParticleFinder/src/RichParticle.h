#ifndef RICHPARTICLE_H
#define RICHPARTICLE_H

#include <iostream>
#include <map>
#include <string>

namespace MPC{
class RichParticle
{
public:
	RichParticle(std::map<std::string, double>& inputVars)
	{
		vars = inputVars;
	}
	void FillNTuple();
	double& operator[] (std::string name);
	const RichParticle& operator= (const MPC::RichParticle& copy);
	
private:
	
	std::map<std::string, double> vars; //variable name and value

};
}

#endif
