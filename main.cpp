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
	
	// COSTRUZIONE DELLA MESH DEL POLIEDRO GEODETICO E DEL SUO DUALE
	PolyhedronMesh GeodeticPolyhedron;
	if ( b > 0 && c == 0){
		GenerateGeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, b);
		CreateDual(GeodeticPolyhedron);
	}
	else if ( b == 0 && c > 0){
		GenerateGeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, c);
		CreateDual(GeodeticPolyhedron);
	}
	
	//STAMPA SU TERMINALE SOLO PER CONFERMA, ELIMINARE A TEMPO DEBITO
	cout<<"Id dei vertici: "<<endl;
	for(int i = 0; i < GeodeticPolyhedron.NumCell0Ds; i++)
		cout<<GeodeticPolyhedron.Cell0DsId[i]<<endl;
	
	cout<<"Coordinate dei vertici: "<<endl;
	for(int i = 0; i< GeodeticPolyhedron.NumCell0Ds; i++)
		cout<<GeodeticPolyhedron.Cell0DsCoordinates(0,i)<<" "<<GeodeticPolyhedron.Cell0DsCoordinates(1,i)<<" "<<GeodeticPolyhedron.Cell0DsCoordinates(2,i)<<endl;
	
	cout<<"Id degli edges: "<<endl;
	for(int i = 0; i < GeodeticPolyhedron.NumCell1Ds; i++)
		cout<<GeodeticPolyhedron.Cell1DsId[i]<<endl;
	
	cout<<"Origin e End degli edges: "<<endl;
	for(int i = 0; i < GeodeticPolyhedron.NumCell1Ds; i++)
		cout<<GeodeticPolyhedron.Cell1DsExtrema(0,i)<<" "<<GeodeticPolyhedron.Cell1DsExtrema(1,i)<<endl;
	
	cout<<"Facce e informazioni "<<endl;
	for(int i = 0; i < GeodeticPolyhedron.NumCell2Ds; i++){
		cout<<"id faccia: "<<GeodeticPolyhedron.Cell2DsId[i]<<endl;
		cout<<"Vertici della faccia: ";
		for(const auto& vertex:GeodeticPolyhedron.Cell2DsVertices[i])
			cout<<vertex<<" ";
		cout<<"Edges della faccia: ";
		for(const auto& edge:GeodeticPolyhedron.Cell2DsEdges[i])
			cout<<edge<<" ";
		cout<<endl;
	}
	
	for(const auto& id:GeodeticPolyhedron.Cell3DsId){
		cout<<"Id del poliedro: "<<id<<endl;
		cout<<"Numero vertici: "<<GeodeticPolyhedron.Cell3DsNumVertices[id]<<endl;
		cout<<"Numero edges: "<<GeodeticPolyhedron.Cell3DsNumEdges[id]<<endl;
		cout<<"Numero facce: "<<GeodeticPolyhedron.Cell3DsNumFaces[id]<<endl;
		cout<<"Vertici del Poliedro"<<endl;
		for(const auto & vertex: GeodeticPolyhedron.Cell3DsVertices[id])
			cout<<vertex<<endl;
		cout<<"Edges del Poliedro"<<endl;
		for(const auto & edge: GeodeticPolyhedron.Cell3DsEdges[id])
			cout<<edge<<endl;
		cout<<"Facce del Poliedro"<<endl;
		for(const auto & face: GeodeticPolyhedron.Cell3DsFaces[id])
			cout<<face<<endl;
	}
		
	
	//EXPORT SU PARAVIEW
	Gedim::UCDUtilities utilities;	
	
	utilities.ExportPoints("./Cell0Ds.inp",
                           GeodeticPolyhedron.Cell0DsCoordinates);

    utilities.ExportSegments("./Cell1Ds.inp",
								GeodeticPolyhedron.Cell0DsCoordinates,
								GeodeticPolyhedron.Cell1DsExtrema); 

	
	return 0;
}