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
	polyhedron.Cell2DsNumVertices.assign(polyhedron.NumCell2Ds, 3);
	polyhedron.Cell2DsNumEdges.assign(polyhedron.NumCell2Ds, 3);
	
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
	int duplicate_id = 0;	// id che servirà per controllare se esiste un duplicato
	int edge_id = 0;		// id degli spigoli del poliedro geodetico che andremo a generare
	int face_id = 0;		// id delle facce del poliedro geodetico che andremo a generare
	int total_points = (PlatonicPolyhedron.NumCell2Ds)*((num_segments + 1) * (num_segments + 2) / 2);	// numero totale MASSIMO di vertici
	int total_edges = 30*(num_segments*num_segments);													// numero totale MASSIMO di spigoli
	int total_faces = 20*(num_segments*num_segments);													// numero totale MASSIMO di facce
	
	
	// usiamo i total points per allocare memoria
	// alcune strutture sono inizializzate a più memoria di quanto sarebbe sufficiente
	// a fine funzione si fa un conservative resize per ridimensionare correttamente
	GeodeticSolid.Cell0DsId.reserve(total_points);
	GeodeticSolid.Cell0DsCoordinates = MatrixXd::Zero(3,total_points);	
	
	GeodeticSolid.Cell1DsExtrema = MatrixXi::Zero(2, total_edges);
	GeodeticSolid.Cell1DsId.reserve(total_edges);
	
	GeodeticSolid.Cell2DsId.reserve(total_faces);
	GeodeticSolid.Cell2DsEdges.resize(total_faces);
	GeodeticSolid.Cell2DsNumEdges.resize(total_faces);
	GeodeticSolid.Cell2DsVertices.resize(total_faces);
	GeodeticSolid.Cell2DsNumVertices.resize(total_faces);
	
	// point_coefficients contiene in chiave 4 valori: {a, b, c, id} dove a, b, c sono 3 valori che individuano i vertici 
	// su una faccia mentre id è l'identificativo della faccia. Il valore è l'id del vertice
	// questa struttura serve per identificare tutti i vertici che una faccia contiene, tenendo conto anche di eventuali duplicati 
	// che si trovano su più facce
	map<array<int, 4>, int> point_coefficients;
	
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
				
				// coordinate dei vertici del poliedro geodetico
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
	
	// conservativeResize della matrice secondo il numero effettivo di vertici che abbiamo creato
	// l'inizializzazione ha "generato" degli zeri che andiamo a eliminare
	GeodeticSolid.Cell0DsCoordinates.conservativeResize(3, GeodeticSolid.NumCell0Ds);
	ProjectionOnSphere(GeodeticSolid);
	
	// GENERAZIONE SPIGOLI E FACCE
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
					
					if(!CheckDuplicatesEdge(GeodeticSolid.Cell1DsExtrema, originVertex, endVertex, edge_id, duplicate_id)){
						GeodeticSolid.NumCell1Ds++;
						GeodeticSolid.Cell1DsId.push_back(edge_id);
						GeodeticSolid.Cell1DsExtrema(0, edge_id) = originVertex;
						GeodeticSolid.Cell1DsExtrema(1, edge_id) = endVertex;
						GeodeticSolid.Cell2DsEdges[face_id][k] = edge_id;
						edge_id++;
					}
					else
						GeodeticSolid.Cell2DsEdges[face_id][k] = GeodeticSolid.Cell1DsId[duplicate_id];
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
						
						if(!CheckDuplicatesEdge(GeodeticSolid.Cell1DsExtrema, originVertex, endVertex, edge_id, duplicate_id)){
							GeodeticSolid.NumCell1Ds++;
							GeodeticSolid.Cell1DsId.push_back(edge_id);
							GeodeticSolid.Cell1DsExtrema(0, edge_id) = originVertex;
							GeodeticSolid.Cell1DsExtrema(1, edge_id) = endVertex;
							GeodeticSolid.Cell2DsEdges[face_id][k] = edge_id;
							edge_id++;
						}
						else
							GeodeticSolid.Cell2DsEdges[face_id][k] = GeodeticSolid.Cell1DsId[duplicate_id];
					}
					face_id++;
				}
				
			}
		}
	}
	GeodeticSolid.Cell1DsExtrema.conservativeResize(2, GeodeticSolid.NumCell1Ds);
	GeodeticSolid.Cell2DsNumVertices.resize(GeodeticSolid.NumCell2Ds);
	GeodeticSolid.Cell2DsNumEdges.resize(GeodeticSolid.NumCell2Ds);
	GeodeticSolid.Cell2DsVertices.resize(GeodeticSolid.NumCell2Ds);
	GeodeticSolid.Cell2DsEdges.resize(GeodeticSolid.NumCell2Ds);
	
	// GENERAZIONE POLIEDRO
	GeodeticSolid.NumCell3Ds++;
	GeodeticSolid.Cell3DsId.push_back(0);
	GeodeticSolid.Cell3DsNumVertices.push_back(GeodeticSolid.NumCell0Ds);
	GeodeticSolid.Cell3DsNumEdges.push_back(GeodeticSolid.NumCell1Ds);
	GeodeticSolid.Cell3DsNumFaces.push_back(GeodeticSolid.NumCell2Ds);
	GeodeticSolid.Cell3DsVertices.push_back(GeodeticSolid.Cell0DsId);
	GeodeticSolid.Cell3DsEdges.push_back(GeodeticSolid.Cell1DsId);
	GeodeticSolid.Cell3DsFaces.push_back(GeodeticSolid.Cell2DsId);
}

/************************************/

void CreateDual(PolyhedronMesh& Polyhedron)
{
	//In teoria basta cambiare qui per generalizzare 
	int baricenter_id = Polyhedron.Cell0DsId.back()+1;
	int dual_face_id = Polyhedron.Cell2DsId.back()+1;
	int dual_edge_id = Polyhedron.Cell1DsId.back()+1;
	int StartNumCells0Ds = Polyhedron.NumCell0Ds;
	int StartNumCells1Ds = Polyhedron.NumCell1Ds;
	int StartNumCells2Ds = Polyhedron.NumCell2Ds;
	int DualNumCell0Ds = Polyhedron.NumCell2Ds;
	int DualNumCell1Ds = Polyhedron.NumCell1Ds;
	int DualNumCell2Ds = Polyhedron.NumCell0Ds;
	int duplicate_id = 0;
	
	Polyhedron.Cell2DsVertices.resize(Polyhedron.NumCell2Ds + DualNumCell2Ds-1);
	Polyhedron.Cell2DsEdges.resize(Polyhedron.NumCell1Ds + DualNumCell1Ds-1);
	
	Polyhedron.Cell0DsCoordinates.conservativeResize(3, Polyhedron.NumCell0Ds + DualNumCell0Ds);
	Polyhedron.Cell1DsExtrema.conservativeResize(2, Polyhedron.NumCell1Ds + DualNumCell1Ds);
	
	vector<int> DualVertices;
	vector<int> DualEdges;
	vector<int> DualFaces;
	map<int, int> Faces_bar;
	
	//Generazione vertici del duale
	for(int i = 0; i <DualNumCell0Ds; i++){
		Vector3d baricenter_coordinates;
		
		Vector3d Vertex1 = Polyhedron.Cell0DsCoordinates.col(Polyhedron.Cell2DsVertices[i][0]);
		Vector3d Vertex2 = Polyhedron.Cell0DsCoordinates.col(Polyhedron.Cell2DsVertices[i][1]);
		Vector3d Vertex3 = Polyhedron.Cell0DsCoordinates.col(Polyhedron.Cell2DsVertices[i][2]);
		
		baricenter_coordinates = (1.0/3.0)*Vertex1 + (1.0/3.0)*Vertex2 + (1.0/3.0)*Vertex3;
		Polyhedron.Cell0DsId.push_back(baricenter_id);
		
		DualVertices.push_back(baricenter_id);
		
		
		Polyhedron.Cell0DsCoordinates(0, baricenter_id) = baricenter_coordinates(0);
		Polyhedron.Cell0DsCoordinates(1,baricenter_id) = baricenter_coordinates(1);
		Polyhedron.Cell0DsCoordinates(2, baricenter_id) = baricenter_coordinates(2);
		
		Faces_bar[i] = baricenter_id;
		baricenter_id ++;
		Polyhedron.NumCell0Ds++;
	}
	
	
	for(int i = 0; i < StartNumCells0Ds; i++){
		vector<int> VertexFaces;
		for(int j = 0; j < StartNumCells2Ds; j++){
			for(int k = 0; k<3; k++){
				if (Polyhedron.Cell2DsVertices[j][k] == Polyhedron.Cell0DsId[i]){

					//Se la faccia a cui sono arrivato contiene il vertice, allora la aggiungo al vettore 
					//delle facce avente quel vertice in comune
					VertexFaces.push_back(j);
					break;
				}
			}
		}
		
		vector<int> ordered_faces;
		order_faces(VertexFaces, ordered_faces, Polyhedron);

		int valence = ordered_faces.size();
		
		vector<int> New_vertices;
		for(const auto& VertexFace: ordered_faces)
			New_vertices.push_back(Faces_bar[VertexFace]);
		
		//Generazione facce e vertici del duale (come nel geodetico ma con k punti per faccia anzichè 3)
		Polyhedron.Cell2DsId.push_back(dual_face_id);
		Polyhedron.Cell2DsVertices[dual_face_id] = New_vertices;
		Polyhedron.Cell2DsEdges[dual_face_id].resize(valence);
		
		
		for (int k = 0; k < valence; k++) {
			int originVertex = Polyhedron.Cell2DsVertices[dual_face_id][k];
			int endVertex;
			if ( k == valence-1 )
				endVertex = Polyhedron.Cell2DsVertices[dual_face_id][0];
			else
				endVertex = Polyhedron.Cell2DsVertices[dual_face_id][k+1];
			if(!CheckDuplicatesEdge(Polyhedron.Cell1DsExtrema, originVertex, endVertex, dual_edge_id, duplicate_id)){
				Polyhedron.Cell1DsId.push_back(dual_edge_id);
				Polyhedron.Cell1DsExtrema(0, dual_edge_id) = originVertex;
				Polyhedron.Cell1DsExtrema(1, dual_edge_id) = endVertex;
				Polyhedron.Cell2DsEdges[dual_face_id][k] = dual_edge_id;
				
				DualEdges.push_back(dual_edge_id);
				
				dual_edge_id++;
				Polyhedron.NumCell1Ds++;
			}
			else
				Polyhedron.Cell2DsEdges[dual_face_id][k] = Polyhedron.Cell1DsId[duplicate_id];
		}
		
		Polyhedron.NumCell2Ds++;
		DualFaces.push_back(dual_face_id);
		dual_face_id++;
	}
	
	Polyhedron.Cell3DsId.push_back(1);
	
	Polyhedron.Cell2DsVertices.resize(dual_face_id);
	Polyhedron.Cell2DsEdges.resize(dual_edge_id);
	Polyhedron.Cell0DsCoordinates.conservativeResize(3, baricenter_id);
	Polyhedron.Cell1DsExtrema.conservativeResize(2, dual_edge_id);
	
	Polyhedron.Cell3DsNumVertices.push_back(DualNumCell0Ds);
	Polyhedron.Cell3DsNumEdges.push_back(DualNumCell1Ds);
	Polyhedron.Cell3DsNumFaces.push_back(DualNumCell2Ds);
	
	Polyhedron.Cell3DsVertices.push_back(DualVertices);
	Polyhedron.Cell3DsEdges.push_back(DualEdges);
	Polyhedron.Cell3DsFaces.push_back(DualFaces);
	
	ProjectionOnSphere(Polyhedron);
	Polyhedron.NumCell3Ds++;

}

/************************************/

void order_faces(const vector<int>& unordered_faces, vector<int>& ordered_faces, const PolyhedronMesh& Polyhedron)
{	
	//Il vettore di facce rimanenti contiene le facce ancora da ordinare, esso è all'inizio uguale a tutto il vettore.
    vector<int> remaining_faces = unordered_faces;

    // Inizio da una faccia qualsiasi, e la metto nel vettore ordinato
    int current = remaining_faces[0];
    ordered_faces.push_back(current);
    remaining_faces.erase(remaining_faces.begin());

	//Fino a quando ho facce da ordinare
    while (!remaining_faces.empty()) {
		
		//Prendo gli edge della faccia corrente
        vector<int> current_edges = Polyhedron.Cell2DsEdges[current];
        bool found = false;
        int found_index;

        // Cerco tra le facce da ordinare quella che ha un edge in comune con la corrente
        for (size_t i = 0; i < remaining_faces.size(); i++) {
            vector<int> candidate_edges = Polyhedron.Cell2DsEdges[remaining_faces[i]];

            // Verifico la presenza dell' edge in comune
            for (int e1 : current_edges) {
                for (int e2 : candidate_edges) {
                    if (e1 == e2) {
                        found = true;
                        found_index = i;
                        break;
                    }
                }
                if (found) 
					break;
            }
            if (found) 
				break;
        }

        if (found) {
			
			//Se ho trovato l'edge comune, allora metto la faccia con l'edge comune dentro a ordered_faces
			//e aggiorno current, in modo da ripartire con il while cercando la faccia che ha un edge in comune con
			//quella appena trovata
            current = remaining_faces[found_index];
            ordered_faces.push_back(current);
            remaining_faces.erase(remaining_faces.begin() + found_index);
        }
    }
}


/************************************/

bool CheckDuplicatesVertex(const MatrixXd& mat, const Vector3d& vec, int& matSize, int& duplicate_id)
{
	for(int i = 0; i < matSize; i++){
		if( (mat.col(i) - vec).norm() < 1e-8 ){
			duplicate_id = i;
			return true;
		}
	}
	return false;
}

/************************************/

bool CheckDuplicatesEdge(const MatrixXi& mat, const int& v1, const int& v2, int& matSize, int& duplicate_id)
{
	for (int i = 0; i < matSize; i++){
		int w1 = mat(0, i);
		int w2 = mat(1, i);
		
		if( (v1 == w1 && v2 == w2) || (v1 == w2 && v2 == w1) ){
			duplicate_id = i;
			return true;
		}
		
	}
	return false;
}

/************************************/

void ProjectionOnSphere(PolyhedronMesh& mesh) {
	for(int i = 0; i < mesh.NumCell0Ds; i++){
		double Norm_factor = (mesh.Cell0DsCoordinates.col(i)).norm();
		mesh.Cell0DsCoordinates(0,i) = mesh.Cell0DsCoordinates(0,i)/Norm_factor;
		mesh.Cell0DsCoordinates(1,i) = mesh.Cell0DsCoordinates(1,i)/Norm_factor;
		mesh.Cell0DsCoordinates(2,i) = mesh.Cell0DsCoordinates(2,i)/Norm_factor;
	}
}

/************************************/