#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Eigen/Eigen"
#include"utils.hpp"
using namespace Eigen;
using namespace std;

bool ImportPolyhedronMesh(PolyhedronMesh& polyhedron, const string& InputFileDirectory){
	
	if(!ImportCell0Ds(polyhedron,InputFileDirectory + "Cell0Ds.csv"))
		return false;
	
	if(!ImportCell1Ds(polyhedron,InputFileDirectory + "Cell1Ds.csv"))
		return false;
	
	if(!ImportCell2Ds(polyhedron,InputFileDirectory + "Cell2Ds.csv"))
		return false;
	
	// Le seguenti righe sono solo per controllare che tutta la memorizzazione sia stata
	// effettuata correttamente, cancellare a tempo debito 
	
	/*
	for( int i = 0; i < polyhedron.NumCell0Ds;i++){
		cout << polyhedron.Cell0DsId[i] << endl;
		cout << polyhedron.Cell0DsCoordinates(0,i) << " " << polyhedron.Cell0DsCoordinates(1,i) << " ";
		cout << polyhedron.Cell0DsCoordinates(2,i) << endl;
	}
	*/
	
	/*	
	for( int i = 0; i < polyhedron.NumCell1Ds; i++){
		cout << polyhedron.Cell1DsId[i] << endl;
		cout << polyhedron.Cell1DsExtrema(0,i) << " " << polyhedron.Cell1DsExtrema(1,i) << endl;
	} 
	*/
	
	/*
	for( int i = 0; i < polyhedron.NumCell2Ds; i++){
		cout << polyhedron.Cell2DsId[i] << endl;
		for( int j = 0; j < 3; j++){
			cout << polyhedron.Cell2DsVertices[i][j] << " ";
		}
		cout << endl;
		for( int j = 0; j < 3; j++){
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
	
	// Remove header
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
		
		 int id;
		
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
	
	// Remove header
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
		
		int id;
		int Origin;
		int End;
		
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
		
		int id;
		
		converter >> id;
		
		polyhedron.Cell2DsId.push_back(id);
		
		// memorizzo i vertici e gli spigoli
		vector<int> vertices(3);
		for(int i = 0; i < 3; i++)
			converter >> vertices[i];
		
		polyhedron.Cell2DsVertices.push_back(vertices);
		
		vector<int> edges(3);
		for(int i = 0; i < 3; i++)
			converter >> edges[i];
		
		polyhedron.Cell2DsEdges.push_back(edges);
		
	}
	
	return true;
}

/************************************/

bool GenerateGeodeticSolidType1(const PolyhedronMesh& PlatonicPolyhedron, PolyhedronMesh& GeodeticSolid, const int& num_segments)
{
	int points_id = 0;		// id dei punti che andremo a generare
	int duplicate_id = 0;	// id che servirà per controllare se esiste già un duplicato
	
	int total_points = (PlatonicPolyhedron.NumCell2Ds)*((num_segments + 1) * (num_segments + 2) / 2);	// numero totale di punti dato b
	
	// usiamo total points per allocare memoria
	GeodeticSolid.Cell0DsId.reserve(total_points);
	GeodeticSolid.Cell0DsCoordinates = MatrixXd::Zero(3,total_points);
	
	//
	map<array<int, 4>, int> point_coefficients;
	
	int edge_id = 0;
	int face_id = 0;
	
	int total_edges = 30*(num_segments^2);
	int total_faces = 20*(num_segments^2);
	GeodeticSolid.Cell1DsId.reserve(total_edges);
	GeodeticSolid.Cell2DsId.reserve(total_faces);
	
	// scorro le facce del solido platonico
	for (const auto id : PlatonicPolyhedron.Cell2DsId) {
		
		// salvo in 3 vettori le coordinate dei vertici della faccia corrente del solido platonico
		Vector3d Vertex1 = PlatonicPolyhedron.Cell0DsCoordinates.col(PlatonicPolyhedron.Cell2DsVertices[id][0]);
		Vector3d Vertex2 = PlatonicPolyhedron.Cell0DsCoordinates.col(PlatonicPolyhedron.Cell2DsVertices[id][1]);
		Vector3d Vertex3 = PlatonicPolyhedron.Cell0DsCoordinates.col(PlatonicPolyhedron.Cell2DsVertices[id][2]);
		
		// GENERAZIONE DEI VERTICI DEL POLIEDRO GEODETICO
		for (int i = 0; i <= num_segments; i++) {
			for (int j = 0; j <= i; j++) {
				
				// pesi dei punti
				int a = num_segments - i;
				int b = i - j;
				int c = j;
				
				// coordinate dei punti del poliedro geodetico
				Vector3d PointCoordinates = double(a)/num_segments*Vertex1 + double(b)/num_segments*Vertex2 + double(c)/num_segments*Vertex3;
				
				// aggiungo al dizionario chiave = [a,b,c] e valore = id
				array<int, 4> coefficients;
				coefficients[0] = a;
				coefficients[1] = b;
				coefficients[2] = c;
				coefficients[3] = id;
					
					
				// se il punto PointCoordinates è gia presente in GeodeticSolid.Cell0DsCoordinates non lo aggiungo nuovamente
				if (!CheckDuplicates(GeodeticSolid.Cell0DsCoordinates, PointCoordinates, points_id, duplicate_id)){
					point_coefficients[coefficients] = points_id;
					GeodeticSolid.Cell0DsId.push_back(points_id);
					GeodeticSolid.Cell0DsCoordinates(0,points_id) = PointCoordinates[0];
					GeodeticSolid.Cell0DsCoordinates(1,points_id) = PointCoordinates[1];
					GeodeticSolid.Cell0DsCoordinates(2,points_id) = PointCoordinates[2];
					points_id ++;
					GeodeticSolid.NumCell0Ds++;
				}
				else
					point_coefficients[coefficients] = duplicate_id;
				
			}
		}
		
		for (int i = 0; i < num_segments; i++){
			for (int j = 0; j < num_segments - i; j++){
				int Origin = point_coefficients[{i, num_segments - i - j, j, id}];
				int End = point_coefficients[{i, num_segments - i - j, j + 1, id}];
				cout << Origin << " " << End << endl;
			}
		}
	}
	
	/*for(int i = 0; i < points_id;i++){
		cout << GeodeticSolid.Cell0DsId[i] << endl;
		cout << GeodeticSolid.Cell0DsCoordinates(0,i) << " " << GeodeticSolid.Cell0DsCoordinates(1,i) << " ";
		cout << GeodeticSolid.Cell0DsCoordinates(2,i) << endl;
	}*/
	
	
	for(auto &itor : point_coefficients){
		cout<<"coefficienti: "<<itor.first[0]<<" "<<itor.first[1]<<" "<<itor.first[2]<<" "<<itor.first[3]<<endl;
		cout<<"id del punto: "<<itor.second<<endl;
		
	}
	
	
	return true;
}

/************************************/

// funzione che guarda se vec è gia presewnte come colonna di mat
bool CheckDuplicates(const MatrixXd& mat, const Vector3d& vec, int matSize, int& duplicate_pos)
{
	for(int i = 0; i < matSize; i++){
		if( (mat.col(i) - vec).norm() < 1e-16 ){
			duplicate_pos = i;
			return true;
		}
	}
	return false;
}
