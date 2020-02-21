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

#include <vector>
#include "node_mdd.hpp"
#include "load_inst.hpp"

class Pattern {									//Nodes of the pattern MDD
public:

	//int parent;									//Parent of sequence (ID used in pattern MDD)
	int ID;
	int freq;									//#Seqs containing pattern
	int act_freq;

	vector<int> patt_seq;						//Item sequence defining the pattern
	bool cond;
	vector<int> seq_ID;

	vector<vector<Node*>*> str_pnt;				//vector of pointers indicating the end position of a pattern inside the MDD, w.r.t. each seq
	vector<vector<vector<vector<int>*>*>*> spn;			
	vector<vector<vector<int>*>*> avr;			//one row per sequence, per attribute, one column per start pointer
	vector<vector<vector<vector<int>*>*>*> lmed;            //one row per sequence, per attribute, per start pointer, one column per information
	vector<vector<vector<vector<int>*>*>*> umed;

	void Update(int seq) {
		seq_ID.push_back(seq);
		str_pnt.push_back(new vector<Node*>);
		if (!tot_spn.empty())
			spn.push_back(new vector<vector<vector<int>*>*>);
		if (!tot_avr.empty())
			avr.push_back(new vector<vector<int>*>);
		if (!lmedi.empty())
			lmed.push_back(new vector<vector<vector<int>*>*>);

		if (!umedi.empty())
			umed.push_back(new vector<vector<vector<int>*>*>);

		cond = 1;
		freq++;
	}

	Pattern() {
		ID = 0;
		freq = 0;
		act_freq = 0;
	}

	~Pattern() { 
		for (int i = 0; i < str_pnt.size(); i++){
			delete str_pnt[i];
		}
		for (int i = 0; i < spn.size(); i++){
			for (int j = 0; j < spn[i]->size(); j++){
				for (int k = 0; k < spn[i]->at(j)->size(); k++)
					delete spn[i]->at(j)->at(k);
				delete spn[i]->at(j);
			}
			delete spn[i];
		}
		for (int i = 0; i < avr.size(); i++){
			for (int j = 0; j < avr[i]->size(); j++)
					delete avr[i]->at(j);
			delete avr[i];
		}
		for (int i = 0; i < lmed.size(); i++){
			for (int j = 0; j < lmed[i]->size(); j++){
				for (int k = 0; k < lmed[i]->at(j)->size(); k++)
					delete lmed[i]->at(j)->at(k);
				delete lmed[i]->at(j);
			}
			delete lmed[i];
		}
		for (int i = 0; i < umed.size(); i++){
			for (int j = 0; j < umed[i]->size(); j++){
				for (int k = 0; k < umed[i]->at(j)->size(); k++)
					delete umed[i]->at(j)->at(k);
				delete umed[i]->at(j);
			}
			delete umed[i];
		}

	}

};

