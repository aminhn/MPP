/*
---------------------------------------------------------------------------------------------------------------------------------- 
Copyright 2019 Amin Hosseininasab
This file is part of MPP mining algorithm.
MPP is a free software, and a GNU General Public License is pending.
----------------------------------------------------------------------------------------------------------------------------------
*/

#include<vector>
#include <iostream>
#include "load_inst.hpp"
#include "build_mdd.hpp"

void Popl_nodes();							
void Intlz_DFS(int ID, Node* fnod, Node* tnod);				
void Add_arc(int ID, int strp, int endp);			
void Intlz_node(int nod);						
bool Check_gap(int i, int strt, int endp);				

vector<Node*> datab_MDD;					
vector<Pattern*> DFS_queue;						

void Build_MDD() {

	datab_MDD = vector<Node*>(M * L, NULL);
	DFS_queue = vector<Pattern*>(L, NULL);

	Popl_nodes();

}


void Popl_nodes() {							

	bool antmon = 0;						
	for (int i = 0; i < N; i++) {
		int endp = items[i]->size();				
		int strp = endp - 1;					
		while (strp > 0) {
			while (antmon == 0) {				
				if (!ugap.empty() && ugapi[0] == 0 && attrs[0]->at(i)->at(endp - 1) - attrs[0]->at(i)->at(strp - 1) > ugap[0]) {		
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
			if (antmon == 1) {				
				int last_p = endp;
				while (endp != strp) {
					if (!lgap.empty() && lgapi[0] == 0 && attrs[0]->at(i)->at(endp - 1) - attrs[0]->at(i)->at(strp - 1) < lgap[0])
						break;
					if (tot_gap.empty() || (tot_gap[0] == 0 && tot_gap.size() == 1) || Check_gap(i ,strp, endp))
						Add_arc(i, strp, endp);
					endp--;
				}
				strp--;
				if (!ugap.empty())			
					antmon = 0;
				endp = last_p;
			}
		}
	}
}


bool Check_gap(int i, int strp, int endp){				

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


void Add_arc(int ID, int strp, int endp) {						

	int fnod = items[ID]->at(strp - 1) + (strp - 1) * L;
	int tnod = items[ID]->at(endp - 1) + (endp - 1) * L;

	Intlz_node(fnod - 1);
	Intlz_node(tnod - 1);

	datab_MDD[tnod - 1]->assign_ID(ID + 1, endp, NULL);				
	datab_MDD[fnod - 1]->assign_ID(ID + 1, strp, datab_MDD[tnod - 1]);

	Intlz_DFS(ID, datab_MDD[fnod - 1], datab_MDD[tnod - 1]);			
}


void Intlz_node(int nod) {								
	if (datab_MDD[nod] == NULL) {	
		datab_MDD[nod] = new Node();
		datab_MDD[nod]->ID = nod + 1;
	}
}


void Intlz_DFS(int ID, Node* fnod, Node* tnod) {					
											
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



