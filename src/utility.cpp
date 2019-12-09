/*
---------------------------------------------------------------------------------------------------------------------------------- 
Copyright 2019 Amin Hosseininasab
This file is part of MPP mining algorithm.
MPP is a free software, and a GNU General Public License is pending.
----------------------------------------------------------------------------------------------------------------------------------
*/

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
