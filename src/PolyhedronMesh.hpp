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