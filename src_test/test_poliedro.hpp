#pragma once

#include <iostream>
#include <gtest/gtest.h>
#include <algorithm>
#include "utils.hpp"

using namespace TriangulationLibrary;

const int TRIANGULATION_PARAMETER = 5;

int VertexDegree(int& ExpectedDegree, const std::vector<int>& Vertices, const std::vector<std::vector<int>>& Faces) {
	int NumVertexOfExpectedDegree = 0;
	for (const auto& idV : Vertices) {
		int CurrentDegree = 0;
		for (const auto& listF : Faces )
			if (std::find(listF.begin(), listF.end(), idV) != listF.end())
				CurrentDegree++;
		if (CurrentDegree == ExpectedDegree)
			NumVertexOfExpectedDegree++;
	}
	
	return NumVertexOfExpectedDegree;
}		

array<int, 3> SolidProperties(const int& V, const int& E, const int& F, const int& TriangulationParameter) {
	int Exp_V = V + E*(2*TriangulationParameter-1) + F * ( (3.0*TriangulationParameter*TriangulationParameter)/2.0 - (3.0*TriangulationParameter/2.0) + 1);
	int Exp_E = E * 2 * TriangulationParameter + F * ( (9.0*TriangulationParameter*TriangulationParameter)/2.0 + (3.0*TriangulationParameter/2.0) );
	int Exp_F = F * ( 3 * TriangulationParameter * TriangulationParameter + 3 * TriangulationParameter);
	array<int, 3> solid_properties = {Exp_V, Exp_E, Exp_F};
	return solid_properties;
}

TEST(TestGeodeticPolyhedron, TestTetrahedronType1)
{
	PolyhedronMesh PlatonicPolyhedron;
	if (!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Tetraedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";

	PolyhedronMesh GeodeticPolyhedron;
	Generation::GeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, TRIANGULATION_PARAMETER);
	
	int T = TRIANGULATION_PARAMETER*TRIANGULATION_PARAMETER;
	int ExpectedVertices = 2*T + 2;
	int ExpectedEdges = 6*T;
	int ExpectedFaces = 4*T;
	
	EXPECT_EQ(GeodeticPolyhedron.NumCell0Ds, ExpectedVertices);
	EXPECT_EQ(GeodeticPolyhedron.NumCell1Ds, ExpectedEdges);
	EXPECT_EQ(GeodeticPolyhedron.NumCell2Ds, ExpectedFaces);
	
	int Degree1 = 3;
	int Degree2 = 6;
	int ExpectedNumDegree1 = 4;
	int ExpectedNumDegree2 = 2*(T-1);
	
	int NumDegree1 = VertexDegree(Degree1, GeodeticPolyhedron.Cell0DsId, GeodeticPolyhedron.Cell2DsVertices);
	int NumDegree2 = VertexDegree(Degree2, GeodeticPolyhedron.Cell0DsId, GeodeticPolyhedron.Cell2DsVertices);
	
	EXPECT_EQ(ExpectedNumDegree1, NumDegree1);
	EXPECT_EQ(ExpectedNumDegree2, NumDegree2);
}

TEST(TestGeodeticPolyhedron, TestOctahedronType1)
{
	PolyhedronMesh PlatonicPolyhedron;
	if (!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Ottaedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";

	PolyhedronMesh GeodeticPolyhedron;
	Generation::GeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, TRIANGULATION_PARAMETER);
	
	int T = TRIANGULATION_PARAMETER*TRIANGULATION_PARAMETER;
	int ExpectedVertices = 4*T + 2;
	int ExpectedEdges = 12*T;
	int ExpectedFaces = 8*T;
	
	EXPECT_EQ(GeodeticPolyhedron.NumCell0Ds, ExpectedVertices);
	EXPECT_EQ(GeodeticPolyhedron.NumCell1Ds, ExpectedEdges);
	EXPECT_EQ(GeodeticPolyhedron.NumCell2Ds, ExpectedFaces);
	
	int Degree1 = 4;
	int Degree2 = 6;
	int ExpectedNumDegree1 = 6;
	int ExpectedNumDegree2 = 4*(T-1);
	
	int NumDegree1 = VertexDegree(Degree1, GeodeticPolyhedron.Cell0DsId, GeodeticPolyhedron.Cell2DsVertices);
	int NumDegree2 = VertexDegree(Degree2, GeodeticPolyhedron.Cell0DsId, GeodeticPolyhedron.Cell2DsVertices);
	
	EXPECT_EQ(ExpectedNumDegree1, NumDegree1);
	EXPECT_EQ(ExpectedNumDegree2, NumDegree2);
}

TEST(TestGeodeticPolyhedron, TestIcosahedronType1)
{
	PolyhedronMesh PlatonicPolyhedron;
	if (!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Icosaedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";

	PolyhedronMesh GeodeticPolyhedron;
	Generation::GeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, TRIANGULATION_PARAMETER);
	
	int T = TRIANGULATION_PARAMETER*TRIANGULATION_PARAMETER;
	int ExpectedVertices = 10*T + 2;
	int ExpectedEdges = 30*T;
	int ExpectedFaces = 20*T;
	
	EXPECT_EQ(GeodeticPolyhedron.NumCell0Ds, ExpectedVertices);
	EXPECT_EQ(GeodeticPolyhedron.NumCell1Ds, ExpectedEdges);
	EXPECT_EQ(GeodeticPolyhedron.NumCell2Ds, ExpectedFaces);
	
	int Degree1 = 5;
	int Degree2 = 6;
	int ExpectedNumDegree1 = 12;
	int ExpectedNumDegree2 = 10*(T-1);
	
	int NumDegree1 = VertexDegree(Degree1, GeodeticPolyhedron.Cell0DsId, GeodeticPolyhedron.Cell2DsVertices);
	int NumDegree2 = VertexDegree(Degree2, GeodeticPolyhedron.Cell0DsId, GeodeticPolyhedron.Cell2DsVertices);
	
	EXPECT_EQ(ExpectedNumDegree1, NumDegree1);
	EXPECT_EQ(ExpectedNumDegree2, NumDegree2);
}

TEST(TestDualPolyhedron, TestType1)
{
	PolyhedronMesh PlatonicPolyhedron;
	if (!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Tetraedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";

	PolyhedronMesh GeodeticPolyhedron;
	Generation::GeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, TRIANGULATION_PARAMETER);
	
	PolyhedronMesh DualPolyhedron;
	Generation::Dual(GeodeticPolyhedron, DualPolyhedron);
	
	int T = TRIANGULATION_PARAMETER*TRIANGULATION_PARAMETER;
	int ExpectedVertices = 4*T;
	int ExpectedFaces = 2*T+2;
	
	EXPECT_EQ(DualPolyhedron.NumCell0Ds, ExpectedVertices);
	EXPECT_EQ(DualPolyhedron.NumCell2Ds, ExpectedFaces);
}

TEST(TestGeodeticPolyhedron, TestTetrahedronType2)
{
	PolyhedronMesh PlatonicPolyhedron;
	if (!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Tetraedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";

	PolyhedronMesh GeodeticPolyhedron;
	Generation::GeodeticSolidType2(PlatonicPolyhedron, GeodeticPolyhedron, TRIANGULATION_PARAMETER);
	
	int V = 4;
	int E = 6;
	int F = 4;
	
	array<int, 3> expected_solid_properties = SolidProperties(V, E, F, TRIANGULATION_PARAMETER);
	array<int, 3> solid_properties = {GeodeticPolyhedron.NumCell0Ds, GeodeticPolyhedron.NumCell1Ds, GeodeticPolyhedron.NumCell2Ds};

	EXPECT_EQ(solid_properties, expected_solid_properties);
}

TEST(TestGeodeticPolyhedron, TestOctahedronType2)
{
	PolyhedronMesh PlatonicPolyhedron;
	if (!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Ottaedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";

	PolyhedronMesh GeodeticPolyhedron;
	Generation::GeodeticSolidType2(PlatonicPolyhedron, GeodeticPolyhedron, TRIANGULATION_PARAMETER);
	
	int V = 6;
	int E = 12;
	int F = 8;
	
	array<int, 3> expected_solid_properties = SolidProperties(V, E, F, TRIANGULATION_PARAMETER);
	array<int, 3> solid_properties = {GeodeticPolyhedron.NumCell0Ds, GeodeticPolyhedron.NumCell1Ds, GeodeticPolyhedron.NumCell2Ds};

	EXPECT_EQ(solid_properties, expected_solid_properties);
}

TEST(TestGeodeticPolyhedron, TestIcosahedronType2)
{
	PolyhedronMesh PlatonicPolyhedron;
	if (!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Icosaedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";

	PolyhedronMesh GeodeticPolyhedron;
	Generation::GeodeticSolidType2(PlatonicPolyhedron, GeodeticPolyhedron, TRIANGULATION_PARAMETER);
	
	int V = 12;
	int E = 30;
	int F = 20;
	
	array<int, 3> expected_solid_properties = SolidProperties(V, E, F, TRIANGULATION_PARAMETER);
	array<int, 3> solid_properties = {GeodeticPolyhedron.NumCell0Ds, GeodeticPolyhedron.NumCell1Ds, GeodeticPolyhedron.NumCell2Ds};

	EXPECT_EQ(solid_properties, expected_solid_properties);
}

TEST(TestDualPolyhedron, TestType2)
{
	PolyhedronMesh PlatonicPolyhedron;
	if (!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Tetraedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";

	PolyhedronMesh GeodeticPolyhedron;
	Generation::GeodeticSolidType2(PlatonicPolyhedron, GeodeticPolyhedron, TRIANGULATION_PARAMETER);
	
	PolyhedronMesh DualPolyhedron;
	Generation::Dual(GeodeticPolyhedron, DualPolyhedron);
	
	int V = 4;
	int E = 6;
	int F = 4;
	
	int ExpectedVertices = F * ( 3 * TRIANGULATION_PARAMETER * TRIANGULATION_PARAMETER + 3 * TRIANGULATION_PARAMETER);
	int ExpectedFaces = V + E*(2*TRIANGULATION_PARAMETER-1) + F * ( (3.0*TRIANGULATION_PARAMETER*TRIANGULATION_PARAMETER)/2.0 - (3.0*TRIANGULATION_PARAMETER/2.0) + 1);
	
	EXPECT_EQ(DualPolyhedron.NumCell0Ds, ExpectedVertices);
	EXPECT_EQ(DualPolyhedron.NumCell2Ds, ExpectedFaces);
}

TEST(TestOrderFaces, Test_unordered)
{
	PolyhedronMesh PlatonicPolyhedron;
	if (!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Ottaedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";
	
	vector<int> unordered_faces = {4, 5, 3, 2};
	vector<int> ordered_faces;
	
	InternalTools::OrderFaces(unordered_faces, ordered_faces, PlatonicPolyhedron);
	vector<int>expected_ordered_faces  = {4, 5, 2, 3};
	
	EXPECT_EQ(ordered_faces, expected_ordered_faces);
}

TEST(TestOrderFaces, Test_ordered)
{
	PolyhedronMesh PlatonicPolyhedron;
	if (!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Icosaedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";
	
	vector<int> unordered_faces = {10, 11, 12, 17, 16};
	vector<int> ordered_faces;
	
	InternalTools::OrderFaces(unordered_faces, ordered_faces, PlatonicPolyhedron);
	vector<int>expected_ordered_faces  = {10, 11, 12, 17, 16};
	
	EXPECT_EQ(ordered_faces, expected_ordered_faces);
}

TEST(TestShortestPath, ShortestPathOnType1)
{
	
	PolyhedronMesh PlatonicPolyhedron;
	if (!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Ottaedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";
	
	PolyhedronMesh GeodeticPolyhedron;
	Generation::GeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, 2);
	
	double path_length;
	int number_edges_in_path;
	vector<int> path_vertices;	
	
	if(!Generation::ShortestPath(GeodeticPolyhedron, 3, 7, path_length, number_edges_in_path, path_vertices))
		FAIL() << "Something went wrong during the execution of ShortestPath function";
	
	vector<int>expected_path = {7,3};
	
	EXPECT_EQ(path_vertices, expected_path);
	EXPECT_EQ(number_edges_in_path, 1);
	EXPECT_NEAR(0.765367, path_length, 1e-6);
}

TEST(TestShortestPath, ShortestPathOnDual)
{
	
	PolyhedronMesh PlatonicPolyhedron;
	PolyhedronMesh DualPolyhedron;
	
	if (!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Tetraedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";
	
	PolyhedronMesh GeodeticPolyhedron;
	
	Generation::GeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, 3);
	Generation::Dual(GeodeticPolyhedron, DualPolyhedron);
	
	double path_length;
	int number_edges_in_path;
	vector<int> path_vertices;	
	
	if(!Generation::ShortestPath(DualPolyhedron, 21, 27, path_length, number_edges_in_path, path_vertices))
		FAIL() << "Something went wrong during the execution of ShortestPath function";
	
	vector<int>expected_path = {27, 25, 26, 21};
	
	EXPECT_EQ(path_vertices, expected_path);
	EXPECT_EQ(number_edges_in_path, 3);
	EXPECT_NEAR(1.316185, path_length,1e-6);
}

TEST(TestShortestPath, ShortestPathOnType2)
{
	
	PolyhedronMesh PlatonicPolyhedron;
	if (!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Icosaedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";
	
	PolyhedronMesh GeodeticPolyhedron;
	Generation::GeodeticSolidType2(PlatonicPolyhedron, GeodeticPolyhedron, 2);
	
	double path_length;
	int number_edges_in_path;
	vector<int> path_vertices;	
	
	if(!Generation::ShortestPath(GeodeticPolyhedron, 146, 144, path_length, number_edges_in_path, path_vertices))
		FAIL() << "Something went wrong during the execution of ShortestPath function";
	
	vector<int>expected_path = {144,145,34,151,146};
	
	EXPECT_EQ(path_vertices, expected_path);
	EXPECT_EQ(number_edges_in_path, 4);
	EXPECT_NEAR(1.451372, path_length, 1e-5);
}