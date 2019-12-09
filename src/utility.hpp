
/*
---------------------------------------------------------------------------------------------------------------------------------- 
Copyright 2019 Amin Hosseininasab
This file is part of MPP mining algorithm.
MPP is a free software, and a GNU General Public License is pending.
----------------------------------------------------------------------------------------------------------------------------------
*/


#pragma once

#include <vector>
#include <time.h>
#include <string>

using namespace std;

int find_ID(int ID, vector<int>* vec);
void Delete_obj(const string& obj);
double give_time(clock_t kk);