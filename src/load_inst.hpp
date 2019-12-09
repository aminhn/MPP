/*
---------------------------------------------------------------------------------------------------------------------------------- 
Copyright 2019 Amin Hosseininasab
This file is part of MPP mining algorithm.
MPP is a free software, and a GNU General Public License is pending.
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
extern vector<vector<vector<int>*>*> attrs;
extern string out_file;

extern map<int,string> item_map_rev;

extern bool b_disp, b_write, use_dic, out_replace, cmem;

extern vector<int> lgap, ugap, lavr, uavr, lspn, uspn, lmed, umed;				//vector keeoing the constraint RHS value per enforced attribute
extern vector<int> ugapi, lgapi, uspni, lspni, uavri, lavri, umedi, lmedi;		//vector keeping the attributes which have a type of constraint enforced on them
extern vector<int> num_minmax, num_avr, num_med, min_attrs, max_attrs; 						//vector keeping the number of specific type constraints enforced on an attribute (attribute per row) (used in keeping info at MDD node level)
extern vector<int> tot_gap, tot_spn, tot_avr, tot_med; 

extern int N, M, L, num_att, theta;

