#pragma once
#include <sstream>
#include "PolyhedronMesh.hpp"

using namespace std;



// Importa il solido platonico di partenza e testa che sia importato correttamente
// polyhedron: una struct di tipo PolyhedronMesh
// InputFileDirectory: la directory in cui sono salvati i file .csv con i dati del solido
// ritorna il risultato della lettura: vero se l'importazione del solido è andata a buon fine, falso altrimenti
bool ImportPolyhedronMesh(PolyhedronMesh& polyhedron, const string& InputFileDirectory);

// Importa le proprietà Cell0D dal file Cell0Ds.csv
// polyhedron: una struct di tipo PolyhedronMesh
// InputFile: nome del file Cell0Ds.csv in formato stringa
// ritorna il risultato della lettura: vero se l'importazione di Cell0Ds.csv è andata a buon fine, falso altrimenti
bool ImportCell0Ds(PolyhedronMesh& polyhedron, const string& InputFile);

// Importa le proprietà Cell1D dal file Cell1Ds.csv
// polyhedron: una struct di tipo PolyhedronMesh
// InputFile: nome del file Cell1Ds.csv in formato stringa
// ritorna il risultato della lettura: vero se l'importazione di Cell1Ds.csv è andata a buon fine, falso altrimenti
bool ImportCell1Ds(PolyhedronMesh& polyhedron, const string& InputFile);

// Importa le proprietà Cell2D dal file Cell2Ds.csv
// polyhedron: una struct di tipo PolyhedronMesh
// InputFile: nome del file Cell2Ds.csv in formato stringa
// ritorna il risultato della lettura: vero se l'importazione di Cell2Ds è andata a buon fine, falso altrimenti
bool ImportCell2Ds(PolyhedronMesh& polyhedron, const string& InputFile);

// Genera il poliedro geodetico di tipo 1 da un solido platonico secondo opportuni parametri
// PlatonicPolyhedron: una struct del tipo PolyhedronMesh che contiene le proprietà del solido platonico
// GeodeticSolid: una struct del tipo PolyhedronMesh su cui vengono salvate le proprietà del poliedro geodetico
// num_segments: parametro della triangolazione che indica in quanti segmenti dividere uno spigolo del solido platonico			
// ritorna il risultato della generazione della triangolazione, vero se è andato a buon fine, falso altrimenti
void GenerateGeodeticSolidType1(const PolyhedronMesh& PlatonicPolyhedron, PolyhedronMesh& GeodeticSolid, const int& num_segments);

// Controlla se ci sono dei duplicati di un vettore di coordinate
// mat: matrice di dim 3xmatSize in cui sono salvate le coordinate
// vec: vettore di cui controllare se ci sono duplicati
// matSize: numero di colonne della matrice mat
// duplicate_id: posizione di un eventuale duplicato del vettore vec
// ritorna vero se un duplicato è stato trovato, falso altrimenti
bool CheckDuplicatesVertex(const MatrixXd& mat, const Vector3d& vec, int& matSize, int& duplicate_id);

// Controlla se ci sono dei duplicati di un segmento che connette due vertici v1 e v2
// mat: matrice di dimensione 2xmatSize in cui sono salvati i vertici agli estremi di un segmento 
// v1 e v2: id di vertici
// matSize: numero di colonne della matrice mat
// duplicate_id: posizione di un eventuale duplicato dell'edge di estremi v1 e v2
// ritorna vero se un duplicato è stato trovato, falso altrimenti
bool CheckDuplicatesEdge(const MatrixXi& mat, const int& v1, const int& v2, int& matSize, int& duplicate_id);

// Proietta sulla sfera di raggio 1 e centro l'origine le coordinate dei vertici della mesh
// mesh: struct del tipo PolyhedronMesh
void ProjectionOnSphere(PolyhedronMesh& mesh);

//Costruisce il poliedro duale DualPolyhedron a partire dal generico poliedro iniziale StartPolyhedron
void CreateDual(PolyhedronMesh& StartPolyhedron, PolyhedronMesh& DualPolyhedron);

//Usata nella costruzione dei poliedri: dato un vettore di facce unordered_faces, tutte aventi come vertice comune il vertice denotato da
//vertex_id, la funzione ordina tale vettore di facce in modo che ciascuna faccia sia seguita da quella adiacente a essa, per costruire correttamente 
//gli edges
void order_faces(const vector<int>& unordered_faces, vector<int>& ordered_faces, const PolyhedronMesh& Polyhedron, const int& vertex_id);
