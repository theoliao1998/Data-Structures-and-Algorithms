#include <getopt.h>
#include <iostream>
#include <vector>
#include "imple.h"

using namespace std;


int main(int argc, char **argv){
	int c;
	bool verbose = 0;
	bool median = 0;
	bool midpoint = 0;
	bool transfer = 0;
	bool timetravel = 0;
	vector<string> eq_symbs;
	unsigned size = 0;
	while (1){
		static struct option long_options[] =
		{
			{"verbose",  no_argument, 0, 'v'},
			{"median",  no_argument, 0, 'm'},
			{"midpoint",  no_argument, 0, 'p'},
			{"transfer",  no_argument, 0, 't'},
			{"ttt",  required_argument, 0, 'g'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "vmptg:",
                       long_options, &option_index);

		if (c == -1)
			break;
		string s;
		switch (c){
			case 'v':  
				verbose = 1;        
				break;
			case 'm':  
				median = 1;        
				break;
			case 'p':  
				midpoint = 1;        
				break;
			case 't':  
				transfer = 1;        
				break;
			case 'g':
				timetravel = 1;
				s = optarg;
				eq_symbs.push_back(s);
				size++;
				break;
			default:
				break;
		}
    }
    implement(verbose,median,midpoint,transfer,timetravel,eq_symbs,size);
}

