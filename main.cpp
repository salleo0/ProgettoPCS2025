#include <sstream>
#include "PolyhedronMesh.hpp"
#include "utils.hpp"

using namespace std;

int main(int argc, char *argv[])
{
	unsigned int p;
	unsigned int q;
	unsigned int b;
	unsigned int c;
	unsigned int id_vertice_1;
	unsigned int id_vertice_2;
	
	stringstream convert;
	
	for (int i = 1; i < argc; i++)
		convert << argv[i] << " ";
	
	convert >> p >> q >> b >> c;
	if (argc == 7)
		convert >> id_vertice_1 >> id_vertice_2;
	
	if (p != 3){
		cerr << "The construction of a geodetic solid requires a value of p equal to 3" << endl;
		return 1;
	}
	
	// chiamata alla costruzione del solido platonico di partenza
	string InputFile = "../SolidiPlatonici";
	PolyhedronMesh PlatonicPolyhedron;
	switch(q) {
		case 3:
			// tetraedro
			cout << "The platonic polyhedron corresponding to the Schlafli symbol {3, " << q << "} is a tetrahedron" << endl;
			InputFile += "/Tetraedro/";
			break;
		case 4:
			// ottaedro
			cout << "The platonic polyhedron corresponding to the Schlafli symbol {3, " << q << "} is a octahedron" << endl;
			InputFile += "/Ottaedro/";
			break;
		case 5:
			// icosaedro
			cout << "The platonic polyhedron corresponding to the Schlafli symbol {3, " << q << "} is a icosahedron" << endl;
			InputFile += "/Icosaedro/";
			break;
		default:
			cerr << "A platonic polygon with Schlafli symbol {3, " << q << "} does not exist" << endl;
			return 1;
			break;
	}
	
	if(!ImportPolyhedronMesh(PlatonicPolyhedron, InputFile)){
		cerr << "Something went wrong during the creation of the platonic polyhedron mesh" << endl;
		return 1;
	}
	
	PolyhedronMesh GeodeticSolid;
	
	GenerateGeodeticSolidType1(PlatonicPolyhedron, GeodeticSolid, b);
	
	return 0;
}