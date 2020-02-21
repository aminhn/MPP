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

//Utility functions, find_ID: binary search to find sequence ID in a node, give_time: Gives Machine time

#include "utility.hpp"
#include "build_mdd.hpp"
#include <iostream>

int find_ID(int ID, vector<int>* vec) {
	int l = 0;
	int u = vec->size()-1;

	while (l <= u) {
		int m = l + (u - l) / 2;

		if (vec->at(m) == ID)
			return m;

		if (vec->at(m) < ID)
			l = m + 1;
		else
			u = m - 1;
	}
	return -1;
}


double give_time(clock_t kk) {
	double ll = ((double)kk) / CLOCKS_PER_SEC;
	return ll;
}
