/*
---------------------------------------------------------------------------------------------------------------------------------- 
Copyright 2019 Amin Hosseininasab
This file is part of MPP mining algorithm.
MPP is a free software, and a GNU General Public License is pending.
----------------------------------------------------------------------------------------------------------------------------------
*/


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

int num_att = 0;								
bool  b_disp = 0, b_write = 0, use_dic = 0, out_replace = 0;			

vector<int> lgap, ugap, lavr, uavr, lspn, uspn, lmed, umed;			
vector<int> ugapi, lgapi, uspni, lspni, uavri, lavri, umedi, lmedi;		
vector<int> num_minmax, num_avr, num_med; 					
vector<int> tot_gap, tot_spn, tot_avr;						

int main(int argc, char* argv[]) {

	double thresh = 0;							
        string file_name;
	int att_num = -1;							
	vector<bool> add_total(3, 1);						
	vector<string> attr_files;
        for (int i=1; i<argc; i++){
		if (argv[i][0] !='-')
			continue;
		else if (strcmp(argv[i], "-thr") == 0)
			thresh = stof(argv[i + 1]);
                else if (strcmp(argv[i], "-file") == 0)
                        file_name = argv[i + 1];
		else if (strcmp(argv[i], "-att") == 0){				
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
		else if (strcmp(argv[i], "-lg") == 0){				
			lgap.push_back(stoi(argv[i + 1]));
			lgapi.push_back(att_num);
			if (add_total[0] == 1) {
				tot_gap.push_back(att_num);
				add_total[0] = 0;
			}
		}
		else if (strcmp(argv[i], "-ug") == 0){				
			ugap.push_back(stoi(argv[i + 1]));
			ugapi.push_back(att_num);
			if (add_total[0] == 1) {
				tot_gap.push_back(att_num);
				add_total[0] = 0;
			}
		}
		else if (strcmp(argv[i], "-ls") == 0){				
			lspn.push_back(stoi(argv[i + 1]));
			lspni.push_back(att_num);
			num_minmax.back()++;
			num_minmax.back()++;
			if (add_total[1] == 1) {
				tot_spn.push_back(att_num);
				add_total[1] = 0;
			}
		}
		else if (strcmp(argv[i], "-us") == 0){				
			uspn.push_back(stoi(argv[i + 1]));
			uspni.push_back(att_num);
			if (add_total[1] == 1) {
				tot_spn.push_back(att_num);
				add_total[1] = 0;
			}
		}
		else if (strcmp(argv[i], "-la") == 0){				
			lavr.push_back(stoi(argv[i + 1]));
			lavri.push_back(att_num);
			num_avr.back()++;
			if (add_total[2] == 1) {
				tot_avr.push_back(att_num);
				add_total[2] = 0;
			}
		}
		else if (strcmp(argv[i], "-ua") == 0){				
			uavr.push_back(stoi(argv[i + 1]));
			uavri.push_back(att_num);
			num_avr.back()++;
			if (add_total[2] == 1) {
				tot_avr.push_back(att_num);
				add_total[2] = 0;
			}
		}
		else if (strcmp(argv[i], "-lm") == 0){				
			lmed.push_back(stoi(argv[i + 1]));
			lmedi.push_back(att_num);
			num_med.back()++;
		}
		else if (strcmp(argv[i], "-um") == 0){				
			umed.push_back(stoi(argv[i + 1]));
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

	if(!Load_instance(item_file, &attr_files, thresh)){			
		cout << "Files invalid, exiting.\n";
		return 0;
	}

	cout << "Instances loaded\n\nBuilding MDD database...\n";

	clock_t kk = clock();

	Build_MDD();								

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


	Freq_miner();								

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
