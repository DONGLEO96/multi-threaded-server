#pragma once
#include<vector>
#include<string>
#include<iostream>
using namespace std;
vector<string> spilt(string s, char c)
{
	int i = 0;
	int j = s.find(c);
	vector<string> res;
	while (j != string::npos)
	{
		res.push_back(string(s.begin() + i, s.begin() + j));
		i = j + 1;
		j = s.find(c, i);
	}
	res.push_back(string(s.begin() + i, s.end()));
	return res;
}
vector<char*> spilt(char* start, int index, char c)
{
	vector<char*> res;
	res.push_back(start);
	for (int i = 0; i < index; ++i)
	{
		if (*(start + i) == c)
		{
			*(start + i) = '\0';
			res.push_back(start + i + 1);
		}
	}
	*(start + index - 1) = '\0';
	return res;
}