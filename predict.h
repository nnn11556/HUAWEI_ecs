#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "lib_io.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include <cstring>
#include <string>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <map>
#include <numeric>
using namespace std;

//test



//���ݽṹ
typedef struct Server_t{		//���������
	int CPU;					//cpu ����
	int MEM;					//mem G
}Server_t;

typedef struct Flavor_t{		//���������
	string falvor_name;			//�������
	int cpu;					// cpu����
	int mem;					// mem G
}Flavor_t;

typedef enum Opt_way{			//�Ż���ʽ
	CPU = 1,
	MEM = 2
}Opt_way;


typedef struct input_t{			//�����ļ�
	Server_t server;			//�������������
	vector<Flavor_t> falvors;	//��ҪԤ������������
	Opt_way option;				//�Ż���ʽ
	int daySpan;				//Ԥ��ʱ���ȣ�����ƣ�
}input_t;

struct flavor_t{					//ͳ��ÿ��flavor����
	int flavor[15];
};

//Flavor�࣬�����������Ϣ������ �ڴ� CPU
struct Flavor {
	std::string name;
	int mem;
	int cpu;
	Flavor(std::string _name, int _mem, int _cpu) :
		name(_name), mem(_mem), cpu(_cpu){}
	Flavor() {

	}
};

typedef struct predict_flavor{
	string flavor_name;
	int num;
}predict_flavor;

//Server�࣬���������������Ϣ�����ڴ� ��CPU �����ڴ� ����CPU �Ѵ�ŵ�������б�
class Server {
public:
	std::vector<Flavor> flavors;  //����������Ѵ��������б�
	///Server���캯��������Ϊ�ڴ��С��CPU��С
	Server(int mem, int cpu);
	///�������������������Ϊ��������󣬷���ֵΪ�Ƿ���óɹ�
	bool put_flavor(Flavor flavor);
	///��ȡ������CPUʹ����
	double get_cpu_usage_rate();
	///��ȡ�������ڴ�ʹ����
	double get_mem_usage_rate();
private:
	int total_mem;  // ������������ڴ�
	int total_cpu;  // �����������CPU
	int free_mem;   // ���������ʣ������ڴ�
	int free_cpu;   // ���������ʣ�����CPU
};
//���ݽṹend


void predict_server(char * info[MAX_INFO_NUM], int info_num, char * data[MAX_DATA_NUM], int data_num, char * filename);

vector<string> string_split(const string &s, const string &seperator);//�ַ����ָ��
int cheak_dateSpan(string begin, string end);//����ʱ����
void handle_input(char* info[MAX_INFO_NUM], int info_num, input_t *input_handle);//���������ļ�����
vector<flavor_t> handle_train(char * readdata[MAX_DATA_NUM], int LineMaxIndex);//����ѵ���ļ�����
vector<vector<double>> dataFilter(vector<Flavor_t> flavors, vector<flavor_t> orig_tra_data);//���˲���ҪԤ�������

void Linear_Regression(vector<double> x, vector<double> y, double *b, double *m, int length);//���Իع飨ʹ���ݶ��½�ʵ�֣�

unordered_map<int, map<string, int>> put_flavors(double* predict, input_t input);//����������������������ַ�ʽ һ�ֻ���ģ���˻�ʵ�� ��
unordered_map<int, map<string, int>> test_put(double* predict, input_t input);//һ�����ǽ����״���ӦFFD

vector<double> mov_average(vector<double> list_);//����ƽ�� �������3�㻬��ƽ��
void cheak_statistics(double *mean, double *var, vector<double> resultSet);
vector<double> replace_value(vector<double> data, double meanvalue);
vector<double> replace_value2(vector<double> data, double meanvalue);
#endif
