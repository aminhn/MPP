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
#include "node_mdd.hpp"
#include "pattern.hpp"
#include "freq_miner.hpp"
#include <cmath>


void Build_MDD();

void Disp_nodes(int, int);

extern vector<Node*> datab_MDD;			//The vector defining the MDD database

