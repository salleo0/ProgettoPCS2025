#include <sstream>
#include "PolyhedronMesh.hpp"
#include "utils.hpp"
#include "UCDUtilities.hpp"

using namespace std;
using namespace TriangulationLibrary;

int main(int argc, char *argv[])
{
	// salvataggio dei dati in input
	// - {p, q} individuano il solido platonico di partenza
	// - (b, c) indicano come triangolare il solido platonico e di consequenza il 
	//			poliedro geodetico corrispondente
	// id_vertex_1 -> id_vertex_2: cercare il cammino minimo tra questi vertici
	//								del poliedro geodetico 
	int p;
	int q;
	int b;
	int c;
	int id_vertex_1;
	int id_vertex_2;
	
	stringstream convert;
	
	for (int i = 1; i < argc; i++)
		convert << argv[i] << " ";
	
	convert >> p >> q >> b >> c;
	
	// COSTRUZIONE DELLA MESH DEL POLIEDRO PLATONICO
	// richiediamo che il poliedro platonico abbia facce di 3 vertici (<=> p = 3)
	if (p != 3){
		cerr << "The construction of a geodetic solid requires a value of p equal to 3" << endl;
		return 1;
	}
	
	// chiamata alla costruzione del solido platonico di partenza
	// {p, q} individuano un poliedro platonico tra: Tetraedro, Ottaedro e Icosaedro
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
	
	if(!Import::ImportPolyhedronMesh(PlatonicPolyhedron, InputFile)){
		cerr << "Something went wrong during the creation of the platonic polyhedron mesh" << endl;
		return 1;
	}
	
	// COSTRUZIONE DELLA MESH DEL POLIEDRO GEODETICO E DEL SUO DUALE
	PolyhedronMesh GeodeticPolyhedron;
	PolyhedronMesh DualPolyhedron;
	if ( b > 0 && c == 0)
		Generation::GeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, b);
	else if ( b == 0 && c > 0)
		Generation::GeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, c);
	else if (b == c)
		Generation::GeodeticSolidType2(PlatonicPolyhedron, GeodeticPolyhedron, b);
	else {
		cerr << "The case b not equal to c with both b and c greater than 0 is not supported " << endl;
		return 1;
	}
	
	// GENERAZIONE DUALE SE q = 3, ALTRIMENTI SOLO ESPORTAZIONE GEODETICO
	Gedim::UCDUtilities utilities;	
	if ( q == 3 ){
		cout << "Generation of a generalized Goldberg polyhedron with Schlafli symbol {3+, 3}_(" << b << ", " << c << ")" << endl;
		Generation::Dual(GeodeticPolyhedron, DualPolyhedron);
		utilities.ExportPoints("./Cell0Ds.inp",
								DualPolyhedron.Cell0DsCoordinates);		
		utilities.ExportSegments("./Cell1Ds.inp",
								DualPolyhedron.Cell0DsCoordinates,
								DualPolyhedron.Cell1DsExtrema);
		if (!ExportOutputFiles(DualPolyhedron)){
			cerr << "Error during the export of .txt files" << endl;
			return 1;
		}
	}
	else {
		cout << "Generation of a geodetic polyhedron with Schlafli symbol {3, " << q << "+}_(" << b << ", " << c << ")" << endl;
		utilities.ExportPoints("./Cell0Ds.inp",
								GeodeticPolyhedron.Cell0DsCoordinates);
		
		utilities.ExportSegments("./Cell1Ds.inp",
								GeodeticPolyhedron.Cell0DsCoordinates,
								GeodeticPolyhedron.Cell1DsExtrema);
		if (!ExportOutputFiles(GeodeticPolyhedron)){
			cerr << "Error during the export of .txt files" << endl;
			return 1;
			
		}	
	}

	if (argc == 7){
		convert >> id_vertex_1 >> id_vertex_2;
		if (ShortestPath(GeodeticPolyhedron, id_vertex_1, id_vertex_2)){
			cout<<"minimum path found correctly!"<<endl;
		}
	}
	
	return 0;
}
