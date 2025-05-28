#pragma once

#include <iostream>
#include"Eigen/Eigen"

using namespace std;
using namespace Eigen;

// MESH DI UN POLIEDRO

namespace TriangulationLibrary {
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
		
		// Cell3D
		int NumCell3Ds = 0;
		vector<int> Cell3DsId = {};					
		vector<int> Cell3DsNumVertices = {};		
		vector<int> Cell3DsNumEdges = {};	
		vector<int> Cell3DsNumFaces = {};
		vector<vector<int>> Cell3DsVertices = {};	
		vector<vector<int>> Cell3DsEdges = {};
		vector<vector<int>> Cell3DsFaces = {};
		
		PolyhedronMesh();
		
		PolyhedronMesh(const PolyhedronMesh& other) {
			NumCell0Ds = other.NumCell0Ds;
			Cell0DsId = other.Cell0DsId;
			Cell0DsCoordinates = other.Cell0DsCoordinates;
			
			NumCell1Ds = other.NumCell1Ds;
			Cell1DsId = other.Cell1DsId;
			Cell1DsExtrema = other.Cell1DsExtrema;
			
			NumCell2Ds = other.NumCell2Ds;
			Cell2DsId = other.Cell2DsId;
			Cell2DsNumVertices = other.Cell2DsNumVertices;
			Cell2DsNumEdges = other.Cell2DsNumEdges;
			Cell2DsVertices = other.Cell2DsVertices;
			Cell2DsEdges = other.Cell2DsEdges;
			
			NumCell3Ds = other.NumCell3Ds;
			Cell3DsId = other.Cell3DsId;					
			Cell3DsNumVertices = other.Cell3DsNumVertices;		
			Cell3DsNumEdges = other.Cell3DsNumEdges;	
			Cell3DsNumFaces = other.Cell3DsNumFaces;
			Cell3DsVertices = other.Cell3DsVertices;	
			Cell3DsEdges = other.Cell3DsEdges;
			Cell3DsFaces = other.Cell3DsFaces;
		}			
	};	

}
