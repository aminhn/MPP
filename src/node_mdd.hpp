/*
---------------------------------------------------------------------------------------------------------------------------------- 
Copyright 2019 Amin Hosseininasab
This file is part of MPP mining algorithm.
MPP is a free software, and a GNU General Public License is pending.
----------------------------------------------------------------------------------------------------------------------------------
*/


#pragma once

#include<vector>
#include "load_inst.hpp"

using namespace std;

class Node {

public:

	int ID;									//Node number in graph
	int parent;								//Used to determine whether node has parent w.r.t to a Seq ID
	int item;

	vector<int> seq_ID;						//Vector which keeps the sequences associated to node
	vector<vector<vector<int>*>*> attr;				//Vector of critical information, one row per sequence, per attribute, columns: 0:actual, min:1, max:size_maxmin, min_sum:size_maxmin + 1, max_sum:size_maxmin + size_sum, num(avr):last
	vector<vector<Node*>*> children;

	void assign_ID(int ID, int lvl, Node* tnod);			//Creates or updates the node information

	Node() { ID = 0; parent = 0; item = 0; }			//Node constructor

	~Node() {
		for (int i = 0; i < children.size(); i++){
			delete children[i];
		} 
	}

};

