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


#pragma once

#include<vector>
#include<string>
#include <fstream>
#include <map>

using namespace std;

bool Load_instance(string &items_file, vector<string>* attrs_files, double thresh);

extern vector<vector<int>* > items;
extern vector<vector<vector<double>*>*> attrs;
extern string out_file;

extern map<int,string> item_map_rev;

extern bool b_disp, b_write, use_dic, out_replace, cmem;

extern vector<double> lgap, ugap, lavr, uavr, lspn, uspn, lmed, umed;				//vector keeoing the constraint RHS value per enforced attribute
extern vector<int> ugapi, lgapi, uspni, lspni, uavri, lavri, umedi, lmedi;		//vector keeping the attributes which have a type of constraint enforced on them
extern vector<int> num_minmax, num_avr, num_med, min_attrs, max_attrs; 						//vector keeping the number of specific type constraints enforced on an attribute (attribute per row) (used in keeping info at MDD node level)
extern vector<int> tot_gap, tot_spn, tot_avr, tot_med; 

extern int N, M, L, num_att, theta;

