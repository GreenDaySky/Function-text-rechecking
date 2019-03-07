#define _CRT_SECURE_NO_WARNINGS 1
#include "TextSimilarity.h"
#include <fstream>
#include <string>
#include <iostream>
#include <windows.h>
#include <algorithm>
#include <assert.h>

using namespace std;

TextSimilarity::TextSimilarity(string dict)
	:DICT(dict)
	, DICT_PATH(dict + "/jieba.dict.utf8")
	, HMM_PATH(dict + "/hmm_model.utf8")
	, USER_DICT_PATH(dict + "/user.dict.utf8")
	, IDF_PATH(dict + "/idf.utf8")
	, STOP_WORD_PATH(dict + "/stop_words.utf8")
	, _jieba(DICT_PATH,
		HMM_PATH,
		USER_DICT_PATH,
		IDF_PATH,
		STOP_WORD_PATH)
	, _maxWordNumber(10)
{
	getStopWordTable(STOP_WORD_PATH.c_str());
}


TextSimilarity::wordFreq TextSimilarity::getWordFreq(const char* filename)
{
	ifstream fin(filename);
	if (!fin.is_open())
	{
		cout << "open file:" << filename << "failed";
		return wordFreq();
	}

	string line;
	wordFreq wf;
	while (!fin.eof())
	{
		getline(fin, line);
		// GBK--> UTF8
		line = GBKtoUTF8(line);
		vector<string> words;
		//对文本进行分词
		_jieba.Cut(line, words, true);
		//统计词频
		for (const auto& e : words)
		{
			//去掉停用词
			if (_stopWordSet.count(e) > 0)
				continue;
			//统计词频
			else
			{
				if (wf.count(e) > 0)
					wf[e]++;
				else
					wf[e] = 1;
			}
		}
	}
	return wf;
}

string TextSimilarity::UTF8toGBK(string s)
{
	int len1 = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, 0, 0);
	WCHAR* str1 = new WCHAR[len1];
	len1 = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, str1, len1);
	if (len1 <= 0)cout << "转码失败";

	int len2 = WideCharToMultiByte(CP_ACP, 0, str1, -1, NULL, 0, NULL, FALSE);
	CHAR* str2 = new CHAR[len2];
	len2 = WideCharToMultiByte(CP_ACP, 0, str1, -1, str2, len2, NULL, FALSE);
	if (len2 <= 0)cout << "转码失败";
	
	string  OutString(str2);

	if (str1)
	{
		delete[] str1;
		str1 = NULL;
	}
	if (str2)
	{
		delete[] str2;
		str2 = NULL;
	}

	return OutString;
}

string TextSimilarity::GBKtoUTF8(string s)
{
	int len1 = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, 0, 0);
	WCHAR* str1 = new WCHAR[len1];
	len1 = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, str1, len1);
	if (len1 <= 0)cout << "转码失败";

	int len2 = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, FALSE);
	CHAR* str2 = new CHAR[len2];
	len2 = WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, len2, NULL, FALSE);
	if (len2 <= 0)cout << "转码失败";


	string  OutString(str2);

	if (str1)
	{
		delete[] str1;
		str1 = NULL;
	}
	if (str2)
	{
		delete[] str2;
		str2 = NULL;
	}

	return OutString;
}

void TextSimilarity::getStopWordTable(const char* stopWordFile)
{
	ifstream fin(stopWordFile);
	if (!fin.is_open())
	{
		cout << "open file:" << stopWordFile << "failed";
		return;
	}

	string line;
	while (!fin.eof())
	{
		getline(fin, line);

		//UTF8		1
		_stopWordSet.insert(line);
	}
	fin.close();
}

//???? 按词频大小 从大到小进行逆序排列
bool cmpReverse(pair<string, int> lp, pair<string, int> rp)
{
	return lp.second > rp.second;
}

vector<pair<string, int>> TextSimilarity::sortByValueReverse(TextSimilarity::wordFreq& wf)
{
	//unordered_map 
	//map是kv结构的无法直接使用sort进行排序
	//sort只支持序列容器
	vector<pair<string, int>> wfvector(wf.begin(), wf.end());
	sort(wfvector.begin(), wfvector.end(), cmpReverse);
	return wfvector;
}

void TextSimilarity::selectAimWords(std::vector<std::pair<std::string, int>>& wfvec, wordSet& wset)
{
	int len = wfvec.size();
	int sz = len > _maxWordNumber ? _maxWordNumber : len;
	for (int i = 0; i < sz; i++)
	{
		//pair<string, int>
		wset.insert(wfvec[i].first);
	}
}

vector<double>	TextSimilarity::getVectorQuantity(TextSimilarity::wordSet& wset, TextSimilarity::wordFreq& wf)
{
	//遍历wordSet中的每一个词
	vector<double>  VectorQuantity;
	for (const auto& e : wset)
	{
		if (wf.count(e))
			// VectorQuantity(value)
			 VectorQuantity.push_back(wf[e]);
		else
			 VectorQuantity.push_back(0);
	}
	return  VectorQuantity;
}


double TextSimilarity::cosine(std::vector<double>  VectorQuantity1, std::vector<double>  VectorQuantity2)
{
	double modular1 = 0, modular2 = 0;
	double products = 0;
	assert( VectorQuantity1.size() ==  VectorQuantity2.size());

	for (int i = 0; i <  VectorQuantity1.size(); i++)
	{
		//点积
		products +=  VectorQuantity1[i] *  VectorQuantity2[i];
	}

	for (int i = 0; i <  VectorQuantity1.size(); i++)
	{
		//模
		modular1 += pow( VectorQuantity1[i], 2);
	}
	modular1 = pow(modular1, 0.5);
	for (int i = 0; i <  VectorQuantity2.size(); i++)
	{
		//模
		modular2 += pow( VectorQuantity1[i], 2);
	}
	modular2 = pow(modular2, 0.5);

	return products / (modular1 * modular2);
}







