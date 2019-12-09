/*
---------------------------------------------------------------------------------------------------------------------------------- 
Copyright 2019 Amin Hosseininasab
This file is part of MPP mining algorithm.
MPP is a free software, and a GNU General Public License is pending.
----------------------------------------------------------------------------------------------------------------------------------
*/


#pragma once

#include "pattern.hpp"
#include "node_mdd.hpp"
#include "load_inst.hpp"

void Freq_miner();

extern vector<Pattern*> DFS_queue;				//DFS queue of potential patterns to extend

extern int num_patt;
extern int num_max_patt;
