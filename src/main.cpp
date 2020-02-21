/*
---------------------------------------------------------------------------------------------------------------------------------- 
Constraint-based Sequential Pattern Mining with Decision Diagrams
Copyright (C) 2020 Carnegie Mellon University
This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
----------------------------------------------------------------------------------------------------------------------------------
*/


// Main document, executes code in four steps: 
//1. Gets input from user on: 
//database name, path, and attribute(s), and types of constraints imposed on each attribute. Supports Gap, Span. Average, Median, can be extended to other constraints,
//contact aminh@andrew.cmu.edu for other constraint implementation. 
//2. Reads input database and attributes into memory using function "Load_instance()"
//3. Builds MDD database model of the input database and attributes using function "Build_MDD()" 
//4. Runs prefix projection mining algorithm on MDD database using function "Freq_miner()"


#include <iostream>
#include <time.h>
#include <string.h>
#include <string>
#include "load_inst.hpp"
#include "build_mdd.hpp"
#include "pattern.hpp" 
#include "freq_miner.hpp"
#include "utility.hpp"

using namespace std;

string out_file;

int num_att = 0;								//total number of attributes
bool  b_disp = 0, b_write = 0, use_dic = 0, out_replace = 0;			//boolian variables (1:yes, 0:no) respectively: display patterns in terminal, write patterns onto file, use dictionary for input database (used when database does not satisfy consecutive numeric values to represent events),

vector<double> lgap, ugap, lavr, uavr, lspn, uspn, lmed, umed;			//vector of constraint RHS value per enforced attribute
vector<int> ugapi, lgapi, uspni, lspni, uavri, lavri, umedi, lmedi;		//vector of attribute numbers which have a constraint type enforced on them, e.g. ugapi = {1,3} implies attributes 1 and 3 have a upper gap constraint
vector<int> num_minmax, num_avr, num_med; 					//vector of the number of specific type constraints enforced on an attribute (attribute per row) (used in keeping info at MDD node level) minmax: span
vector<int> tot_gap, tot_spn, tot_avr;						//vector of attributes numbers which have upper or lower constraint types enforced on them (used in storing info at pattern level)

int main(int argc, char* argv[]) {

	double thresh = 0;							//frequency threshold
        string file_name;
	int att_num = -1;							//current attribute number
	vector<bool> add_total(3, 1);						//vector of indicators on whether a constraint type has been inforced on current attribute or not (add_total[0]:gap [1]:span [2]:average)
	vector<string> attr_files;
        for (int i=1; i<argc; i++){
		if (argv[i][0] !='-' || isdigit(argv[i][1]))
			continue;
		else if (strcmp(argv[i], "-thr") == 0)
			thresh = stod(argv[i + 1]);
                else if (strcmp(argv[i], "-file") == 0)
                        file_name = argv[i + 1];
		else if (strcmp(argv[i], "-att") == 0){				//executed when new attribute is introduced
			att_num++;
			num_att++;
			add_total = vector<bool>(3, 1);
			attr_files.push_back(argv[i + 1]);
			num_minmax.push_back(0);
			num_avr.push_back(0);
			num_med.push_back(0);
		}
		else if (att_num == -1 && (argv[i][1] == 'l' || argv[i][1] == 'u')){
			cout << "Error: No atrribute file specefied for constraint << argv[i] <<, exiting.\n";
			return 0;
		}
		else if (strcmp(argv[i], "-lg") == 0){				//lower gap constraint on attribute number "att_num"
			lgap.push_back(stod(argv[i + 1]));
			lgapi.push_back(att_num);
			if (add_total[0] == 1) {
				tot_gap.push_back(att_num);
				add_total[0] = 0;
			}
		}
		else if (strcmp(argv[i], "-ug") == 0){				//upper gap constraint on attribute number "att_num"
			ugap.push_back(stod(argv[i + 1]));
			ugapi.push_back(att_num);
			if (add_total[0] == 1) {
				tot_gap.push_back(att_num);
				add_total[0] = 0;
			}
		}
		else if (strcmp(argv[i], "-ls") == 0){				//lower span constraint on attribute number "att_num"
			lspn.push_back(stod(argv[i + 1]));
			lspni.push_back(att_num);
			num_minmax.back()++;
			num_minmax.back()++;
			if (add_total[1] == 1) {
				tot_spn.push_back(att_num);
				add_total[1] = 0;
			}
		}
		else if (strcmp(argv[i], "-us") == 0){				//upper span constraint on attribute number "att_num"
			uspn.push_back(stod(argv[i + 1]));
			uspni.push_back(att_num);
			if (add_total[1] == 1) {
				tot_spn.push_back(att_num);
				add_total[1] = 0;
			}
		}
		else if (strcmp(argv[i], "-la") == 0){				//lower average constraint on attribute number "att_num"
			lavr.push_back(stod(argv[i + 1]));
			lavri.push_back(att_num);
			num_avr.back()++;
			if (add_total[2] == 1) {
				tot_avr.push_back(att_num);
				add_total[2] = 0;
			}
		}
		else if (strcmp(argv[i], "-ua") == 0){				//upper gap constraint on attribute number "att_num"
			uavr.push_back(stod(argv[i + 1]));
			uavri.push_back(att_num);
			num_avr.back()++;
			if (add_total[2] == 1) {
				tot_avr.push_back(att_num);
				add_total[2] = 0;
			}
		}
		else if (strcmp(argv[i], "-lm") == 0){				//lower median constraint on attribute number "att_num"
			lmed.push_back(stod(argv[i + 1]));
			lmedi.push_back(att_num);
			num_med.back()++;
		}
		else if (strcmp(argv[i], "-um") == 0){				//upper median constraint on attribute number "att_num"
			umed.push_back(stod(argv[i + 1]));
			umedi.push_back(att_num);
			num_med.back()++;
		}
		else if (strcmp(argv[i], "-dic") == 0)
			use_dic = 1;
		else if (strcmp(argv[i], "-out") == 0)			
			b_disp = 1;
		else if (strcmp(argv[i], "-write") == 0){
			b_write = 1;
			out_file = argv[i + 1];
		}
		else 
			cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!Command " << argv[i] << " not recognized and skipped.!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
        }

	if (thresh == 0){	
		cout << "No Threshold given, using threshold deafult of 1%\n";
		thresh = 0.01;
	}

        cout << "\n********************** " << file_name << " **********************\n";

	string item_file = file_name;

	cout << "loading instances...\n";

	if(!Load_instance(item_file, &attr_files, thresh)){			//load instance into memory
		cout << "Files invalid, exiting.\n";
		return 0;
	}

	cout << "Instances loaded\n\nBuilding MDD database...\n";

	clock_t kk = clock();

	Build_MDD();								//build MDD database

	cout << "MDD database built, CPU time is: ";
	cout << give_time(clock() - kk) << "\n\nRunning mining algorithm...\n";

	if (b_write){
		ofstream file;
		file.open(out_file, std::ios::app);
		file << "\n********************** " << file_name << " **********************\n";
		//file << "Database: " << folder << ", thr: " << thresh << ", gap_ub: " << ugap << ", spn_lb: " << lspn << ", spn_ub: " << uspn << "\n";
		file << "\nMDD Database built in " << give_time(clock() - kk) << " seconds\n\n";
		file.close();
	}


	Freq_miner();								//run prefix projection mining algorithm

	cout << "Mining Complete\n\nFound a total of " << num_patt << " patterns\n";
	cout << "Found a total of " << num_max_patt << " maximal patterns\n";

	cout << "Total CPU time is: ";
	cout << give_time(clock() - kk) << "\n";

	if (b_write){
		ofstream file;
		file.open(out_file, std::ios::app);
		file << "\nMining completed in " << give_time(clock() - kk) << " seconds\n";
		file << "Found a total of " << num_max_patt << " maximal patterns\n";
		file.close();
	}

	return 0;
}
