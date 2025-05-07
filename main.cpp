#include <sstream>

int main(int argc, char *argv[])
{
	unsigned int p;
	unsigned int q;
	unsigned int b;
	unsigned int c;
	unsigned int id_vertice_1;
	unsigned int id_vertice_2;
	
	stringstream convert;
	
	for (unsigned int i = 1; i < argc; i++)
		convert << argv[i];
	
	convert >> p >> q >> b >> c;
	if (argc == 7)
		convert >> id_vertice_1 >> id_vertice_2;
		
	return 0;
}