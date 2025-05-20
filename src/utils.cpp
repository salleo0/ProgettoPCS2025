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
}

/************************************/

void CreateDual(PolyhedronMesh& StartPolyhedron, PolyhedronMesh& DualPolyhedron)
{
	//In teoria basta cambiare qui per generalizzare 
	int baricenter_id = 0;
	int face_id = 0;
	int edge_id = 0;
	
	//Il Duale ha un numero di facce uguale al numero di vertici del poliedro di partenza
	DualPolyhedron.NumCell0Ds = StartPolyhedron.NumCell2Ds;
	DualPolyhedron.Cell0DsId.resize(DualPolyhedron.NumCell0Ds);
	DualPolyhedron.Cell0DsCoordinates = MatrixXd::Zero(3,DualPolyhedron.NumCell0Ds);
	
	//Il Duale ha lo stesso numero di edges del poliedro di partenza, grazie alla formula di Eulero, qualunque sia la varietà su cui si fa la mesh :)
	DualPolyhedron.NumCell1Ds = StartPolyhedron.NumCell1Ds;
	
	//Il Duale ha un numero di facce uguale al numero di vertici del poliedro di partenza
	DualPolyhedron.NumCell2Ds = StartPolyhedron.NumCell0Ds;
	DualPolyhedron.Cell1DsId.resize(DualPolyhedron.NumCell1Ds);
	DualPolyhedron.Cell1DsExtrema = MatrixXi::Zero(2, DualPolyhedron.NumCell1Ds);
	DualPolyhedron.Cell2DsId.resize(DualPolyhedron.NumCell2Ds);
	DualPolyhedron.Cell2DsEdges.resize(DualPolyhedron.NumCell2Ds);
	DualPolyhedron.Cell2DsVertices.resize(DualPolyhedron.NumCell2Ds);
	int duplicate_id = 0;
	
	//Mappa che associa all'id della faccia l'id del baricentro corrispondente, da usare se cambiassimo gli id dopo, per ora sono uguali
	map<int, int> Faces_bar;
	for (const auto& id : StartPolyhedron.Cell2DsId) {
		Vector3d baricenter;
		
		// salvo in 3 vettori le coordinate dei vertici della faccia corrente del solido platonico
		Vector3d Vertex1 = StartPolyhedron.Cell0DsCoordinates.col(StartPolyhedron.Cell2DsVertices[id][0]);
		Vector3d Vertex2 = StartPolyhedron.Cell0DsCoordinates.col(StartPolyhedron.Cell2DsVertices[id][1]);
		Vector3d Vertex3 = StartPolyhedron.Cell0DsCoordinates.col(StartPolyhedron.Cell2DsVertices[id][2]);
		
		// salvo le coordinate del baricentro
		baricenter = (1.0/3.0)*Vertex1 + (1.0/3.0)*Vertex2 + (1.0/3.0)*Vertex3;
		DualPolyhedron.Cell0DsId.push_back(baricenter_id);
		
		// salvo le coordinate dentro alle Cell0DsCoordinates del poliedro Duale
		DualPolyhedron.Cell0DsCoordinates(0, id) = baricenter(0);
		DualPolyhedron.Cell0DsCoordinates(1,id) = baricenter(1);
		DualPolyhedron.Cell0DsCoordinates(2, id) = baricenter(2);
		
		//Associo all'id della faccia l'id del baricentro nella mappa, per ora ridondante ma poi sarà meglio
		Faces_bar[id] = baricenter_id;
		baricenter_id ++;
	}
	
	for(const auto& vertex_id: StartPolyhedron.Cell0DsId){
		
		//Vettore che contiene le facce che hanno il vertice in comune
		vector<int> VertexFaces;
		for(const auto& face_id: StartPolyhedron.Cell2DsId){
			for(int j = 0; j<3; j++){
				if (StartPolyhedron.Cell2DsVertices[face_id][j] == vertex_id){
					
					//Se la faccia a cui sono arrivato contiene il vertice, allora la aggiungo al vettore 
					//delle facce avente quel vertice in comune
					VertexFaces.push_back(face_id);
					break;
				}
			}
		}
		
		//PROBLEMA: il vettore VertexFaces contiene tutte le facce adiacenti a un vertice ma NON è ordinato in modo sensato, 
		//per costruire gli edges devo ordinarlo in modo che ogni faccia nel vettore abbia come successiva la faccia adiacente, 
		//ovvero quella che ha l'edge in comune con la faccia corrente, per ordinare questo vettore chiamo la funzione order_faces
		//il vettore ordered_Faces è passato per riferimento, in modo che venga aggiornato dalla funzione order_Faces.
		vector<int> ordered_faces;
		order_faces(VertexFaces, ordered_faces, StartPolyhedron);
		
		//la valenza del vertice è pari alla lunghezza del vettore di facce che condividono il vertice dato 
		//ATTENZIONE: Questa parte non è superflua, perché le valenze NON SONO sempre 3 per il generico solido geodetico!!!
		int valence = ordered_faces.size();
		vector<int> New_vertices;
		
		//qui associo a ogni faccia del poliedro di partenza l'id del vertice nel duale corrispondente
		for(const auto& VertexFace: ordered_faces)
			New_vertices.push_back(Faces_bar[VertexFace]);
		DualPolyhedron.Cell2DsId.push_back(face_id);
		DualPolyhedron.Cell2DsVertices[face_id] = New_vertices;
		DualPolyhedron.Cell2DsEdges[face_id].resize(valence);
		
		//Questa è la creazione degli edges, praticamente identica a quella per il solido geodetico, 
		//con la differenza che il vettore di vertici della faccia ha tanti elementi quanti la valenza del 
		//vertice, e a parte questo dettaglio è tutto uguale!
		for (int k = 0; k < valence; k++) {
			int originVertex = DualPolyhedron.Cell2DsVertices[face_id][k];
			int endVertex;
			if ( k == valence-1 )
				endVertex = DualPolyhedron.Cell2DsVertices[face_id][0];
			else
				endVertex = DualPolyhedron.Cell2DsVertices[face_id][k+1];
			if(!CheckDuplicatesEdge(DualPolyhedron.Cell1DsExtrema, originVertex, endVertex, edge_id, duplicate_id)){
				DualPolyhedron.Cell1DsId.push_back(edge_id);
				DualPolyhedron.Cell1DsExtrema(0, edge_id) = originVertex;
				DualPolyhedron.Cell1DsExtrema(1, edge_id) = endVertex;
				DualPolyhedron.Cell2DsEdges[face_id][k] = edge_id;
				edge_id++;
			}
			else
				DualPolyhedron.Cell2DsEdges[face_id][k] = DualPolyhedron.Cell1DsId[duplicate_id];
		}
		face_id++;
	}
	
	//Proietto sulla sfera i punti del duale
	ProjectionOnSphere(DualPolyhedron);
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
                if (found) break;
            }
            if (found) break;
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