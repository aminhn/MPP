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

//Build_MDD() function: build MDD database model of input tabular database

#include<vector>
#include <iostream>
#include "load_inst.hpp"
#include "build_mdd.hpp"

void Popl_nodes();							//Populates the MDD node vector
void Intlz_DFS(int ID, Node* fnod, Node* tnod);				//Initializes the DFS pattern queue with every possible sized one pattern that can be extended
void Add_arc(int ID, int strp, int endp);				//Adds an arc to the MDD database
void Intlz_node(int nod);						//Constructs an empty node
bool Check_gap(int i, int strt, int endp);				//Checks satisfaction of gap constraints

vector<Node*> datab_MDD;						//MDD database is essentially a vector of nodes
vector<Pattern*> DFS_queue;						//DFS queue of patterns to extend in mining algorithm

void Build_MDD() {

	datab_MDD = vector<Node*>(M * L, NULL);
	DFS_queue = vector<Pattern*>(L, NULL);

	Popl_nodes();

}


void Popl_nodes() {							//this function decides to build an arc between two nodes pointed to by strp and endp. An arc is contructed if it does not violate any of the imposed constraints

	bool antmon = 0;						//antimonotone property of contraints
	for (int i = 0; i < N; i++) {
		int endp = items[i]->size();				//endp initialized to last event in a sequence
		int strp = endp - 1;					//strp initialized to one to last event in sequence
		while (strp > 0) {
			while (antmon == 0) {				//while antimonotone property of contraints is violated backtrack on endp
				if (!ugap.empty() && ugapi[0] == 0 && abs(attrs[0]->at(i)->at(endp - 1) - attrs[0]->at(i)->at(strp - 1)) > ugap[0]) {		//antimonotone contraints are upper gap
					endp--;
					if (strp == endp) {
						strp--;
						if (strp == 0)
							break;
					}
				}
				else
					antmon = 1;
			}
			if (antmon == 1) {				//while monotone property of constraints is staisfied add arc from strp to all nodes between strp and endp
				int last_p = endp;
				while (endp != strp) {
					if (!lgap.empty() && lgapi[0] == 0 && abs(attrs[0]->at(i)->at(endp - 1) - attrs[0]->at(i)->at(strp - 1)) < lgap[0])
						break;
					if (tot_gap.empty() || (tot_gap[0] == 0 && tot_gap.size() == 1) || Check_gap(i ,strp, endp))
						Add_arc(i, strp, endp);
					endp--;
				}
				strp--;
				if (!ugap.empty())			//need to recheck whther antimonotone property is satisfied
					antmon = 0;
				endp = last_p;
			}
		}
	}
}


bool Check_gap(int i, int strp, int endp){				//checks upper and lower gap constraints imposed on any attribute value

	for (int att = 0; att < lgap.size(); att++){
		if (lgap[att] == 0)
			continue;
		if (abs(attrs[lgapi[att]]->at(i)->at(endp - 1) - attrs[lgapi[att]]->at(i)->at(strp - 1)) < lgap[att])
			return 0;
	}

	for (int att = 0; att < ugap.size(); att++){
		if (ugap[att] == 0)
			continue;
		if (abs(attrs[ugapi[att]]->at(i)->at(endp - 1) - attrs[ugapi[att]]->at(i)->at(strp - 1)) > ugap[att])
			return 0;
	}

	return 1;	
}


void Add_arc(int ID, int strp, int endp) {						//Adds an arc from strp node to endp node

	int fnod = items[ID]->at(strp - 1) + (strp - 1) * L;
	int tnod = items[ID]->at(endp - 1) + (endp - 1) * L;

	Intlz_node(fnod - 1);
	Intlz_node(tnod - 1);

	datab_MDD[tnod - 1]->assign_ID(ID + 1, endp, NULL);				//stores in MDD node the required information for constraint satisfaction in mining algorithm
	datab_MDD[fnod - 1]->assign_ID(ID + 1, strp, datab_MDD[tnod - 1]);

	Intlz_DFS(ID, datab_MDD[fnod - 1], datab_MDD[tnod - 1]);			//Adds pointer as starting point for mining algorithm
}


void Intlz_node(int nod) {								
	if (datab_MDD[nod] == NULL) {	
		datab_MDD[nod] = new Node();
		datab_MDD[nod]->ID = nod + 1;
	}
}


void Intlz_DFS(int ID, Node* fnod, Node* tnod) {					//adds a pattern (of size one) to DFS queue of mining algorithm. A pattern is added to queue only if it satisfies or can satisfy imposed constraints
											//If a pattern is added, it is initialized with required information for constraint satisfaction during minign algorithm. All code in this fuction either checks satisfaction or stores information
	if (fnod->parent != ID + 1){
		int att_pos = 0;
		for (vector<int>::iterator it = lspni.begin(); it != lspni.end(); it++){
			if (fnod->attr.back()->at(*it)->at(2) - fnod->attr.back()->at(*it)->at(1) < lspn[att_pos]) 
				return;
			att_pos++;
		}

		att_pos = 0;
		for (vector<int>::iterator it = lavri.begin(); it != lavri.end(); it++){
			if ((double)(fnod->attr.back()->at(*it)->at(0) + tnod->attr.back()->at(*it)->at(num_minmax[*it] + num_avr[*it])) / (1 + tnod->attr.back()->at(*it)->at(num_minmax[*it] + 2 * num_avr[*it])) < lavr[att_pos]) 
				return;
			att_pos++;
		}

		att_pos = 0;
		for (vector<int>::iterator it = uavri.begin(); it != uavri.end(); it++){
			if ((double)(fnod->attr.back()->at(*it)->at(0) + tnod->attr.back()->at(*it)->at(num_minmax[*it] + 1)) / (1 + tnod->attr.back()->at(*it)->at(num_minmax[*it] + num_avr[*it] + 1)) > uavr[att_pos]) 
				return;
			att_pos++;
		}

		att_pos = 0;
		for (vector<int>::iterator it = umedi.begin(); it != umedi.end(); it++){
			if (fnod->attr.back()->at(*it)->at(0) > umed[att_pos]){
				if (-1 + fnod->attr.back()->at(*it)->at(num_minmax[*it] + num_avr[*it] * 2 + (num_med[*it] - 1) * 3 + 1) < 0) 
					return;
				else if (-1 + fnod->attr.back()->at(*it)->at(num_minmax[*it] + num_avr[*it] * 2 + (num_med[*it] - 1) * 3 + 1) == 0){
					int fnod3;
					if (fnod->attr.back()->at(*it)->at(0) < fnod->attr.back()->at(*it)->at(num_minmax[*it] + num_avr[*it] * 2 + (num_med[*it] - 1) * 3 + 3))
						fnod3 = fnod->attr.back()->at(*it)->at(0);
					else
						fnod3 = fnod->attr.back()->at(*it)->at(num_minmax[*it] + num_avr[*it] * 2 + (num_med[*it] - 1) * 3 + 3);
					if (0.5 * (fnod3 + fnod->attr.back()->at(*it)->at(num_minmax[*it] + num_avr[*it] * 2 + (num_med[*it] - 1) * 3 + 2)) > umed[att_pos])
						return;
				}
			}
			att_pos++;
		}

		att_pos = 0;
		for (vector<int>::iterator it = lmedi.begin(); it != lmedi.end(); it++){
			if (fnod->attr.back()->at(*it)->at(0) < lmed[att_pos]){
				if (-1 + fnod->attr.back()->at(*it)->at(num_minmax[*it] + num_avr[*it] * 2 + 1) < 0) 
					return;
				else if (-1 + fnod->attr.back()->at(*it)->at(num_minmax[*it] + num_avr[*it] * 2 + 1) == 0){
					int fnod2;
					if (fnod->attr.back()->at(*it)->at(0) > fnod->attr.back()->at(*it)->at(num_minmax[*it] + num_avr[*it] * 2 + 2))
						fnod2 = fnod->attr.back()->at(*it)->at(0);
					else
						fnod2 = fnod->attr.back()->at(*it)->at(num_minmax[*it] + num_avr[*it] * 2 + 2);
					if (0.5 * (fnod2 + fnod->attr.back()->at(*it)->at(num_minmax[*it] + num_avr[*it] * 2 + 3)) < lmed[att_pos])
						return;
				}
			}
			att_pos++;
		}




		if (DFS_queue[fnod->item - 1] == NULL) {
			DFS_queue[fnod->item - 1] = new Pattern();
			DFS_queue[fnod->item - 1]->Update(ID + 1);
			DFS_queue[fnod->item - 1]->patt_seq.push_back(fnod->item);
		}

		if (DFS_queue[fnod->item - 1]->seq_ID.back() != ID + 1)
			DFS_queue[fnod->item - 1]->Update(ID + 1);

		DFS_queue[fnod->item - 1]->str_pnt.back()->push_back(fnod);

		for (int i = 0; i < tot_spn.size(); i++) {
			if (DFS_queue[fnod->item - 1]->spn.back()->size() < tot_spn.size())
				DFS_queue[fnod->item - 1]->spn.back()->push_back(new vector<vector<int>*>);
			DFS_queue[fnod->item - 1]->spn.back()->at(i)->push_back(new vector<int>(2, fnod->attr.back()->at(tot_spn[i])->at(0)));
		}
		for (int i = 0; i < tot_avr.size(); i++){
			if (DFS_queue[fnod->item - 1]->avr.back()->size() < tot_avr.size())
				DFS_queue[fnod->item - 1]->avr.back()->push_back(new vector<int>);
			DFS_queue[fnod->item - 1]->avr.back()->at(i)->push_back(fnod->attr.back()->at(tot_avr[i])->at(0));
		}

		for (int i = 0; i < lmedi.size(); i++){
			if (DFS_queue[fnod->item - 1]->lmed.back()->size() < lmedi.size())
				DFS_queue[fnod->item - 1]->lmed.back()->push_back(new vector<vector<int>*>);
			DFS_queue[fnod->item - 1]->lmed.back()->at(i)->push_back(new vector<int>(3));
			if (fnod->attr.back()->at(lmedi[i])->at(0) < lmed[i]){
				DFS_queue[fnod->item - 1]->lmed.back()->at(i)->back()->at(0) = -1;
				DFS_queue[fnod->item - 1]->lmed.back()->at(i)->back()->at(1) = fnod->attr.back()->at(lmedi[i])->at(0);
				DFS_queue[fnod->item - 1]->lmed.back()->at(i)->back()->at(2) = max_attrs[lmedi[i]] + 1;
			}
			else {
				DFS_queue[fnod->item - 1]->lmed.back()->at(i)->back()->at(0) = 1;
				DFS_queue[fnod->item - 1]->lmed.back()->at(i)->back()->at(1) = min_attrs[lmedi[i]] - 1;
				DFS_queue[fnod->item - 1]->lmed.back()->at(i)->back()->at(2) = fnod->attr.back()->at(lmedi[i])->at(0);
			}
		}

		for (int i = 0; i < umedi.size(); i++){
			if (DFS_queue[fnod->item - 1]->umed.back()->size() < umedi.size())
				DFS_queue[fnod->item - 1]->umed.back()->push_back(new vector<vector<int>*>);
			DFS_queue[fnod->item - 1]->umed.back()->at(i)->push_back(new vector<int>(3));
			if (fnod->attr.back()->at(umedi[i])->at(0) <= umed[i]){
				DFS_queue[fnod->item - 1]->umed.back()->at(i)->back()->at(0) = 1;
				DFS_queue[fnod->item - 1]->umed.back()->at(i)->back()->at(1) = fnod->attr.back()->at(umedi[i])->at(0);
				DFS_queue[fnod->item - 1]->umed.back()->at(i)->back()->at(2) = max_attrs[umedi[i]] + 1;
			}
			else {
				DFS_queue[fnod->item - 1]->umed.back()->at(i)->back()->at(0) = -1;
				DFS_queue[fnod->item - 1]->umed.back()->at(i)->back()->at(1) = min_attrs[umedi[i]] - 1;
				DFS_queue[fnod->item - 1]->umed.back()->at(i)->back()->at(2) = fnod->attr.back()->at(umedi[i])->at(0);
			}
		}

		fnod->parent = ID + 1;
	}

}



