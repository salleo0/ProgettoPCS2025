#include <sstream>
#include "PolyhedronMesh.hpp"
#include "utils.hpp"
#include "UCDUtilities.hpp"

using namespace std;

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
	if (argc == 7)
		convert >> id_vertex_1 >> id_vertex_2;
	
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
	
	if(!ImportPolyhedronMesh(PlatonicPolyhedron, InputFile)){
		cerr << "Something went wrong during the creation of the platonic polyhedron mesh" << endl;
		return 1;
	}
	
	// COSTRUZIONE DELLA MESH DEL POLIEDRO GEODETICO
	PolyhedronMesh GeodeticPolyhedron;
	if ( b > 0 && c == 0)
		GenerateGeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, b);
	else if ( b == 0 && c > 0)
		GenerateGeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, c);
	
	
	/*for(int i =0; i<GeodeticPolyhedron.NumCell1Ds;i++)
		cout<<"edge id: "<<GeodeticPolyhedron.Cell1DsId[i]<<" Origin and end: "<<GeodeticPolyhedron.Cell1DsExtrema(0,GeodeticPolyhedron.Cell1DsId[i])<<" "<<GeodeticPolyhedron.Cell1DsExtrema(1,GeodeticPolyhedron.Cell1DsId[i])<<endl;
	for(int i = 0; i<GeodeticPolyhedron.NumCell2Ds;i++){
		cout<<"Face id: "<<i<<endl;
		cout<<"Vertices: "<<GeodeticPolyhedron.Cell2DsVertices[i][0]<<" "<<GeodeticPolyhedron.Cell2DsVertices[i][1]<<" "<<GeodeticPolyhedron.Cell2DsVertices[i][2]<<endl;
		cout<<"Edges: "<<GeodeticPolyhedron.Cell2DsEdges[i][0]<<" "<<GeodeticPolyhedron.Cell2DsEdges[i][1]<<" "<<GeodeticPolyhedron.Cell2DsEdges[i][2]<<endl;
		}*/
		
	Gedim::UCDUtilities utilities;	
    utilities.ExportPoints("./Cell0Ds.inp",
                           GeodeticPolyhedron.Cell0DsCoordinates);

    utilities.ExportSegments("./Cell1Ds.inp",
                             GeodeticPolyhedron.Cell0DsCoordinates,
                             GeodeticPolyhedron.Cell1DsExtrema); 
	return 0;
}