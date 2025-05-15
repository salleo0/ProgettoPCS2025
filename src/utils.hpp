#pragma once
#include <sstream>
#include "PolyhedronMesh.hpp"

using namespace std;

bool ImportPolyhedronMesh(PolyhedronMesh& polyhedron, 
						const string& InputFileDirectory);

bool ImportCell0Ds(PolyhedronMesh& polyhedron, 
					const string& InputFile);

bool ImportCell1Ds(PolyhedronMesh& polyhedron, 
					const string& InputFile);

bool ImportCell2Ds(PolyhedronMesh& polyhedron, 
					const string& InputFile);
					
bool GenerateGeodeticSolidType1(const PolyhedronMesh& PlatonicPolyhedron,
							PolyhedronMesh& GeodeticSolid,
							const int& b);
							
bool CheckDuplicatesVertex(const MatrixXd& mat, const Vector3d& vec, int& matSize, int& duplicate_pos);

bool CheckDuplicatesEdge(const MatrixXi& mat, const int& v1, const int& v2, int& matSize);


