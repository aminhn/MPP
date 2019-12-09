/*
---------------------------------------------------------------------------------------------------------------------------------- 
Copyright 2019 Amin Hosseininasab
This file is part of MPP mining algorithm.
MPP is a free software, and a GNU General Public License is pending.
----------------------------------------------------------------------------------------------------------------------------------
*/


#pragma once

#include<vector>
#include "node_mdd.hpp"
#include "pattern.hpp"
#include "freq_miner.hpp"
#include <cmath>


void Build_MDD();

void Disp_nodes(int, int);

extern vector<Node*> datab_MDD;			//The vector defining the MDD database

