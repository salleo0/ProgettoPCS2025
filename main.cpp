#include <sstream>
#include "PolyhedronMesh.hpp"
#include "utils.hpp"

using namespace std;

int main(int argc, char *argv[])
{
	int p;
	int q;
	int b;
	int c;
	int id_vertice_1;
	int id_vertice_2;
	
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
	
	
	for( int i = 0; i < GeodeticSolid.NumCell0Ds;i++){
		cout << GeodeticSolid.Cell0DsId[i] << endl;
		cout << GeodeticSolid.Cell0DsCoordinates(0,i) << " " << GeodeticSolid.Cell0DsCoordinates(1,i) << " ";
		cout << GeodeticSolid.Cell0DsCoordinates(2,i) << endl;
	}
	
	cout << "-----------------" << endl;
	
	for( int i = 0; i < GeodeticSolid.NumCell1Ds; i++){
		cout << GeodeticSolid.Cell1DsId[i] << endl;
		cout << GeodeticSolid.Cell1DsExtrema(0,i) << " " << GeodeticSolid.Cell1DsExtrema(1,i) << endl;
	} 
	cout << "-----------------" << endl;
	
	for( int i = 0; i < GeodeticSolid.NumCell2Ds; i++){
		cout << GeodeticSolid.Cell2DsId[i] << endl;
		for( int j = 0; j < 3; j++){
			cout << GeodeticSolid.Cell2DsVertices[i][j] << " ";
		}
		cout << endl;
		
		for( int j = 0; j < 3; j++){
			cout << GeodeticSolid.Cell2DsEdges[i][j] << " ";
		}
		cout << endl;
	}
	
	return 0;
}