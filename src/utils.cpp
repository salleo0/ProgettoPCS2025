#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include "Eigen/Eigen"
#include"utils.hpp"
#include "UCDUtilities.hpp"

using namespace Eigen;
using namespace std;

namespace TriangulationLibrary {
	namespace Import {
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
	
	}

/************************************/
	namespace Generation {
		void GeodeticSolidType1(const PolyhedronMesh& PlatonicPolyhedron, PolyhedronMesh& GeodeticSolid, const int& num_segments)
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
						if(i > 0){
							
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
								
								if (!CheckDuplicatesEdge(GeodeticSolid.Cell1DsExtrema, originVertex, endVertex, edge_id, duplicate_id)){
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

		
		/*void GeodeticSolidType2(PolyhedronMesh& PlatonicPolyhedron, PolyhedronMesh& GeodeticSolid, int TriangulationParameter)
		{
			PolyhedronMesh tempMesh;
			GeodeticSolidType1(PlatonicPolyhedron, tempMesh, TriangulationParameter);
			
			int T = 3*TriangulationParameter*TriangulationParameter;
			int total_points = 10*T + 2;
			int total_edges = 30*T;
			int total_faces = 20*T;
			
			GeodeticSolid.NumCell0Ds = tempMesh.NumCell0Ds;
			
			GeodeticSolid.Cell0DsId.reserve(total_points);
			GeodeticSolid.Cell0DsCoordinates = MatrixXd::Zero(3, total_points);
			GeodeticSolid.Cell1DsId.reserve(total_edges);
			GeodeticSolid.Cell1DsExtrema = MatrixXi::Zero(2, total_edges);
			
			GeodeticSolid.Cell2DsId.reserve(total_faces);

			// tutti i vertici del type 1 sono anche in comune con il type2, quindi li inseriamo nella matrice delle coordinate conservando anche l'id
			for (const auto& id : tempMesh.Cell0DsId) {
				GeodeticSolid.Cell0DsId.push_back(id);
				GeodeticSolid.Cell0DsCoordinates.col(id) = tempMesh.Cell0DsCoordinates.col(id);
			}
			int point_id = tempMesh.Cell0DsId.back() + 1;
			// generazione dei baricentri
		
			int point_id = GeodeticSolid.NumCell0Ds;
			for (const auto& VertexVector : tempMesh.Cell2DsVertices) {
				Vector3d Vertex1Coord = tempMesh.Cell0DsCoordinates.col(VertexVector[0]);
				Vector3d Vertex2Coord = tempMesh.Cell0DsCoordinates.col(VertexVector[1]);
				Vector3d Vertex3Coord = tempMesh.Cell0DsCoordinates.col(VertexVector[2]);
				
				Vector3d MidpointCoord = (Vertex1Coord + Vertex2Coord + Vertex3Coord)/3;
				GeodeticSolid.Cell0DsId.push_back(point_id);
				GeodeticSolid.Cell0DsCoordinates.col(point_id) = MidpointCoord/(MidpointCoord.norm());
				GeodeticSolid.NumCell0Ds++;
				point_id++;
				cout << point_id << endl;
			}
			
			// ATTENZIONE: IL PROBLEMA è ALLA FINE DELLA GENERAZIONE DEI BARICENTRI, ci potrebbero essere più punti del previsto
			cout << "ok" << endl;

			for (int j = 0; j < PlatonicPolyhedron.Cell1DsExtrema.cols(); j++) {
				const auto& VertexIdVector = PlatonicPolyhedron.Cell1DsExtrema.col(j);
				Vector3d Vertex1Coord = PlatonicPolyhedron.Cell0DsCoordinates.col(VertexIdVector[0]);
				Vector3d Vertex2Coord = PlatonicPolyhedron.Cell0DsCoordinates.col(VertexIdVector[1]);
				Vector3d Direction = Vertex2Coord - Vertex1Coord;
				double factor = (Direction.norm())/(2*TriangulationParameter);
				for (int i = 1; i <= 2*TriangulationParameter - 1; i++) {
					Vector3d PointOnSegmentCoord = Vertex1Coord + i*factor*Direction;
					PointOnSegmentCoord = PointOnSegmentCoord/(PointOnSegmentCoord.norm());
					int tmp;
					if (!CheckDuplicatesVertex(GeodeticSolid.Cell0DsCoordinates, PointOnSegmentCoord, GeodeticSolid.NumCell0Ds, tmp)) {
						GeodeticSolid.Cell0DsId.push_back(point_id);
						GeodeticSolid.Cell0DsCoordinates.col(point_id) = PointOnSegmentCoord;
						point_id++;
						GeodeticSolid.NumCell0Ds++;
					}
				}
			}
			
			for(const auto& Id: GeodeticSolid.Cell0DsId){
				cout<<Id<<endl;
				cout<<GeodeticSolid.Cell0DsCoordinates(0,Id)<<" "<<GeodeticSolid.Cell0DsCoordinates(1,Id)<<GeodeticSolid.Cell0DsCoordinates(2,Id)<<endl;
			}
			
			
		}*/
		
/*******************************************************/

		
		void GeodeticSolidType2(PolyhedronMesh& PlatonicPolyhedron, PolyhedronMesh& GeodeticSolid, int num_segments)

			int points_id = 0;		// id dei vertici del poliedro geodetico che andremo a generare
			int duplicate_id = 0;	// id che servirà per controllare se esiste un duplicato
			int edge_id = 0;		// id degli spigoli del poliedro geodetico che andremo a generare
			int face_id = 0;		// id delle facce del poliedro geodetico che andremo a generare
			int T = 3*num_segments*num_segments;
			int total_points = 10*T + 2;
			int total_edges = 30*T;
			int total_faces = 20*T;
															
			
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

			GeodeticSolid.Cell0DsCoordinates.conservativeResize(3, GeodeticSolid.NumCell0Ds);
			
			//fino a qui ho in GeodeticSolid.Cell0DsCoordinates una matrice contenente le coordinate dei punti dei triangoli, e il vettore dei loro id
			
			
			// assicurati che points_id parta da dove era rimasto nel ciclo precedente!!!!!!
			
			
			// GENERAZIONE SPIGOLI E FACCE E ALTRI PUNTI

			for  (const auto& id : PlatonicPolyhedron.Cell2DsId){
				for (int i = 0; i < num_segments; i++){
					for (int j = 0; j < num_segments - i; j++){
						
						// questa antiestetica sezione definisce gli interi che diventeranno gli identificatvi dei punti e degli edges che formeranno la mesh di tipo 2
						// per tutte le facce del poliedro platonico
						// point 1 è il punto tra vertice 1 e vertice 2
						// point 2 è il punto tra vertice 2 e vertice 3
						// point 3 è il punto tra vertice 3 e vertice 1
						
						unsigned int baricentro_id;
						unsigned int Vertex1 = point_coefficients[{i,num_segments-i-j,j,id}];
						unsigned int Vertex2 = point_coefficients[{i,num_segments-i-(j+1),j+1,id}];
						unsigned int Vertex3 = point_coefficients[{i+1,num_segments-(i+1)-j,j,id}];
						
						unsigned int point1_id;
						unsigned int point2_id;
						unsigned int point3_id;
						
						unsigned int edgeV1P1_id;
						unsigned int edgeV1B_id;
						unsigned int edgeV1P3_id;
						
						unsigned int edgeV2P1_id;
						unsigned int edgeV2B_id;
						unsigned int edgeV2P2_id;
						
						unsigned int edgeV3P2_id;
						unsigned int edgeV3P3_id;
						unsigned int edgeV3B_id;
						
						unsigned int edgeP1B_id;
						unsigned int edgeP2B_id;
						unsigned int edgeP3B_id;
						
											
						Vector3d Baricentro = (GeodeticSolid.Cell0DsCoordinates.col(Vertex1) + GeodeticSolid.Cell0DsCoordinates.col(Vertex2) + GeodeticSolid.Cell0DsCoordinates.col(Vertex3))/3;
						
						GeodeticSolid.NumCell0Ds++;
						GeodeticSolid.Cell0DsCoordinates.col(points_id) = Baricentro;
						baricentro_id = points_id;
						points_id++;
						
						Vector3d point1 = (GeodeticSolid.Cell0DsCoordinates.col(Vertex1) + GeodeticSolid.Cell0DsCoordinates.col(Vertex2))/2;
						Vector3d point2 = (GeodeticSolid.Cell0DsCoordinates.col(Vertex2) + GeodeticSolid.Cell0DsCoordinates.col(Vertex3))/2;
						Vector3d point3 = (GeodeticSolid.Cell0DsCoordinates.col(Vertex3) + GeodeticSolid.Cell0DsCoordinates.col(Vertex1))/2;
						
						
						// Ragionando a facce con la punta in su
						
						// quando siamo nel bordo inferiore della faccia del Platonico devo fare le giuste osservazioni, 
						// i punti di bordo possonbo essere gia stati creati, e in tal caso con checkduplicatevertex mi salvo l'id che già è stato associato
						// e provvedo (nella costruzione degli edges che inbcludono tal punto) a utilizzare il corretto id.
						// Stesso discordo per gli edges che uniscono i punti di bordo ai vertici: possono essere gia stati creati, dunque nella costruzione
						// delle facce che hanno questi edges devo identificare gli edges coll corretto id.
						// I tre if che seguono fanno queste verifiche. 
						
						
						if(i==0){
							
							if(!CheckDuplicatesVertex(GeodeticSolid.Cell0DsCoordinates, point1, GeodeticSolid.NumCell0Ds, duplicate_id)){
								GeodeticSolid.NumCell0Ds++;
								GeodeticSolid.Cell0DsCoordinates.col(points_id) = point1;
								point1_id = point_id;
								points_id++;
							}
							else {
								point1_id = duplicate_id;
							}
							
							// edge tra point 1 e baricentro
							edgeP1B_id = edge_id;
							GeodeticSolid.Cell1DsExtrema(0,edge_id) = point1_id;
							GeodeticSolid.Cell1DsExtrema(1,edge_id) = baricentro_id;
							GeodeticSolid.Cell1DsId.push_back(edgeP1B_id);
							edge_id++;
							GeodeticSolid.NumCell1Ds++;
							
							// edge tra point1 e vertex1: se questo edge non è stato fatto prima lo costruisco,
							// se fosse stato gia creato, salvo il suo id.
							if(!CheckDuplicatesEdge(GeodeticSolid.Cell1DsExtrema, Vertex1, point1_id, GeodeticSolid.NumCell1Ds, duplicate_id) {
								edgeV1P1_id = edge_id;
								GeodeticSolid.Cell1DsId.push_back(edgeV1P1_id);
								GeodeticSolid.Cell1DsExtrema(0, edgeV1P1_id) = Vertex1;
								GeodeticSolid.Cell1DsExtrema(1, edgeV1P1_id) = point1_id;
								
								GeodeticSolid.Cell1DsId.push_back(edgeV1P1_id);
								edge_id++;
								GeodeticSolid.NumCell1Ds++;
							}
							else {
								edgeV1P1_id = duplicate_id;
							}
							
							// edge tra point1 e vertex2: se questo edge non è stato fatto prima lo costruisco,
							// se fosse stato gia creato, salvo il suo id.
							if(!CheckDuplicatesEdge(GeodeticSolid.Cell1DsExtrema, Vertex2, point1_id, GeodeticSolid.NumCell1Ds, duplicate_id) {
								edgeV2P1_id = edge_id;
								GeodeticSolid.Cell1DsId.push_back(edgeV2P1_id);
								GeodeticSolid.Cell1DsExtrema(0, edgeV2P1_id) = Vertex2;
								GeodeticSolid.Cell1DsExtrema(1, edgeV2P1_id) = point1_id;
								
								GeodeticSolid.Cell1DsId.push_back(edgeV2P1_id);
								edge_id++;
								GeodeticSolid.NumCell1Ds++;
							}
							else {
								edgeV2P1_id = duplicate_id;
							}
						}
						
						// giunto a questo punto per i triangoli della mesh 1 che stanno sul bordo inferiore ho creato edgeP1B edgeV2P1 e edgeV1P1.
						
						
						
						
						// ora mi concentro sul bordo sinistro della faccai del poliedro platonico. I punti a cui devo stare attento riguardo i doppioni sono
						// i point3, e gli edges edgeV1P3 e edgeV3P3
						
						if(j==0){
							if(!CheckDuplicatesVertex(GeodeticSolid.Cell0DsCoordinates, point3, GeodeticSolid.NumCell0Ds, duplicate_id)){
								GeodeticSolid.NumCell0Ds++;
								GeodeticSolid.Cell0DsCoordinates.col(points_id) = point3;
								point3_id = points_id;
								points_id++;	
							}
							else {
								point3_id = duplicate_id;
							}
							
							// edge tra point 3 e baricentro
							GeodeticSolid.Cell1DsExtrema(0,edge_id) = point3_id;
							GeodeticSolid.Cell1DsExtrema(1,edge_id) = baricentro_id;
							edgeP3B_id = edge_id;
							GeodeticSolid.Cell1DsId.push_back(edgeP3B_id);
							edge_id++;
							GeodeticSolid.NumCell1Ds++;
							
							
							// edge tra point3 e vertex1: se questo edge non è stato fatto prima lo costruisco,
							// se fosse stato gia creato, salvo il suo id.
							
							if(!CheckDuplicatesEdge(GeodeticSolid.Cell1DsExtrema, Vertex1, point3_id, GeodeticSolid.NumCell1Ds, duplicate_id) {
								edgeV1P3_id = edge_id;
								GeodeticSolid.Cell1DsId.push_back(edgeV1P3_id);
								GeodeticSolid.Cell1DsExtrema(0, edgeV1P3_id) = Vertex1;
								GeodeticSolid.Cell1DsExtrema(1, edgeV1P3_id) = point3_id;
								
								GeodeticSolid.Cell1DsId.push_back(edgeV1P3_id);
								edge_id++;
								GeodeticSolid.NumCell1Ds++;
							}
							else {
								edgeV1P3 = duplicate_id;
							}
							
							// edge tra point3 e vertex3: se questo edge non è stato fatto prima lo costruisco,
							// se fosse stato gia creato, salvo il suo id.
							if(!CheckDuplicatesEdge(GeodeticSolid.Cell1DsExtrema, Vertex3, point3_id, GeodeticSolid.NumCell1Ds, duplicate_id) {
								edgeV3P3_id = edge_id;
								GeodeticSolid.Cell1DsId.push_back(edgeV3P3_id);
								GeodeticSolid.Cell1DsExtrema(0, edgeV3P3_id) = Vertex3;
								GeodeticSolid.Cell1DsExtrema(1, edgeV3P3_id) = point3_id;
								
								GeodeticSolid.Cell1DsId.push_back(edgeV3P3_id);
								edge_id++;
								GeodeticSolid.NumCell1Ds++;
							}
							else {
								edgeV3P3_id = duplicate_id;
							}
						}
							
						// giunti qui per i triangoli della mesh 1 che stanno a contatto con il bordo sinistro abbiamo creato edgeP3B, edgeV1P3 ed edgeV3P3 	
							
						
						
						
						
						

							 
						if(j == num_segments - i - 1){
							if(!CheckDuplicatesVertex(GeodeticSolid.Cell0DsCoordinates, point2, GeodeticSolid.NumCell0Ds, duplicate_id)){
								GeodeticSolid.NumCell0Ds++;
								GeodeticSolid.Cell0DsCoordinates.col(points_id) = point2;
								point2_id = points_id;
								points_id++;
							}
							else {
								point2_id = duplicate_id;
							}
							
							// edge tra point 2 e baricentro
							GeodeticSolid.Cell1DsExtrema(0,edge_id) = point2_id;
							GeodeticSolid.Cell1DsExtrema(1,edge_id) = baricentro_id;
							edgeP2B_id = edge_id;
							GeodeticSolid.Cell1DsId.push_back(edgeP2B_id);
							edge_id++;
							GeodeticSolid.NumCell1Ds++;
							
							
							// edge tra point 2 e vertex2: se questo edge non è stato già fatto lo costruisco,
							// altrimenti salvo il suo id.
							
							if(!CheckDuplicatesEdge(GeodeticSolid.Cell1DsExtrema, Vertex2, point2_id, GeodeticSolid.NumCell1Ds, duplicate_id) {
								edgeV2P2_id = edge_id;
								GeodeticSolid.Cell1DsId.push_back(edgeV2P2_id);
								GeodeticSolid.Cell1DsExtrema(0, edgeV2P2_id) = Vertex2;
								GeodeticSolid.Cell1DsExtrema(1, edgeV2P2_id) = point2_id;
								
								GeodeticSolid.Cell1DsId.push_back(edgeV2P2_id);
								edge_id++;
								GeodeticSolid.NumCell1Ds++;
							}
							else {
								edgeV2P2_id = duplicate_id;
							}
							
							// edge tra point2 e vertex3: se questo edge non è stato fatto prima lo costruisco,
							// se fosse stato gia creato, salvo il suo id.
							if(!CheckDuplicatesEdge(GeodeticSolid.Cell1DsExtrema, Vertex3, point2_id, GeodeticSolid.NumCell1Ds, duplicate_id) {
								edgeV3P2_id = edge_id;
								GeodeticSolid.Cell1DsId.push_back(edgeV3P2_id);
								GeodeticSolid.Cell1DsExtrema(0, edgeV3P2_id) = Vertex1;
								GeodeticSolid.Cell1DsExtrema(1, edgeV3P2_id) = point2_id;
								
								GeodeticSolid.Cell1DsId.push_back(edgeV3P2_id);
								edge_id++;
								GeodeticSolid.NumCell1Ds++;
							}
							else {
								edgeV3P2_id = duplicate_id;
							}

						}
						
						// ho finalmente trattato tutti i triangoli della mesh che stanno a contatto con gli spigoli del platonico. eviotando i doppioni
						
						
						// Leo guardalo fino a qui, da qui in poi è da fare!!!!!!!!!!!!!!!!!!!!!!!!!
						
						// adesso mancano tre cose: 
						// 1) da collegare i vertici con il baricentro, manovra che deve essere fatta per TUTTI i triangoli della mesh1, anche quelli a testa in giù
						// 2) collegare tra di loro i baricentri dei triangoli adiacenti.
						// 3) fare tutte le facce!!
						
						
						
						GeodeticSolid.NumCell1Ds++;
						GeodeticSolid.Cell1DsId.push_back(edge_id);
						GeodeticSolid.Cell1DsExtrema(0,edge_id) = vertex1;
						GeodeticSolid.Cell1DsExtrema(1,edge_id) = baricentro_id;
						edge_id++;
						
						GeodeticSolid.NumCell1Ds++;
						GeodeticSolid.Cell1DsId.push_back(edge_id);
						GeodeticSolid.Cell1DsExtrema(0,edge_id) = vertex2;
						GeodeticSolid.Cell1DsExtrema(1,edge_id) = baricentro_id;
						edge_id++;
						
						GeodeticSolid.NumCell1Ds++;
						GeodeticSolid.Cell1DsId.push_back(edge_id);
						GeodeticSolid.Cell1DsExtrema(0,edge_id) = vertex3;
						GeodeticSolid.Cell1DsExtrema(1,edge_id) = baricentro_id;
						edge_id++;
						
						
						
						
						
						
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
						if(i > 0){
							
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
								
								if (!CheckDuplicatesEdge(GeodeticSolid.Cell1DsExtrema, originVertex, endVertex, edge_id, duplicate_id)){
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
			

		
		/************************************/

		void Dual(PolyhedronMesh& StartPolyhedron, PolyhedronMesh& DualPolyhedron)
		{
			int baricenter_id = 0;
			int face_id = 0;
			int edge_id = 0;
			
			//Il Duale ha un numero di facce uguale al numero di vertici del poliedro di partenza
			DualPolyhedron.NumCell0Ds = StartPolyhedron.NumCell2Ds;
			//DualPolyhedron.Cell0DsId.resize(DualPolyhedron.NumCell0Ds);
			//DualPolyhedron.Cell0DsCoordinates = MatrixXd::Zero(3,DualPolyhedron.NumCell0Ds);
			
			//Il Duale ha lo stesso numero di edges del poliedro di partenza, grazie alla formula di Eulero, qualunque sia la varietà su cui si fa la mesh :)
			DualPolyhedron.NumCell1Ds = StartPolyhedron.NumCell1Ds;
			
			//Il Duale ha un numero di facce uguale al numero di vertici del poliedro di partenza
			DualPolyhedron.NumCell2Ds = StartPolyhedron.NumCell0Ds;
			DualPolyhedron.Cell0DsId.reserve(DualPolyhedron.NumCell0Ds);
			DualPolyhedron.Cell0DsCoordinates = MatrixXd::Zero(3,DualPolyhedron.NumCell0Ds);	
			
			DualPolyhedron.Cell1DsExtrema = MatrixXi::Zero(2, DualPolyhedron.NumCell1Ds);
			DualPolyhedron.Cell1DsId.reserve(DualPolyhedron.NumCell1Ds);
			
			DualPolyhedron.Cell2DsId.reserve(DualPolyhedron.NumCell2Ds);
			DualPolyhedron.Cell2DsEdges.resize(DualPolyhedron.NumCell2Ds);
			DualPolyhedron.Cell2DsNumEdges.resize(DualPolyhedron.NumCell2Ds);
			DualPolyhedron.Cell2DsVertices.resize(DualPolyhedron.NumCell2Ds);
			DualPolyhedron.Cell2DsNumVertices.resize(DualPolyhedron.NumCell2Ds);
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
					for(int j = 0; j < 3; j++){
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
				//ovvero quella che ha l'edge in comune con la faccia corrente, per ordinare questo vettore chiamo la funzione OrderFaces
				//il vettore ordered_Faces è passato per riferimento, in modo che venga aggiornato dalla funzione order_Faces.
				vector<int> ordered_faces;
				OrderFaces(VertexFaces, ordered_faces, StartPolyhedron);
				
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
				
				DualPolyhedron.Cell2DsNumVertices[face_id] = valence;
				DualPolyhedron.Cell2DsNumEdges[face_id] = valence;
				
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

			ProjectionOnSphere(DualPolyhedron);
			
			DualPolyhedron.Cell1DsExtrema.conservativeResize(2, DualPolyhedron.NumCell1Ds);
			DualPolyhedron.Cell2DsNumVertices.resize(DualPolyhedron.NumCell2Ds);
			DualPolyhedron.Cell2DsNumEdges.resize(DualPolyhedron.NumCell2Ds);
			DualPolyhedron.Cell2DsVertices.resize(DualPolyhedron.NumCell2Ds);
			DualPolyhedron.Cell2DsEdges.resize(DualPolyhedron.NumCell2Ds);
			
			// GENERAZIONE POLIEDRO
			DualPolyhedron.NumCell3Ds++;
			DualPolyhedron.Cell3DsId.push_back(0);
			DualPolyhedron.Cell3DsNumVertices.push_back(DualPolyhedron.NumCell0Ds);
			DualPolyhedron.Cell3DsNumEdges.push_back(DualPolyhedron.NumCell1Ds);
			DualPolyhedron.Cell3DsNumFaces.push_back(DualPolyhedron.NumCell2Ds);
			DualPolyhedron.Cell3DsVertices.push_back(DualPolyhedron.Cell0DsId);
			DualPolyhedron.Cell3DsEdges.push_back(DualPolyhedron.Cell1DsId);
			DualPolyhedron.Cell3DsFaces.push_back(DualPolyhedron.Cell2DsId);
		}
		
	}

	/************************************/

	void OrderFaces(const vector<int>& unordered_faces, vector<int>& ordered_faces, const PolyhedronMesh& Polyhedron)
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

	void ShortestPath(PolyhedronMesh& Polyhedron, int StartVertex, int EndVertex)
	{	
		// generazione della lista di adiacenza, poiché è tutto indicizzato sequenzialmente, 
		// conviene usare un vector di vector anziché un vector di liste
		vector<vector<int>> adjacency_list;
		adjacency_list.reserve(Polyhedron.NumCell0Ds);
		MatrixXd W = MatrixXd::Zero(Polyhedron.NumCell0Ds, Polyhedron.NumCell0Ds);
		
		
		// iterando su tutti gli id dei punti, 
		// per ciascun punto itero in tutti gli id dei segmenti (origin,end) e guardo quali hanno per estremo quel punto.
		// L'estremo che è diverso dal punto in questione lo appendo alla "lista" di adiacenza.
		
		for(int i = 0; i < Polyhedron.NumCell0Ds; i++){
			vector<int> neighbors;
			for(const auto& edge : Polyhedron.Cell1DsId){
				int Origin = Polyhedron.Cell1DsExtrema(0,edge);
				int End = Polyhedron.Cell1DsExtrema(1,edge);
				if (Origin == i)
					neighbors.push_back(End);
				else if(End == i)
					neighbors.push_back(Origin);
			}
			adjacency_list.push_back(neighbors);
		}
		
		for(int i = 0; i<adjacency_list.size();i++){
			for(const auto& neighbor: adjacency_list[i]){
				W(i,neighbor) = (Polyhedron.Cell0DsCoordinates.col(neighbor)-Polyhedron.Cell0DsCoordinates.col(i)).norm();
			}
		}
		
		// algoritmo di Dijkstra per esplorare il grafo, pred è un vettore
		// ausiliario usato per ricostruire il percorso

		vector<int> pred(Polyhedron.NumCell0Ds, -1);
		vector<double> dist(Polyhedron.NumCell0Ds, 1000.0);
		priority_queue<pair<int, double>, vector<pair<int, double>>, greater<pair<int, double>>> PQ;
		
		pred[StartVertex] = StartVertex;
		dist[StartVertex] = 0;
		
		for(int i = 0; i < Polyhedron.NumCell0Ds; i++)
			PQ.push({i, dist[i]});
		while(!PQ.empty()){
			int u = PQ.top().first;
			int p = PQ.top().second;
			PQ.pop();
			for(const auto& w : adjacency_list[u]){
				if( dist[w] > dist[u] + W(u,w) ) {
					dist[w] = dist[u] + W(u,w);
					pred[w] = u;
					PQ.push({w, dist[w]});
				}
			}
		}
		
		// path contiene gli id dei vertici che compongono il cammino minimo 
		// al contrario, perché sono id provenienti dal vettore pred
		
		vector<int> path;
		int v = EndVertex;
		while(v != StartVertex) {
			path.push_back(v);
			v = pred[v];
		} 
		path.push_back(StartVertex);
		
		vector<double> PathPointsProperties(Polyhedron.NumCell0Ds, 0.0);
		for (const auto& point : path)
			PathPointsProperties[point] = 1.0;


		Gedim::UCDProperty<double> ShortPathProperty;
		ShortPathProperty.Label = "shortest path";
		ShortPathProperty.UnitLabel = "";
		ShortPathProperty.Size = PathPointsProperties.size();
		ShortPathProperty.NumComponents = 1;
		ShortPathProperty.Data = PathPointsProperties.data();  


		vector<Gedim::UCDProperty<double>> PointsProperties;
		PointsProperties.push_back(ShortPathProperty);
	
		Gedim::UCDUtilities utilities;
		utilities.ExportPoints("./Cell0DsShortPath.inp",
								Polyhedron.Cell0DsCoordinates,
								PointsProperties);
	
		vector<int> pathEdges; 
		vector<double> PathEdgesProperties(Polyhedron.NumCell1Ds, 0.0);

		for (int i = 0; i < path.size()-1; i++){
			int v1 = path[i];
			int v2 = path[i+1];
			for(const auto& edge: Polyhedron.Cell1DsId){
				if ((Polyhedron.Cell1DsExtrema(0, edge) == v1 && Polyhedron.Cell1DsExtrema(1, edge) == v2) || 
					(Polyhedron.Cell1DsExtrema(0, edge) == v2 && Polyhedron.Cell1DsExtrema(1,edge) == v1)){
						pathEdges.push_back(edge);
						PathEdgesProperties[edge] = 1.0;
					}
			}	
		}
		
		double Totdist = 0.0;
		for(int i = 0; i<path.size()-1;i++)
			Totdist += W(path[i],path[i+1]);
		
		cout<<"La lunghezza totale del cammino minimo è: "<<Totdist<<", il numero di nodi è "<<pathEdges.size()<<endl;
		
		ShortPathProperty.Label = "shortest path";
		ShortPathProperty.UnitLabel = "";
		ShortPathProperty.Size = PathEdgesProperties.size();
		ShortPathProperty.NumComponents = 1;
		ShortPathProperty.Data = PathEdgesProperties.data();  
	
		vector<Gedim::UCDProperty<double>> EdgesProperties;
		EdgesProperties.push_back(ShortPathProperty);
		utilities.ExportSegments("./Cell1DsShortPath.inp",
								Polyhedron.Cell0DsCoordinates,
								Polyhedron.Cell1DsExtrema,
								PointsProperties,
								EdgesProperties);
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

	void ProjectionOnSphere(PolyhedronMesh& mesh) 
	{
		for(int i = 0; i < mesh.NumCell0Ds; i++){
			double Norm_factor = (mesh.Cell0DsCoordinates.col(i)).norm();
			mesh.Cell0DsCoordinates(0,i) = mesh.Cell0DsCoordinates(0,i)/Norm_factor;
			mesh.Cell0DsCoordinates(1,i) = mesh.Cell0DsCoordinates(1,i)/Norm_factor;
			mesh.Cell0DsCoordinates(2,i) = mesh.Cell0DsCoordinates(2,i)/Norm_factor;
		}
	}

	/************************************/

	bool ExportOutputFiles(const PolyhedronMesh& mesh)
	{
		{
			ofstream fileout("Cell0Ds.txt");
			if (!fileout.is_open())
				return false;

			fileout << "ID;X;Y;Z;" << endl;
			
			for (int i = 0; i < mesh.NumCell0Ds; i++)
				fileout << mesh.Cell0DsId[i] << ";"
					<< mesh.Cell0DsCoordinates(0,i) << ";"
					<< mesh.Cell0DsCoordinates(1,i) << ";"
					<< mesh.Cell0DsCoordinates(2,i) << endl;
		}

		{
			ofstream fileout("Cell1Ds.txt");
			if (!fileout.is_open())
				return false;
			
			fileout << "ID;Origin;End" << endl;
			
			for (int i = 0; i < mesh.NumCell1Ds; i++)
				fileout << mesh.Cell1DsId[i] << ";"
					<< mesh.Cell1DsExtrema(0,i) << ";"
					<< mesh.Cell1DsExtrema(1,i) << endl;
		}
		
		{
			ofstream fileout("Cell2Ds.txt");
			if (!fileout.is_open())
				return false;

			fileout << "ID;NumVertices;VerticesID;NumEdges;EdgesID" << endl;
			
			for (int i = 0; i < mesh.NumCell2Ds; i++) {
				fileout << mesh.Cell2DsId[i] << ";"
					<< mesh.Cell2DsNumVertices[i];
				
				for(int j = 0; j < mesh.Cell2DsNumVertices[i]; j++)
					fileout << ";" << mesh.Cell2DsVertices[i][j];
				
				fileout << ";" << mesh.Cell2DsNumEdges[i];
				
				for(int j = 0; j < mesh.Cell2DsNumEdges[i]; j++)
					fileout << ";" << mesh.Cell2DsEdges[i][j];
				
				fileout << endl;
			}
		}
		
		{
			ofstream fileout("Cell3Ds.txt");
			if (!fileout.is_open())
				return false;
			
			fileout << "ID;NumVertices;VerticesID;NumEdges;EdgesID;NumFaces;FacesID" << endl;
			
			for (int i = 0; i < mesh.NumCell3Ds; i++) {
				fileout << mesh.Cell3DsId[i] << ";"
					<< mesh.Cell3DsNumVertices[i];
				
				for (int j = 0; j < mesh.Cell3DsNumVertices[i]; j++)
					fileout << ";" << mesh.Cell3DsVertices[i][j];
				
				fileout << ";" << mesh.Cell3DsNumEdges[i];
				
				for (int j = 0; j < mesh.Cell3DsNumEdges[i]; j++)
					fileout << ";" << mesh.Cell3DsEdges[i][j];
				
				fileout << ";" << mesh.Cell3DsNumFaces[i];
				
				for (int j = 0; j < mesh.Cell3DsNumFaces[i]; j++)
					fileout << ";" << mesh.Cell3DsFaces[i][j];
				
				fileout << endl;
			}
		}
		return true;
	}

}
