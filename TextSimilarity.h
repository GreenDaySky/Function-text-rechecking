#pragma once
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <cppjieba/jieba.hpp>

class TextSimilarity
{
public:
	typedef std::unordered_map<std::string, int> wordFreq;
	typedef std::unordered_set<std::string> wordSet;
	TextSimilarity(std::string dict);
	wordFreq getWordFreq(const char* filename);
	std::string UTF8toGBK(std::string s);
	std::string GBKtoUTF8(std::string s);
	std::vector<std::pair<std::string, int>> sortByValueReverse(wordFreq& wf);
	void selectAimWords(std::vector<std::pair<std::string, int>>& wfvec, wordSet& wset);
	std::vector<double> getVectorQuantity(wordSet& wset, wordFreq& wf);
	double cosine(std::vector<double>  VectorQuantity1, std::vector<double>  VectorQuantity2);

private:
	void getStopWordTable(const char* stopWordFile);
	std::string DICT;
	std::string DICT_PATH;
	std::string HMM_PATH;
	std::string USER_DICT_PATH;
	std::string IDF_PATH;
	std::string STOP_WORD_PATH;
	cppjieba::Jieba _jieba;
	wordSet _stopWordSet;
	int _maxWordNumber;
};