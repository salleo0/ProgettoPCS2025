#pragma once

#include <iostream>
#include"Eigen/Eigen"

using namespace std;
using namespace Eigen;

// MESH DI UN POLIEDRO

struct PolyhedronMesh
{
	// Cell0D
	int NumCell0Ds = 0;					// numero di vertici
	vector<int> Cell0DsId = {};			// Id dei vertici
	MatrixXd Cell0DsCoordinates = {};	// coordinate nello spazio dei vertici
	
	// Cell1D
	int NumCell1Ds = 0;				// numero di lati
	vector<int> Cell1DsId = {};		// Id dei lati
	MatrixXi Cell1DsExtrema = {};	// matrice di dimensione 2xNumCell1Ds: alla colonna i-esima 
									// sono memorizzati gli id degli estremi del lato di id i
	
	// Cell2D
	int NumCell2Ds = 0;							// numero di facce
	vector<int> Cell2DsId = {};					// Id delle facce
	vector<int> Cell2DsNumVertices = {};		// l'i-esima entrata memorizza il numero di vertici della faccia di id i
	vector<int> Cell2DsNumEdges = {};			// l'i-esima entrata memorizza il numero di spigoli della faccia di id i
	vector<vector<int>>  Cell2DsVertices = {};	// l'i-esimo vettore memorizza gli id dei vertici della faccia di id i
	vector<vector<int>> Cell2DsEdges = {};		// l'i-esimo vettore memorizza gli id degli spigoli della faccia di id i
};

class PolyhedraMesh 
{
	// Cell0D
	int NumCell0Ds = 0;					// numero di vertici
	vector<int> Cell0DsId = {};			// Id dei vertici
	MatrixXd Cell0DsCoordinates = {};	// coordinate nello spazio dei vertici
	
	// Cell1D
	int NumCell1Ds = 0;				// numero di lati
	vector<int> Cell1DsId = {};		// Id dei lati
	MatrixXi Cell1DsExtrema = {};	// matrice di dimensione 2xNumCell1Ds: alla colonna i-esima 
									// sono memorizzati gli id degli estremi del lato di id i
	
	// Cell2D
	int NumCell2Ds = 0;							// numero di facce
	vector<int> Cell2DsId = {};					// Id delle facce
	vector<int> Cell2DsNumVertices = {};		// l'i-esima entrata memorizza il numero di vertici della faccia di id i
	vector<int> Cell2DsNumEdges = {};			// l'i-esima entrata memorizza il numero di spigoli della faccia di id i
	vector<vector<int>> Cell2DsVertices = {};	// l'i-esimo vettore memorizza gli id dei vertici della faccia di id i
	vector<vector<int>> Cell2DsEdges = {};		// l'i-esimo vettore memorizza gli id degli spigoli della faccia di id i
	
	// Cell3D
	int NumCell3Ds = 0;
	vector<int> Cell3DsId = {};
	vector<int> Cell3DsNumVertices = {};
	vector<int> Cell3DsNumEdges = {};
	vector<int> Cell3DsNumFaces = {};
	vector<vector<int>> Cell3DsVertices = {};
	vector<vector<int>> Cell3DsEdges = {};
	vector<vector<int>> Cell3DsFaces = {};
	
public:
	// metodo che aggiunge i dati riempiti da una struttura di tipo PolyhedronMesh alla struttura PolyhedraMesh
	void append(PolyhedronMesh& Polyhedron) {
		
		// Cell0D
		Cell0DsCoordinates.conservativeResize(3, NumCell0Ds + Polyhedron.NumCell0Ds);
		Cell0DsId.reserve(NumCell0Ds + Polyhedron.NumCell0Ds);
		int id;
		if (NumCell0Ds == 0) 
			id = 0;
		else
			id = Cell0DsId.back() + 1;
		
		for (int i = 0; i < Polyhedron.Cell0DsCoordinates.cols(); i++) {
			Vector3d coord = Polyhedron.Cell0DsCoordinates.col(i);
			bool duplicate = false;
			for (int j = 0; j < Cell0DsCoordinates.cols(); j++) {
				if ((coord - Cell0DsCoordinates.col(j)).norm() < 1e-16) {
					duplicate = true;
					break;
				}
			}
			if (!duplicate) {
				NumCell0Ds++;
				Cell0DsCoordinates.col(id) = coord;
				Cell0DsId.push_back(id);
				id++;
			}
		}
		Cell0DsCoordinates.conservativeResize(3, NumCell0Ds);
		
		// Cell1D
		Cell1DsExtrema.conservativeResize(2, NumCell1Ds + Polyhedron.NumCell1Ds);
		Cell1DsId.reserve(NumCell1Ds + Polyhedron.NumCell1Ds);
		if (NumCell1Ds == 0) 
			id = 0;
		else
			id = Cell1DsId.back() + 1;
		for (int i = 0; i < Polyhedron.Cell1DsId.cols(); i++) {
			Vector2d extrema = Polyhedron.Cell1DsExtrema.col(i);
			bool duplicate = false;
			for (int j = 0; j < Cell1DsExtrema.cols(); j++) {
				int origin = Cell1DsExtrema.col()(0);
				int end = Cell1DsExtrema.col()(1);
				if ( (extrema(0) == origin && extrema(1) == end) || (extrema(1) == origin && extrema(0) == end) ){
					duplicate = true;
					break;
				}
			}
			if (!duplicate) {
				NumCell1Ds++;
				Cell1DsExtrema.col(id) = extrema;
				Cell1DsId.push_back(id);
				id++;
			}
		}
		
		// Cell2D
		
			
					
	