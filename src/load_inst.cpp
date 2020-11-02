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

//Load_inst() function: loads input database and attribute values into memory


#include<iostream>
#include <sstream>
#include <algorithm>
#include "load_inst.hpp"
#include "node_mdd.hpp"

using namespace std;

int M = 0, N = 0, L = 0, theta;									//M:size of largest sequences, N:number of sequences in input database, L:number of event types, theta:frequency threshold
	
vector<vector<int>*> items;									//input database in table format (rows of sequences, each sequence a set of events)
vector<vector<vector<double>*>*> attrs;								//vector of attribute values in table format (one table per attribute, each table rows of sequence attributes)
vector<int> max_attrs, min_attrs;								//minimum and maximum value of an attribute						
map<string,int> item_map;									//used when dic=1, build a dictionary of input (original) event names to consecutive numericals
map<int,string> item_map_rev;									//reverse map of numericals to original event names

bool Load_att(string &inst);									//load attribute function
bool Load_items(string &inst);									//load input database function

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

	theta = thresh * N;									//if a pattern occurs in more than theta number of sequences (thresh precentage of total sequences) it is frequent			
	
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
	min_attrs.push_back(1000000);

	if (inst=="Position"){							//If no time attribute is defined and only position of events is required use Position as name of attribute. (stores position of each event as attribute value)
		cout<< "Using position as time attribute\n";
		attrs.push_back(new vector<vector<double>*>);
		for (int i=0; i<N; i++){
			attrs.back()->push_back(new vector<double>);
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
		double ditem;
		int size_m;
		attrs.push_back(new vector<vector<double>*>);
		while (getline(file, line))
		{
			istringstream word(line);
			attrs.back()->push_back(new vector<double>);
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







