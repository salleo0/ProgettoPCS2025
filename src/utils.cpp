#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Eigen/Eigen"
#include"utils.hpp"
using namespace Eigen;
using namespace std;

bool ImportPolyhedronMesh(PolyhedronMesh& polyhedron, const string& InputFile){
	
	if(!ImportCell0Ds(polyhedron,InputFile+"Cell0Ds.csv"))
		return false;
	
	if(!ImportCell1Ds(polyhedron,InputFile+"Cell1Ds.csv"))
		return false;
	
	if(!ImportCell2Ds(polyhedron,InputFile+"Cell2Ds.csv"))
		return false;
	
	// Le seguenti righe sono solo per controllare che tutta la memorizzazione sia stata
	// effettuata correttamente, cancellare a tempo debito 
	
	/*
	for(unsigned int i = 0; i < polyhedron.NumCell0Ds;i++){
		cout << polyhedron.Cell0DsId[i] << endl;
		cout << polyhedron.Cell0DsCoordinates(0,i) << " " << polyhedron.Cell0DsCoordinates(1,i) << " ";
		cout << polyhedron.Cell0DsCoordinates(2,i) << endl;
	}
	*/
	
	/*	
	for(unsigned int i = 0; i < polyhedron.NumCell1Ds; i++){
		cout << polyhedron.Cell1DsId[i] << endl;
		cout << polyhedron.Cell1DsExtrema(0,i) << " " << polyhedron.Cell1DsExtrema(1,i) << endl;
	} 
	*/
	
	/*
	for(unsigned int i = 0; i < polyhedron.NumCell2Ds; i++){
		cout << polyhedron.Cell2DsId[i] << endl;
		for(unsigned int j = 0; j < 3; j++){
			cout << polyhedron.Cell2DsVertices[i][j] << " ";
		}
		cout << endl;
		for(unsigned int j = 0; j < 3; j++){
			cout << polyhedron.Cell2DsEdges[i][j] << " ";
		}
		cout << endl;
	}
	*/
	
	return true;
	
}

/************************************/

bool ImportCell0Ds(PolyhedronMesh& polyhedron, const string& InputFile)
{
	ifstream file(InputFile);
	if(file.fail()){
		cerr << "Unable to open Cell0Ds.csv file" << endl;
		return false;
	}
	
	// Importo in una lista tutte le righe del file 
	list<string> listLines;
	string line;
	while(getline(file, line))
		listLines.push_back(line);
	file.close();
	
	//remove header
	listLines.pop_front();
	
	polyhedron.NumCell0Ds = listLines.size();
	if(polyhedron.NumCell0Ds == 0){
		cerr << "There are no Cells 0D" << endl;
		return false;
	}
	
	polyhedron.Cell0DsId.reserve(polyhedron.NumCell0Ds);
	polyhedron.Cell0DsCoordinates = MatrixXd::Zero(3,polyhedron.NumCell0Ds);
	
	for (const string& str: listLines){
		string line = str;
		replace(line.begin(), line.end(), ';', ' ');
		istringstream converter(line);
		
		unsigned int id;
		
		converter >> id >> polyhedron.Cell0DsCoordinates(0,id) >> polyhedron.Cell0DsCoordinates(1,id) >> polyhedron.Cell0DsCoordinates(2,id);
		
		polyhedron.Cell0DsId.push_back(id);
	}
	return true;
}

/************************************/

bool ImportCell1Ds(PolyhedronMesh& polyhedron, const string& InputFile)
{
	ifstream file(InputFile);
	
	if(file.fail()){
		cerr << "unable to open Cell1Ds.csv file" << endl;
		return false;
	}
	
	// importo in una lista tutte le righe del file
	list<string> listLines;
	string line;
	while(getline(file,line))
		listLines.push_back(line);
	
	file.close();
	
	// remove header
	listLines.pop_front();
	
	polyhedron.NumCell1Ds = listLines.size();
	
	if (polyhedron.NumCell1Ds == 0){
		cerr << "There is no cell 1D" << endl;
		return false;
	}
	
	// salvo le informazioni nelle righe in polyhedron	
	polyhedron.Cell1DsId.reserve(polyhedron.NumCell1Ds);
	polyhedron.Cell1DsExtrema = MatrixXi::Zero(2, polyhedron.NumCell1Ds);
	
	for (const string& str : listLines){	
		string line = str;
		replace(line.begin(), line.end(), ';', ' ');
		istringstream converter(line);
		
		unsigned int id;
		unsigned int Origin;
		unsigned int End;
		
		converter >> id >> Origin >> End;
		
		// test per verificare che nessun bordo abbia lunghezza zero
		if(Origin == End){
			cerr << "at least one edge has zero length";
			return false;
		}
		
		polyhedron.Cell1DsExtrema(0, id) = Origin;
		polyhedron.Cell1DsExtrema(1, id) = End;
		
		polyhedron.Cell1DsId.push_back(id);
	
	}
	return true;
}

/************************************/

bool ImportCell2Ds(PolyhedronMesh& polyhedron, const string& InputFile) 
{
	ifstream file(InputFile);
	
	if(file.fail()){
		cerr << "unable to open Cell2Ds.csv file" << endl;
		return false;
	}
	
	// importo in una lista tutte le righe del file
	list<string> listLines;
	string line;
	while (getline(file, line))
		listLines.push_back(line);
	
	file.close();
	
	// remove header
	listLines.pop_front();
	
	polyhedron.NumCell2Ds = listLines.size();
	
	if(polyhedron.NumCell2Ds == 0){
		cerr << "there is no cell 2D" << endl;
		return false;
	}
	
	// salvo le informazioni nelle righe in polyhedron
	polyhedron.Cell2DsId.reserve(polyhedron.NumCell2Ds);
	polyhedron.Cell2DsVertices.reserve(polyhedron.NumCell2Ds);
	polyhedron.Cell2DsEdges.reserve(polyhedron.NumCell2Ds);
	
	for (const string& str : listLines){
		string line = str;
		replace(line.begin(), line.end(), ';', ' ');
		istringstream converter(line);
		
		unsigned int id;
		
		converter >> id;
		
		polyhedron.Cell2DsId.push_back(id);
		
		// memorizzo i vertici e gli spigoli
		vector<unsigned int> vertices(3);
		for(unsigned int i = 0; i < 3; i++)
			converter >> vertices[i];
		polyhedron.Cell2DsVertices.push_back(vertices);
		
		vector<unsigned int> edges(3);
		for(unsigned int i = 0; i < 3; i++)
			converter >> edges[i];
		polyhedron.Cell2DsEdges.push_back(edges);
		
	}
	
	return true;
}

/************************************/

bool GenerateGeodeticSolid(const PolyhedronMesh& PlatonicPolyhedron, PolyhedronMesh& GeodeticSolid)
{
	
	return true;
}

/************************************/

//bool Generate