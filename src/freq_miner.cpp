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

//Freq_miner() function: mines all frequent patterns in the MDD database 


#include "freq_miner.hpp"
#include <iostream>
#include <time.h>
#include "utility.hpp"

void Extend_patt(Pattern*);										//Extends a pattern by one event
void Find_items(int ID, Pattern* _patt, vector<Pattern*>* pot_patt, vector<int>* item_count);		//Finds number patterns of the form <_patt,event> in the database, for all event types
void Out_final_patt(vector<int>* seq, int freq);							//Writes found pattern to terminal or file
int Check_cons(int ID, int par_pos, Node* tnod, Pattern* _patt);					//Checks satisfcation of constraints during mining algorithm

vector<bool> indic_vec;

int num_patt;
int num_max_patt;
int iter;
int chil_ID_pos;
double mm=0;

void Freq_miner() {

	num_patt = 0;

	if (b_write){
		ofstream file;
		file.open(out_file);
		file << "\n\nPatterns found for frequency: %" << theta << "\n\n";
		file.close();
	}

	while (!DFS_queue.empty()) {								//takes pattern out from last input to DFS queue and searches for its extension by possible events
		if (DFS_queue.back() != NULL && DFS_queue.back()->freq >= theta)
			Extend_patt(DFS_queue.back());
		else {
			if (DFS_queue.back()!=NULL)
				DFS_queue.back()->~Pattern();
			DFS_queue.pop_back();
		}
	}
}


void Extend_patt(Pattern* _patt) {								//Extends _patt by any possible event types

	num_patt++;
	DFS_queue.pop_back();

	indic_vec = vector<bool>(L, 1);
	vector<int> item_count(L, 0);
	iter = 0;										//position at which the str_pnt vector of ID under consideration is stored at parent node 

	vector<Pattern*> pot_patt(L, NULL);

	for (int i = 0; i < _patt->str_pnt.size(); i++) {
		Find_items(_patt->seq_ID[i] - 1, _patt, &pot_patt, &item_count);		//finds number of patterns per L (number of event types) possible extensions of _patt
		iter++;

	}


	int all = 0;
	for (int i = 0; i < L; i++) {								//For every possible extension checks frequency threshold, if satisfied adds new patter to DFS queue
		if (item_count[i] >= theta) {
			pot_patt[i]->patt_seq = _patt->patt_seq;
			pot_patt[i]->patt_seq.push_back(i + 1);
			pot_patt[i]->freq = item_count[i];

			DFS_queue.push_back(pot_patt[i]);

			all++;
		}
		else if (pot_patt[i] != NULL && pot_patt[i]!=0)
			pot_patt[i]->~Pattern();
	}

	if (_patt->patt_seq.size() > 1 && _patt->act_freq >= theta) {				//A maximal pattern (cannot be extended further by any event)
		num_max_patt++;
		if (b_disp || b_write)
			Out_final_patt(&_patt->patt_seq, _patt->act_freq);
		//cin.get();
	}
	_patt->~Pattern();
        
}


void Find_items(int ID, Pattern* _patt, vector<Pattern*>* pot_patt, vector<int>* item_count) {			//Find extensions by searching children of MDD nodes

	int par_pos = _patt->str_pnt[iter]->size();	//position of parent in str_pnt[iter] vector (back_tracking on parent vector due to bottom-up MDD construction)

	for (vector<Node*>::reverse_iterator it1 = _patt->str_pnt[iter]->rbegin(); it1 != _patt->str_pnt[iter]->rend(); it1++) {

		par_pos--;
		int ID_pos = find_ID(ID + 1, &(*it1)->seq_ID);							//position of ID in parent vector (used to get position of relevent children, time, skip, etc., vectors)
		if (ID_pos == -1)
			continue;

		int chil_pos = (*it1)->children[ID_pos]->size()-1;						//position of child in children vector 
		for (vector<Node*>::reverse_iterator it2 = (*it1)->children[ID_pos]->rbegin(); it2 != (*it1)->children[ID_pos]->rend(); it2++) {


			if (indic_vec[(*it2)->item - 1] == 0)
				continue;

			int cond = 1;
			if (!tot_spn.empty() || !tot_avr.empty() || !lmedi.empty() || !umedi.empty()) {		//constraint check
				cond = Check_cons(ID, par_pos, *it2, _patt);
				if (cond == -1)
					break;
				else if (cond == 0)
					continue;
			}

			if (iter - (*item_count)[(*it2)->item - 1] > _patt->freq - theta) {			//rest of code corresponds to information generation and storing for constraint satisfaction
				indic_vec[(*it2)->item - 1] = 0;
				continue;
			}

			if ((*pot_patt)[(*it2)->item - 1] == NULL) {
				(*pot_patt)[(*it2)->item - 1] = new Pattern();
				(*pot_patt)[(*it2)->item - 1]->Update(ID + 1);
				(*item_count)[(*it2)->item - 1]++;
			}

			if ((*pot_patt)[(*it2)->item - 1]->seq_ID.back() != ID + 1) {
				(*pot_patt)[(*it2)->item - 1]->Update(ID + 1);
				(*item_count)[(*it2)->item - 1]++;
			}

			if (cond == 1 && (*pot_patt)[(*it2)->item - 1]->cond == 1) {
				(*pot_patt)[(*it2)->item - 1]->cond = 0;
				(*pot_patt)[(*it2)->item - 1]->act_freq++;
			}
			
                    	(*pot_patt)[(*it2)->item - 1]->str_pnt.back()->push_back((*it2));

			for (int i = 0; i < tot_spn.size(); i++) {
				if ((*pot_patt)[(*it2)->item - 1]->spn.back()->size() < tot_spn.size())
					(*pot_patt)[(*it2)->item - 1]->spn.back()->push_back(new vector<vector<int>*>);
				(*pot_patt)[(*it2)->item - 1]->spn.back()->at(i)->push_back(new vector<int>(2));
				if ((*it2)->attr[chil_ID_pos]->at(tot_spn[i])->at(0) < _patt->spn[iter]->at(i)->at(par_pos)->at(0))
					(*pot_patt)[(*it2)->item - 1]->spn.back()->at(i)->back()->at(0) = (*it2)->attr[chil_ID_pos]->at(tot_spn[i])->at(0);
				else
					(*pot_patt)[(*it2)->item - 1]->spn.back()->at(i)->back()->at(0) = _patt->spn[iter]->at(i)->at(par_pos)->at(0);
				if ((*it2)->attr[chil_ID_pos]->at(tot_spn[i])->at(0) > _patt->spn[iter]->at(i)->at(par_pos)->at(1))
					(*pot_patt)[(*it2)->item - 1]->spn.back()->at(i)->back()->at(1) = (*it2)->attr[chil_ID_pos]->at(tot_spn[i])->at(0);
				else
					(*pot_patt)[(*it2)->item - 1]->spn.back()->at(i)->back()->at(1) = _patt->spn[iter]->at(i)->at(par_pos)->at(1);
			}

			for (int i = 0; i < tot_avr.size(); i++) {
				if ((*pot_patt)[(*it2)->item - 1]->avr.back()->size() < tot_avr.size())
					(*pot_patt)[(*it2)->item - 1]->avr.back()->push_back(new vector<int>);
				(*pot_patt)[(*it2)->item - 1]->avr.back()->at(i)->push_back(_patt->avr[iter]->at(i)->at(par_pos) +  (*it2)->attr[chil_ID_pos]->at(tot_avr[i])->at(0));
			}

			for (int i = 0; i < lmedi.size(); i++){
				if ((*pot_patt)[(*it2)->item - 1]->lmed.back()->size() < lmedi.size())
					(*pot_patt)[(*it2)->item - 1]->lmed.back()->push_back(new vector<vector<int>*>);
				(*pot_patt)[(*it2)->item - 1]->lmed.back()->at(i)->push_back(new vector<int>(3));
				if ((*it2)->attr[chil_ID_pos]->at(lmedi[i])->at(0) < lmed[i]){
					(*pot_patt)[(*it2)->item - 1]->lmed.back()->at(i)->back()->at(0) = _patt->lmed[iter]->at(i)->at(par_pos)->at(0) - 1;
					(*pot_patt)[(*it2)->item - 1]->lmed.back()->at(i)->back()->at(2) = _patt->lmed[iter]->at(i)->at(par_pos)->at(2);
					if (_patt->lmed[iter]->at(i)->at(par_pos)->at(1) > (*it2)->attr[chil_ID_pos]->at(lmedi[i])->at(0)) 				//max of mins
						(*pot_patt)[(*it2)->item - 1]->lmed.back()->at(i)->back()->at(1) = _patt->lmed[iter]->at(i)->at(par_pos)->at(1);
					else
						(*pot_patt)[(*it2)->item - 1]->lmed.back()->at(i)->back()->at(1) = (*it2)->attr[chil_ID_pos]->at(lmedi[i])->at(0);
				}
				else {
					(*pot_patt)[(*it2)->item - 1]->lmed.back()->at(i)->back()->at(0) = _patt->lmed[iter]->at(i)->at(par_pos)->at(0) + 1;
					(*pot_patt)[(*it2)->item - 1]->lmed.back()->at(i)->back()->at(1) = _patt->lmed[iter]->at(i)->at(par_pos)->at(1);
					if (_patt->lmed[iter]->at(i)->at(par_pos)->at(2) < (*it2)->attr[chil_ID_pos]->at(lmedi[i])->at(0)) 				//min of maxs
						(*pot_patt)[(*it2)->item - 1]->lmed.back()->at(i)->back()->at(2) = _patt->lmed[iter]->at(i)->at(par_pos)->at(2);
					else
						(*pot_patt)[(*it2)->item - 1]->lmed.back()->at(i)->back()->at(2) = (*it2)->attr[chil_ID_pos]->at(lmedi[i])->at(0);
				}
			}
			for (int i = 0; i < umedi.size(); i++){
				if ((*pot_patt)[(*it2)->item - 1]->umed.back()->size() < umedi.size())
					(*pot_patt)[(*it2)->item - 1]->umed.back()->push_back(new vector<vector<int>*>);
				(*pot_patt)[(*it2)->item - 1]->umed.back()->at(i)->push_back(new vector<int>(3));
				if ((*it2)->attr[chil_ID_pos]->at(umedi[i])->at(0) <= umed[i]){
					(*pot_patt)[(*it2)->item - 1]->umed.back()->at(i)->back()->at(0) = _patt->umed[iter]->at(i)->at(par_pos)->at(0) + 1;
					(*pot_patt)[(*it2)->item - 1]->umed.back()->at(i)->back()->at(2) = _patt->umed[iter]->at(i)->at(par_pos)->at(2);
					if (_patt->umed[iter]->at(i)->at(par_pos)->at(1) > (*it2)->attr[chil_ID_pos]->at(umedi[i])->at(0))				//max of mins
						(*pot_patt)[(*it2)->item - 1]->umed.back()->at(i)->back()->at(1) = _patt->umed[iter]->at(i)->at(par_pos)->at(1);
					else
						(*pot_patt)[(*it2)->item - 1]->umed.back()->at(i)->back()->at(1) = (*it2)->attr[chil_ID_pos]->at(umedi[i])->at(0);
				}
				else {
					(*pot_patt)[(*it2)->item - 1]->umed.back()->at(i)->back()->at(0) = _patt->umed[iter]->at(i)->at(par_pos)->at(0) - 1;
					(*pot_patt)[(*it2)->item - 1]->umed.back()->at(i)->back()->at(1) = _patt->umed[iter]->at(i)->at(par_pos)->at(1);
					if (_patt->umed[iter]->at(i)->at(par_pos)->at(2) < (*it2)->attr[chil_ID_pos]->at(umedi[i])->at(0))				//min of maxs
						(*pot_patt)[(*it2)->item - 1]->umed.back()->at(i)->back()->at(2) = _patt->umed[iter]->at(i)->at(par_pos)->at(2);
					else
						(*pot_patt)[(*it2)->item - 1]->umed.back()->at(i)->back()->at(2) = (*it2)->attr[chil_ID_pos]->at(umedi[i])->at(0);
				}
			}


		}
		chil_pos--;
	}
}


int Check_cons(int ID, int par_pos, Node* tnod, Pattern* _patt) {							//constriant satisfaction

	chil_ID_pos = find_ID(ID + 1, &tnod->seq_ID);   								//position of ID in child vector
	int satis = 1;

	int att_pos = 0;												//position of attribute in constraint vector
	for (vector<int>::iterator it = uspni.begin(); it != uspni.end(); it++){					//upper bound span, antimonotone, pattern cannot be extended to any feasible one for this node or any other node (child)
		if (*it == 0 && tnod->attr[chil_ID_pos]->at(*it)->at(0) - _patt->spn[iter]->at(att_pos)->at(par_pos)->at(0) > uspn[att_pos])
			return -1;
		else if(*it != 0){
			int act_spn;
			if (tnod->attr[chil_ID_pos]->at(*it)->at(0) < _patt->spn[iter]->at(att_pos)->at(par_pos)->at(0))
				act_spn = _patt->spn[iter]->at(att_pos)->at(par_pos)->at(1) - tnod->attr[chil_ID_pos]->at(*it)->at(0);
			else if(tnod->attr[chil_ID_pos]->at(*it)->at(0) > _patt->spn[iter]->at(att_pos)->at(par_pos)->at(1))
				act_spn = tnod->attr[chil_ID_pos]->at(*it)->at(0) - _patt->spn[iter]->at(att_pos)->at(par_pos)->at(0);
			else
				act_spn = _patt->spn[iter]->at(att_pos)->at(par_pos)->at(1) - _patt->spn[iter]->at(att_pos)->at(par_pos)->at(0);
			if (act_spn > uspn[att_pos])
				return 0;
		}
		att_pos++;
	}

	att_pos = 0;
	for (vector<int>::iterator it = lspni.begin(); it != lspni.end(); it++){					//lower bound span, if condition holds, current node cannot be extended to feasible one but later nodes may
		if (*it == 0 && tnod->attr[chil_ID_pos]->at(*it)->at(0) - _patt->spn[iter]->at(att_pos)->at(par_pos)->at(0) < lspn[att_pos]){
			if (tnod->attr[chil_ID_pos]->at(*it)->at(2) - _patt->spn[iter]->at(att_pos)->at(par_pos)->at(0) < lspn[att_pos])
				return 0;
			else
				satis = 2;
		}
		else if (*it != 0){ 
			int act_spn;		
			if (tnod->attr[chil_ID_pos]->at(*it)->at(0) < _patt->spn[iter]->at(att_pos)->at(par_pos)->at(0))
				act_spn = _patt->spn[iter]->at(att_pos)->at(par_pos)->at(1) - tnod->attr[chil_ID_pos]->at(*it)->at(0);
			else if(tnod->attr[chil_ID_pos]->at(*it)->at(0) > _patt->spn[iter]->at(att_pos)->at(par_pos)->at(1))
				act_spn = tnod->attr[chil_ID_pos]->at(*it)->at(0) - _patt->spn[iter]->at(att_pos)->at(par_pos)->at(0);
			else
				act_spn = _patt->spn[iter]->at(att_pos)->at(par_pos)->at(1) - _patt->spn[iter]->at(att_pos)->at(par_pos)->at(0);
			if (act_spn < lspn[att_pos]){
				int low, hig;
				if (tnod->attr[chil_ID_pos]->at(*it)->at(2) > _patt->spn[iter]->at(att_pos)->at(par_pos)->at(1))
					hig = tnod->attr[chil_ID_pos]->at(*it)->at(2);
				else
					hig = _patt->spn[iter]->at(att_pos)->at(par_pos)->at(1);
				if (tnod->attr[chil_ID_pos]->at(*it)->at(1) < _patt->spn[iter]->at(att_pos)->at(par_pos)->at(0))
					low = tnod->attr[chil_ID_pos]->at(*it)->at(1);
				else
					low = _patt->spn[iter]->at(att_pos)->at(par_pos)->at(0);
				if (hig - low < lspn[att_pos])
					return 0;
				else
					satis = 2;
			}
		}
		att_pos++;
	}

	for (int att = 0; att < uavri.size(); att++){
		double act_pavr = (double)(_patt->avr[iter]->at(att)->at(par_pos) + tnod->attr[chil_ID_pos]->at(uavri[att])->at(0)) / (_patt->patt_seq.size() + 1);
		if (act_pavr <= uavr[att])
			continue;
		else
			satis = 2;
		double lb_pavr = (double)(_patt->avr[iter]->at(att)->at(par_pos) + tnod->attr[chil_ID_pos]->at(uavri[att])->at(num_minmax[uavri[att]] + 1)) / (_patt->patt_seq.size() + tnod->attr[chil_ID_pos]->at(uavri[att])->at(num_minmax[uavri[att]] + num_avr[uavri[att]] + 1));
		if (lb_pavr > uavr[att])
			return 0;
	}

	for (int att = 0; att < lavri.size(); att++){
		double act_pavr = (double)(_patt->avr[iter]->at(att)->at(par_pos) + tnod->attr[chil_ID_pos]->at(lavri[att])->at(0)) / (_patt->patt_seq.size() + 1);
		if (act_pavr >= lavr[att])
			continue;
		else
			satis = 2;
		double ub_pavr = (double)(_patt->avr[iter]->at(att)->at(par_pos) + tnod->attr[chil_ID_pos]->at(lavri[att])->at(num_minmax[lavri[att]] + num_avr[lavri[att]])) / (_patt->patt_seq.size() + tnod->attr[chil_ID_pos]->at(lavri[att])->at(num_minmax[lavri[att]] + 2 * num_avr[lavri[att]]));
		if (ub_pavr < lavr[att])
			return 0;
	}

	for (int i = 0; i < lmedi.size(); i++){
		if (tnod->attr[chil_ID_pos]->at(lmedi[i])->at(0) < lmed[i]){
			if (_patt->lmed[iter]->at(i)->at(par_pos)->at(0) - 1 > 0)
				continue;
			else{
				if (_patt->lmed[iter]->at(i)->at(par_pos)->at(0) - 1 == 0){
					int max_min;
					if (_patt->lmed[iter]->at(i)->at(par_pos)->at(1) > tnod->attr[chil_ID_pos]->at(lmedi[i])->at(0))
						max_min = _patt->lmed[iter]->at(i)->at(par_pos)->at(1);
					else
						max_min = tnod->attr[chil_ID_pos]->at(lmedi[i])->at(0);
					if (0.5 * (_patt->lmed[iter]->at(i)->at(par_pos)->at(2) + max_min) >= lmed[i])
						continue;
				}
				// algo only reaches here if constraint is infeasible when extended by tnod, thus cheking best case scenario in following lines
				if (_patt->lmed[iter]->at(i)->at(par_pos)->at(0) - 1 + tnod->attr[chil_ID_pos]->at(lmedi[i])->at(num_minmax[lmedi[i]] + num_avr[lmedi[i]] * 2 + 1) < 0)
					return 0;
				else if (_patt->lmed[iter]->at(i)->at(par_pos)->at(0) - 1 + tnod->attr[chil_ID_pos]->at(lmedi[i])->at(num_minmax[lmedi[i]] + num_avr[lmedi[i]] * 2 + 1) == 0){
					int max_min, min_max, max_patt2;
					if (_patt->lmed[iter]->at(i)->at(par_pos)->at(1) > tnod->attr[chil_ID_pos]->at(lmedi[i])->at(0))
						max_patt2 = _patt->lmed[iter]->at(i)->at(par_pos)->at(1);
					else
						max_patt2 = tnod->attr[chil_ID_pos]->at(lmedi[i])->at(0);

					if (max_patt2 > tnod->attr[chil_ID_pos]->at(lmedi[i])->at(num_minmax[lmedi[i]] + num_avr[lmedi[i]] * 2 + 2))
						max_min = max_patt2;
					else
						max_min = tnod->attr[chil_ID_pos]->at(lmedi[i])->at(num_minmax[lmedi[i]] + num_avr[lmedi[i]] * 2 + 2);
					if (_patt->lmed[iter]->at(i)->at(par_pos)->at(2) < tnod->attr[chil_ID_pos]->at(lmedi[i])->at(num_minmax[lmedi[i]] + num_avr[lmedi[i]] * 2 + 3))
						min_max = _patt->lmed[iter]->at(i)->at(par_pos)->at(2);
					else
						min_max = tnod->attr[chil_ID_pos]->at(lmedi[i])->at(num_minmax[lmedi[i]] + num_avr[lmedi[i]] * 2 + 3);

					if (0.5 * (min_max + max_min) < lmed[i])
						return 0;
				}
				satis = 2;
			}
		}
		else{
			if (_patt->lmed[iter]->at(i)->at(par_pos)->at(0) + 1 > 0)
				continue;
			else{  
				if (_patt->lmed[iter]->at(i)->at(par_pos)->at(0) + 1 == 0){
					int min_max;
					if (_patt->lmed[iter]->at(i)->at(par_pos)->at(2) < tnod->attr[chil_ID_pos]->at(lmedi[i])->at(0))
						min_max = _patt->lmed[iter]->at(i)->at(par_pos)->at(2);
					else
						min_max = tnod->attr[chil_ID_pos]->at(lmedi[i])->at(0);
					if (0.5 * (min_max + _patt->lmed[iter]->at(i)->at(par_pos)->at(1)) >= lmed[i])
						continue;
				}
				// algo only reaches here if constraint is infeasible when extended by tnod, thus cheking best case scenario in following lines
				if (_patt->lmed[iter]->at(i)->at(par_pos)->at(0) + 1 + tnod->attr[chil_ID_pos]->at(lmedi[i])->at(num_minmax[lmedi[i]] + num_avr[lmedi[i]] * 2 + 1) < 0)
					return 0;
				else if (_patt->lmed[iter]->at(i)->at(par_pos)->at(0) + 1 + tnod->attr[chil_ID_pos]->at(lmedi[i])->at(num_minmax[lmedi[i]] + num_avr[lmedi[i]] * 2 + 1) == 0){
					int max_min, min_max, min_patt3;
					if (_patt->lmed[iter]->at(i)->at(par_pos)->at(2) < tnod->attr[chil_ID_pos]->at(lmedi[i])->at(0))
						min_patt3 = _patt->lmed[iter]->at(i)->at(par_pos)->at(2);
					else
						min_patt3 = tnod->attr[chil_ID_pos]->at(lmedi[i])->at(0);
					if (_patt->lmed[iter]->at(i)->at(par_pos)->at(1) > tnod->attr[chil_ID_pos]->at(lmedi[i])->at(num_minmax[lmedi[i]] + num_avr[lmedi[i]] * 2 + 2))
						max_min = _patt->lmed[iter]->at(i)->at(par_pos)->at(1);
					else
						max_min = tnod->attr[chil_ID_pos]->at(lmedi[i])->at(num_minmax[lmedi[i]] + num_avr[lmedi[i]] * 2 + 2);
					if (min_patt3 < tnod->attr[chil_ID_pos]->at(lmedi[i])->at(num_minmax[lmedi[i]] + num_avr[lmedi[i]] * 2 + 3))
						min_max = min_patt3;
					else
						min_max = tnod->attr[chil_ID_pos]->at(lmedi[i])->at(num_minmax[lmedi[i]] + num_avr[lmedi[i]] * 2 + 3);

					if (0.5 * (min_max + max_min) < lmed[i])
						return 0;
				}
				satis = 2;
			}
		}		
	}

	for (int i = 0; i < umedi.size(); i++){
		if (tnod->attr[chil_ID_pos]->at(umedi[i])->at(0) > umed[i]){
			if (_patt->umed[iter]->at(i)->at(par_pos)->at(0) - 1 > 0)
				continue;
			else{
				if (_patt->umed[iter]->at(i)->at(par_pos)->at(0) - 1 == 0){
					int min_max;
					if (_patt->umed[iter]->at(i)->at(par_pos)->at(2) < tnod->attr[chil_ID_pos]->at(umedi[i])->at(0))
						min_max = _patt->umed[iter]->at(i)->at(par_pos)->at(2);
					else
						min_max = tnod->attr[chil_ID_pos]->at(umedi[i])->at(0);

					if (0.5 * (min_max + _patt->umed[iter]->at(i)->at(par_pos)->at(1)) <= umed[i])
						continue;
				}
				// algo only reaches here if constraint is infeasible when extended by tnod, thus cheking best case scenario in following lines
				if (_patt->umed[iter]->at(i)->at(par_pos)->at(0) -1 + tnod->attr[chil_ID_pos]->at(umedi[i])->at(num_minmax[umedi[i]] + num_avr[umedi[i]] * 2 + (num_med[umedi[i]] - 1) * 3 + 1) < 0)
					return 0;
				else if (_patt->umed[iter]->at(i)->at(par_pos)->at(0) -1 + tnod->attr[chil_ID_pos]->at(umedi[i])->at(num_minmax[umedi[i]] + num_avr[umedi[i]] * 2 + (num_med[umedi[i]] - 1) * 3 + 1) == 0){
					int max_min, min_max, min_patt3;
					if (_patt->umed[iter]->at(i)->at(par_pos)->at(2) < tnod->attr[chil_ID_pos]->at(umedi[i])->at(0))
						min_patt3 = _patt->umed[iter]->at(i)->at(par_pos)->at(2);
					else
						min_patt3 = tnod->attr[chil_ID_pos]->at(umedi[i])->at(0);

					if (_patt->umed[iter]->at(i)->at(par_pos)->at(1) > tnod->attr[chil_ID_pos]->at(umedi[i])->at(num_minmax[umedi[i]] + num_avr[umedi[i]] * 2 + (num_med[umedi[i]] - 1) * 3 + 2))
						max_min = _patt->umed[iter]->at(i)->at(par_pos)->at(1);
					else
						max_min = tnod->attr[chil_ID_pos]->at(umedi[i])->at(num_minmax[umedi[i]] + num_avr[umedi[i]] * 2 + (num_med[umedi[i]] - 1) * 3 + 2);
					if (min_patt3 < tnod->attr[chil_ID_pos]->at(umedi[i])->at(num_minmax[umedi[i]] + num_avr[umedi[i]] * 2 + (num_med[umedi[i]] - 1) * 3 + 3))
						min_max = min_patt3;
					else
						min_max = tnod->attr[chil_ID_pos]->at(umedi[i])->at(num_minmax[umedi[i]] + num_avr[umedi[i]] * 2 + (num_med[umedi[i]] - 1) * 3 + 3);

					if (0.5 * (min_max + max_min) > umed[i])
						return 0;
				}
				satis = 2;
			}
		}
		else{
			if (_patt->umed[iter]->at(i)->at(par_pos)->at(0) + 1 > 0)
				continue;
			else{   
				if (_patt->umed[iter]->at(i)->at(par_pos)->at(0) + 1 == 0){
					int max_min;
					if (_patt->umed[iter]->at(i)->at(par_pos)->at(1) > tnod->attr[chil_ID_pos]->at(umedi[i])->at(0))
						max_min = _patt->umed[iter]->at(i)->at(par_pos)->at(1);
					else
						max_min = tnod->attr[chil_ID_pos]->at(umedi[i])->at(0);

					if (0.5 * (_patt->umed[iter]->at(i)->at(par_pos)->at(2) + max_min) <= umed[i])
						continue;
				}
				// algo only reaches here if constraint is infeasible when extended by tnod, thus cheking best case scenario in following lines
				if (_patt->umed[iter]->at(i)->at(par_pos)->at(0) + 1 + tnod->attr[chil_ID_pos]->at(umedi[i])->at(num_minmax[umedi[i]] + num_avr[umedi[i]] * 2 + (num_med[umedi[i]] - 1) * 3 + 1) < 0)
					return 0;
				else if (_patt->umed[iter]->at(i)->at(par_pos)->at(0) + 1 + tnod->attr[chil_ID_pos]->at(umedi[i])->at(num_minmax[umedi[i]] + num_avr[umedi[i]] * 2 + (num_med[umedi[i]] - 1) * 3 + 1) == 0){
					int max_min, min_max, max_patt2;
					if (_patt->umed[iter]->at(i)->at(par_pos)->at(1) > tnod->attr[chil_ID_pos]->at(umedi[i])->at(0))
						max_patt2 = _patt->umed[iter]->at(i)->at(par_pos)->at(1);
					else
						max_patt2 = tnod->attr[chil_ID_pos]->at(umedi[i])->at(0);

					if (max_patt2 > tnod->attr[chil_ID_pos]->at(umedi[i])->at(num_minmax[umedi[i]] + num_avr[umedi[i]] * 2 + (num_med[umedi[i]] - 1) * 3 + 2))
						max_min = max_patt2;
					else
						max_min = tnod->attr[chil_ID_pos]->at(umedi[i])->at(num_minmax[umedi[i]] + num_avr[umedi[i]] * 2 + (num_med[umedi[i]] - 1) * 3 + 2);
					if (_patt->umed[iter]->at(i)->at(par_pos)->at(2) < tnod->attr[chil_ID_pos]->at(umedi[i])->at(num_minmax[umedi[i]] + num_avr[umedi[i]] * 2 + (num_med[umedi[i]] - 1) * 3 + 3))
						min_max = _patt->umed[iter]->at(i)->at(par_pos)->at(2);
					else
						min_max = tnod->attr[chil_ID_pos]->at(umedi[i])->at(num_minmax[umedi[i]] + num_avr[umedi[i]] * 2 + (num_med[umedi[i]] - 1) * 3 + 3);

					if (0.5 * (min_max + max_min) > umed[i])
						return 0;
				}
				satis = 2;
			}
		}		
	}

	if (satis == 2)
		return 2; 	
			
	return 1;
}


void Out_final_patt(vector<int>* seq, int freq) {								//Writes patterns to terminal or file

	ofstream file;

	if (b_disp)
		cout << "************** Found Pattern: freq :" << freq << " : ";
	if (b_write){
		file.open(out_file, std::ios::app);
		file << "************** Found Pattern: freq :" << freq << " : ";
	}

	for (int ii = 0; ii < seq->size(); ii++) {
		if (use_dic){
			map<int,string>::iterator it = item_map_rev.find((*seq)[ii]);
			if (b_disp)
				cout << it->second << " ";
			if (b_write)
				file << it->second << " ";
		}
		else{
			if (b_disp)
				cout << (*seq)[ii] << " ";
			if(b_write)
				file << (*seq)[ii] << " ";
		}
	}
	if (b_disp)
		cout << "\n";
	if (b_write){
		file << "\n";
		file.close();
	}
}




