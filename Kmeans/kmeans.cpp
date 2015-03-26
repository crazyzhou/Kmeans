#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <time.h>
#define K 3
#define THRESHOLD 0.01
using namespace std;

void GenerateFirstCentroids(unordered_map<string, vector<short>> dic, vector<vector<double>> &cen, vector<string> stk)
{
	int len = stk.size();
	int a[K] = { 0 };
	srand(unsigned(time(NULL)));
	int temp;
	for (int i = 0; i < K; i++)
	{
		bool flag = true;
		while (flag)
		{
			int j;
			temp = rand() % len;//产生随机数
			for (j = 0; j < i; j++)
			{
				if (a[j] == temp)
				{
					flag = true;
					break;
				}
			}
			if (j == i) flag = false;
		}
		a[i] = temp;
		vector<short> vec = dic[stk[temp]];
		int len2 = vec.size();
		vector<double> vecD;
		for (int j = 0; j < len2; j++)
		{
			vecD.push_back((double)vec[j]);
		}
		cen.push_back(vecD);
	}
	return;
}

double EuclideanDistanceDD(vector<double> a, vector<double> b)
{
	int len = a.size();
	if (len != b.size())
		return -1;
	double ret = 0;
	for (int i = 0; i < len; i++)
	{
		ret += (a[i] - b[i])*(a[i] - b[i]);
	}
	return sqrt(ret);
}

double EuclideanDistanceSD(vector<short> a, vector<double> b)
{
	int len = a.size();
	if (len != b.size())
		return -1;
	double ret = 0;
	for (int i = 0; i < len; i++)
	{
		ret += ((double)a[i] - b[i])*((double)a[i] - b[i]);
	}
	return sqrt(ret);
}
double CosineSimilaritySD(vector<short> a, vector<double> b)
{
	int len = a.size();
	if (len != b.size())
		return -1;
	double ret = 0, am = 0, bm = 0;
	for (int i = 0; i < len; i++)
	{
		ret += (double)a[i] * b[i];
		am += (double)a[i] * (double)a[i];
		bm += b[i] * b[i];
	}
	return ret / (am*bm);
}
double ManhattanDistanceSD(vector<short> a, vector<double> b)
{
	int len = a.size();
	if (len != b.size())
		return -1;
	double ret = 0;
	for (int i = 0; i < len; i++)
	{
		ret += abs((double)a[i] - b[i]);
	}
	return ret;
}

void binstr2vector(string s, vector<short>&v)
{
	int len = s.size();
	for (int i = 0; i < len; i++)
	{
		v.push_back((s[i] - '0'));
	}
	return;
}
int checkCluster(vector<short> v, vector<vector<double>> centroids)
{
	double theta = DBL_MIN;
	int ret = 0;
	for (int i = 0; i < K; i++)
	{
		if (CosineSimilaritySD(v, centroids[i]) > theta)
		{
			ret = i;
			theta = CosineSimilaritySD(v, centroids[i]);
		}
	}
	return ret;
}
double GenerateNewCentroids(unordered_map<string, int> clu, vector<vector<double>> &cen, unordered_map<string, vector<short>> dic)
{
	double ret = 0;
	int count[K] = { 0 };
	int dim = cen[0].size();
	vector<vector<double>> oldCen = cen;
	for (int i = 0; i < K; i++)
	{
		for (int j = 0; j < dim; j++)
		{
			cen[i][j] = 0;
		}
	}
	for (pair<string, int> p : clu)
	{
		count[p.second]++;
		for (int i = 0; i < dim; i++)
		{
			cen[p.second][i] += dic[p.first][i];
		}
	}
	for (int i = 0; i < K; i++)
	{
		for (int j = 0; j < dim; j++)
		{
			cen[i][j] = cen[i][j] / count[i];
		}
		ret += EuclideanDistanceDD(oldCen[i], cen[i]);
	}
	return ret;
}

int main()
{
	unordered_map<string, int> stockCluster;
	vector<string> stk;
	unordered_map<string, vector<short>> stockDic;
	vector<vector<double>> centroids;
	double err;
	int shit = 0;
	string line;
	ifstream fin("F:\\StockVector.txt",ios::in);
	while (getline(fin, line))
	{
		int i = line.find(' ');
		string stockName = line.substr(0, i);
		stk.push_back(stockName);
		string stockVec = line.substr(i + 1);
		vector<short> vec;
		binstr2vector(stockVec, vec);
		stockDic[stockName] = vec;
	}
	fin.close();
	cout << "read over!" << endl;
	GenerateFirstCentroids(stockDic, centroids, stk);
	shit = 0;
	do
	{
		cout << shit++ << endl;
		for (pair<string, vector<short>> p : stockDic)
		{
			int clusterID = checkCluster(p.second, centroids);
			stockCluster[p.first] = clusterID;
		}
		err = GenerateNewCentroids(stockCluster, centroids, stockDic);
		cout << err << endl;
	} while (err > THRESHOLD);
	int count[K] = { 0 };
	int dim = centroids[0].size();
	for (pair<string, int> p : stockCluster)
	{
		count[p.second]++;
	}
	for (int i = 0; i < K; i++)
	{
		cout << count[i] << endl;
	}
	ofstream fout("F:\\StockResult.txt", ios::out);
	for (int i = 0; i < K; i++)
	{
		fout << i << ' ' << count[i]<< endl;
		for (int j = 0; j < dim; j++)
		{
			fout << centroids[i][j] << ' ';
		}
	}
	fout << endl;
	for (pair<string, int> p : stockCluster)
	{
		fout << p.first << " " << p.second << endl;
	}
	fout.close();
	cout << "OK";
	cin >> shit;
	return 0;
}