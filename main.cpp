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
	
	if (argc != 5 && argc != 7) {
		cerr << "Input arguments not valid. The number of input arguments must be equal to 4 (no short path) or to 6 (calculate the short path between two vertices)" << endl;
		return 1;
	}
	
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
	
	if(!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, InputFile)){
		cerr << "Something went wrong during the creation of the platonic polyhedron mesh" << endl;
		return 1;
	}
	
	// COSTRUZIONE DELLA MESH DEL POLIEDRO GEODETICO
	PolyhedronMesh GeodeticPolyhedron;
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
	
	// GENERAZIONE DUALE SE q = 3
	if ( q == 3){
		PolyhedronMesh DualPolyhedron;
		Generation::Dual(GeodeticPolyhedron, DualPolyhedron);
		GeodeticPolyhedron = DualPolyhedron;
		cout << "Generation of a generalized Goldberg polyhedron with Schlafli symbol {3+, 3}_(" << b << ", " << c << ")" << endl;
	}
	else 
		cout << "Generation of a geodetic polyhedron with Schlafli symbol {3, " << q << "+}_(" << b << ", " << c << ")" << endl;
	
	// ESPORTAZIONE FILE .inp E SE PRESENTI, COLLEGARE I VERTICI CON LO SHORTEST PATH
	bool flag = true;
	if (argc == 7){
		convert >> id_vertex_1 >> id_vertex_2;
		double path_length = 0.0;
		int num_edges_in_path = 0;
		if(Generation::ShortestPath(GeodeticPolyhedron, id_vertex_1, id_vertex_2, path_length, num_edges_in_path)){
			cout << "Shortest path between the vertices of id " << id_vertex_1 << " and " << id_vertex_2 << " found" << endl;
			cout << "Total length of the walk: " << path_length << "\t - \t Number of edges between nodes: " << num_edges_in_path << endl;
			flag = false;
		}
		else
			cerr<<"Invalid vertices: Cell0D does not contain vertices of id either " << id_vertex_1 << " or " << id_vertex_2 << "or both; impossible to generate the shortest path. Proceeding with only the exportation of the mesh."<< endl;
	}
	
	if (flag) {
		Gedim::UCDUtilities utilities;
		utilities.ExportPoints("./Cell0Ds.inp",
								GeodeticPolyhedron.Cell0DsCoordinates);
		utilities.ExportSegments("./Cell1Ds.inp",
								GeodeticPolyhedron.Cell0DsCoordinates,
								GeodeticPolyhedron.Cell1DsExtrema);
	}
	
	if (!FileManagement::ExportOutputFiles(GeodeticPolyhedron)){
		cerr << "Error during the export of .txt files" << endl;
		return 1;
	}
	
	return 0;
}
