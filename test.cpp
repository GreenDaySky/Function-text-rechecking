#define _CRT_SECURE_NO_WARNINGS 1
#include "TextSimilarity.h"
#include <iostream>
using namespace std;

void testVectorQuantity()
{
	TextSimilarity ts("dict");
	TextSimilarity::wordFreq wf = ts.getWordFreq("test1.txt");
	TextSimilarity::wordFreq wf2 = ts.getWordFreq("test2.txt");

	vector<pair<string, int>> wfvec = ts.sortByValueReverse(wf);
	vector<pair<string, int>> wfvec2 = ts.sortByValueReverse(wf2);

	cout << "wfvect" << endl;
	for (int i = 0; i < 10; i++)
	{
		cout << ts.UTF8toGBK(wfvec[i].first) << ":" << wfvec[i].second << "\t";

	}
	cout << endl;

	cout << "wfvect2" << endl;
	for (int i = 0; i < 10; i++)
	{
		cout << ts.UTF8toGBK(wfvec2[i].first) << ":" << wfvec2[i].second << "\t";

	}
	cout << endl;

	TextSimilarity::wordSet wset;
	//用排好序的词频挑选前n个候选词
	ts.selectAimWords(wfvec, wset);
	ts.selectAimWords(wfvec2, wset);


	cout << "wset" << endl;

	for (const auto& e : wset)
	{
		cout << ts.UTF8toGBK(e) << " " ;
	}
	cout << endl;
	//根据候选词构建词频向量
	vector<double>  VectorQuantity = ts.getVectorQuantity(wset, wf);
	vector<double>  VectorQuantity2 = ts.getVectorQuantity(wset, wf2);

	cout << " VectorQuantity" << endl;
	for (const auto& v :  VectorQuantity)
	{
		cout << v << " ";
	}
	cout << endl;

	cout << " VectorQuantity2" << endl;
	for (const auto& v :  VectorQuantity2)
	{
		cout << v << " ";
	}
	cout << endl;

	cout << "cosine: " << ts.cosine( VectorQuantity,  VectorQuantity2) << endl;
}

int main()
{
	testVectorQuantity();
	system("pause");
	return 0;
}