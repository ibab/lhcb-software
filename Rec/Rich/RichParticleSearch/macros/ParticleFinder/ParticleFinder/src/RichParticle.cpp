#include "RichParticle.h"

using std::map;
using std::string;

namespace MPC{

void RichParticle::FillNTuple()
{
	std::map<string,double>::iterator it;
	for (it=vars.begin(); it != vars.end(); it++)
	{
		std::cout << (*it).first << " " << (*it).second << std::endl;
	}	
}


double& RichParticle::operator[] (string varName)
{
	return vars[varName];
}

const RichParticle& RichParticle::operator= (const MPC::RichParticle& copy)
{
	if (this == &copy) return copy;

	map<string,double> var_copy = copy.vars;
	vars.swap(var_copy);

	return *this;
}

}
