/*
---------------------------------------------------------------------------------------------------------------------------------- 
Copyright 2019 Amin Hosseininasab
This file is part of MPP mining algorithm.
MPP is a free software, and a GNU General Public License is pending.
----------------------------------------------------------------------------------------------------------------------------------
*/


#include<iostream>
#include <sstream>
#include <algorithm>
#include "load_inst.hpp"
#include "node_mdd.hpp"

using namespace std;

int M = 0, N = 0, L = 0, theta;									
	
vector<vector<int>*> items;									
vector<vector<vector<int>*>*> attrs;								
vector<int> max_attrs, min_attrs;												
map<string,int> item_map;									
map<int,string> item_map_rev;									

bool Load_att(string &inst);									
bool Load_items(string &inst);									

bool Load_instance(string &items_file, vector<string>* attrs_files, double thresh) {
	
	attrs.reserve(attrs_files->size());

	if(!Load_items(items_file))
		return 0;

	cout << "Found " << N << " valid lines, with maximum line length of " << M << ", and total of " << L << " items\n";

	for (int i = 0; i < num_att ; i++) {
		if(!Load_att(attrs_files->at(i)))
			return 0;
		cout << "Attribute " << attrs_files->at(i) << " loaded\n";
	}

	theta = thresh * N;									
	
	return 1;
}

bool Load_items(string &inst) {

	ifstream file(inst);

	if (file.good())
	{
		string line;
		int size_m;
		int ditem;
		while (getline(file, line))
		{
			istringstream word(line);
			items.push_back(new vector<int>);
			string itm;
			size_m = 0;
			while (word >> itm) {
				if (use_dic){
					map<string,int>::iterator it = item_map.find(itm);
					if (it == item_map.end()){
						item_map[itm] = ++L;
						item_map_rev[L] = itm;
						ditem = L;
					}
					else
						ditem = it->second;
				}
				else
					ditem = abs(stod(itm));

				items.back()->push_back(ditem);
				size_m++;
				if (use_dic == 0 && ditem > L)
					L = ditem;
			}
			if (size_m > M)
				M = size_m;
			N++;
		}

	}
	else {
		cout << "!!!!!! No such file exists: " << inst << " !!!!!!\n";
		return 0;
	}
	
	return 1;
}

bool Load_att(string &inst) {

	max_attrs.push_back(0);
	min_attrs.push_back(10^6);

	if (inst=="Position"){							
		cout<< "Using position as time attribute\n";
		attrs.push_back(new vector<vector<int>*>);
		for (int i=0; i<N; i++){
			attrs.back()->push_back(new vector<int>);
			for (int j=0; j<items[i]->size(); j++){
				attrs.back()->back()->push_back(j+1);
			}
		}	
		return 1;
	}

	ifstream file(inst);
	if (file.good())
	{
		string line;
		int ditem;
		int size_m;
		attrs.push_back(new vector<vector<int>*>);
		while (getline(file, line))
		{
			istringstream word(line);
			attrs.back()->push_back(new vector<int>);
			string itm;
			while (word >> itm) {
				ditem = stod(itm);
				attrs.back()->back()->push_back(ditem);
				if (ditem > max_attrs.back())
					max_attrs.back() = ditem;
				if (ditem < min_attrs.back())
					min_attrs.back() = ditem;
			}
		}
	}
	else {
		cout << "!!!!!! No such file exists: " << inst << " !!!!!!\n";
		return 0;
	}

	return 1;
}







