#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Eigen/Eigen"
#include"utils.hpp"
using namespace Eigen;
using namespace std;

bool ImportPolyhedronMesh(PolyhedronMesh& polyhedron, const string& InputFileDirectory){
	
	if(!ImportCell0Ds(polyhedron, InputFileDirectory + "Cell0Ds.csv"))
		return false;
	
	if(!ImportCell1Ds(polyhedron, InputFileDirectory + "Cell1Ds.csv"))
		return false;
	
	if(!ImportCell2Ds(polyhedron, InputFileDirectory + "Cell2Ds.csv"))
		return false;	
	
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
	
	// rimozione header
	listLines.pop_front();
	
	polyhedron.NumCell0Ds = listLines.size();
	if(polyhedron.NumCell0Ds == 0){
		cerr << "There are no Cells 0D" << endl;
		return false;
	}
	
	// salvo le informazioni nelle righe in polyhedron
	polyhedron.Cell0DsId.reserve(polyhedron.NumCell0Ds);
	polyhedron.Cell0DsCoordinates = MatrixXd::Zero(3,polyhedron.NumCell0Ds);
	
	for (string& line: listLines){
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
	
	// rimozione header
	listLines.pop_front();
	
	polyhedron.NumCell1Ds = listLines.size();
	if (polyhedron.NumCell1Ds == 0){
		cerr << "There is no cell 1D" << endl;
		return false;
	}
	
	// salvo le informazioni nelle righe in polyhedron	
	polyhedron.Cell1DsId.reserve(polyhedron.NumCell1Ds);
	polyhedron.Cell1DsExtrema = MatrixXi::Zero(2, polyhedron.NumCell1Ds);
	
	for (string& line : listLines){	
		replace(line.begin(), line.end(), ';', ' ');
		istringstream converter(line);
		int id;
		int Origin;
		int End;
		
		converter >> id >> Origin >> End;
		
		// test per verificare che nessun edge abbia lunghezza zero
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
	
	for (string& line : listLines){
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

void GenerateGeodeticSolidType1(const PolyhedronMesh& PlatonicPolyhedron, PolyhedronMesh& GeodeticSolid, const int& num_segments)
{
	int points_id = 0;		// id dei vertici del poliedro geodetico che andremo a generare
	int duplicate_id = 0;	// id che servirà per controllare se esiste un duplicato di un vertice
	int edge_id = 0;		// id degli spigoli del poliedro geodetico che andremo a generare
	int face_id = 0;		// id delle facce del poliedro geodetico che andremo a generare
	int duplicate_pos = 0;
	int total_points = (PlatonicPolyhedron.NumCell2Ds)*((num_segments + 1) * (num_segments + 2) / 2);	// numero totale MASSIMO di vertici
	int total_edges = 30*(num_segments*num_segments);													// numero totale MASSIMO di spigoli
	int total_faces = 20*(num_segments*num_segments);													// numero totale MASSIMO di facce
	
	
	// usiamo i total points per allocare memoria
	GeodeticSolid.Cell0DsId.reserve(total_points);
	GeodeticSolid.Cell0DsCoordinates = MatrixXd::Zero(3,total_points);
	
	GeodeticSolid.Cell1DsExtrema = MatrixXi::Zero(2, total_edges);
	GeodeticSolid.Cell1DsId.reserve(total_edges);
	
	GeodeticSolid.Cell2DsId.reserve(total_faces);
	GeodeticSolid.Cell2DsEdges.resize(total_faces);
	GeodeticSolid.Cell2DsNumEdges.resize(total_faces);
	GeodeticSolid.Cell2DsVertices.resize(total_faces);
	GeodeticSolid.Cell2DsNumVertices.resize(total_faces);
	
	// 
	map<array<int, 4>, int> point_coefficients;
	
	// scorro le facce del solido platonico
	for (const auto& id : PlatonicPolyhedron.Cell2DsId) {
		
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
				if (!CheckDuplicatesVertex(GeodeticSolid.Cell0DsCoordinates, PointCoordinates, points_id, duplicate_id)){
					point_coefficients[coefficients] = points_id;
					GeodeticSolid.Cell0DsId.push_back(points_id);
					GeodeticSolid.Cell0DsCoordinates(0,points_id) = PointCoordinates[0];
					GeodeticSolid.Cell0DsCoordinates(1,points_id) = PointCoordinates[1];
					GeodeticSolid.Cell0DsCoordinates(2,points_id) = PointCoordinates[2];
					points_id++;
					GeodeticSolid.NumCell0Ds++;
				}
				else
					point_coefficients[coefficients] = duplicate_id;
			}
		}
	}
	
	for(int i = 0; i <GeodeticSolid.NumCell0Ds;i++){
		double Norm_factor = (GeodeticSolid.Cell0DsCoordinates.col(i)).norm();
		GeodeticSolid.Cell0DsCoordinates(0,i) = GeodeticSolid.Cell0DsCoordinates(0,i)/Norm_factor;
		GeodeticSolid.Cell0DsCoordinates(1,i) = GeodeticSolid.Cell0DsCoordinates(1,i)/Norm_factor;
		GeodeticSolid.Cell0DsCoordinates(2,i) = GeodeticSolid.Cell0DsCoordinates(2,i)/Norm_factor;
	}
	
	for  (const auto& id : PlatonicPolyhedron.Cell2DsId){
		for (int i = 0; i < num_segments; i++){
			for (int j = 0; j < num_segments - i; j++){
				int Vertex1 = point_coefficients[{i,num_segments-i-j,j,id}];
				int Vertex2 = point_coefficients[{i,num_segments-i-(j+1),j+1,id}];
				int Vertex3 = point_coefficients[{i+1,num_segments-(i+1)-j,j,id}];
				// generazione triangolo "a punta in su"
				// face
				GeodeticSolid.NumCell2Ds++;
				GeodeticSolid.Cell2DsId.push_back(face_id);
				GeodeticSolid.Cell2DsNumVertices[face_id] = 3;
				GeodeticSolid.Cell2DsNumEdges[face_id] = 3;
				vector<int> VerticesVector = {Vertex1, Vertex2, Vertex3};
				GeodeticSolid.Cell2DsVertices[face_id] = VerticesVector;
				GeodeticSolid.Cell2DsEdges[face_id].resize(3);
				
				// edges
				for (int k = 0; k < 3; k++) {
					int originVertex = GeodeticSolid.Cell2DsVertices[face_id][k];
					int endVertex;
					if ( k == 2 )
						endVertex = GeodeticSolid.Cell2DsVertices[face_id][0];
					else
						endVertex = GeodeticSolid.Cell2DsVertices[face_id][k+1];
					if(!CheckDuplicatesEdge(GeodeticSolid.Cell1DsExtrema, originVertex, endVertex, edge_id, duplicate_pos)){
						GeodeticSolid.NumCell1Ds++;
						GeodeticSolid.Cell1DsId.push_back(edge_id);
						GeodeticSolid.Cell1DsExtrema(0, edge_id) = originVertex;
						GeodeticSolid.Cell1DsExtrema(1, edge_id) = endVertex;
						GeodeticSolid.Cell2DsEdges[face_id][k] = edge_id;
						edge_id++;
					}
					else
						GeodeticSolid.Cell2DsEdges[face_id][k] = GeodeticSolid.Cell1DsId[duplicate_pos];
				}
				face_id++;
				// generazione triangolo "a punta in giù"
				if(i>0){
					
					int Vertex4 = point_coefficients[{i-1,num_segments-(i-1)-(j+1),(j+1),id}];
					GeodeticSolid.NumCell2Ds++;
					GeodeticSolid.Cell2DsId.push_back(face_id);
					GeodeticSolid.Cell2DsNumVertices[face_id] = 3;
					GeodeticSolid.Cell2DsNumEdges[face_id] = 3;
					VerticesVector[2] = Vertex4;
					GeodeticSolid.Cell2DsVertices[face_id] = VerticesVector;
					GeodeticSolid.Cell2DsEdges[face_id].resize(3);
					for (int k = 0; k < 3; k++) {
						int originVertex = GeodeticSolid.Cell2DsVertices[face_id][k];
						int endVertex;
						if ( k == 2 )
							endVertex = GeodeticSolid.Cell2DsVertices[face_id][0];
						else
							endVertex = GeodeticSolid.Cell2DsVertices[face_id][k+1];
						if(!CheckDuplicatesEdge(GeodeticSolid.Cell1DsExtrema, originVertex, endVertex, edge_id, duplicate_pos)){
							GeodeticSolid.NumCell1Ds++;
							GeodeticSolid.Cell1DsId.push_back(edge_id);
							GeodeticSolid.Cell1DsExtrema(0, edge_id) = originVertex;
							GeodeticSolid.Cell1DsExtrema(1, edge_id) = endVertex;
							GeodeticSolid.Cell2DsEdges[face_id][k] = edge_id;
							edge_id++;
						}
						else
							GeodeticSolid.Cell2DsEdges[face_id][k] = GeodeticSolid.Cell1DsId[duplicate_pos];
					}
					face_id++;
				}
			}
		}
	}
}

/************************************/

bool CheckDuplicatesVertex(const MatrixXd& mat, const Vector3d& vec, int& matSize, int& duplicate_pos)
{
	for(int i = 0; i < matSize; i++){
		if( (mat.col(i) - vec).norm() < 1e-8 ){
			duplicate_pos = i;
			return true;
		}
	}
	return false;
}

/************************************/

bool CheckDuplicatesEdge(const MatrixXi& mat, const int& v1, const int& v2, int& matSize, int& duplicate_pos)
{
	for (int i = 0; i < matSize; i++){
		int w1 = mat(0, i);
		int w2 = mat(1, i);
		
		if( (v1 == w1 && v2 == w2) || (v1 == w2 && v2 == w1) ){
			duplicate_pos = i;
			return true;
		}
		
	}
	return false;
}