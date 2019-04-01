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



//数据结构
typedef struct Server_t{		//物理服务器
	int CPU;					//cpu 核数
	int MEM;					//mem G
}Server_t;

typedef struct Flavor_t{		//虚拟服务器
	string falvor_name;			//虚拟机名
	int cpu;					// cpu核数
	int mem;					// mem G
}Flavor_t;

typedef enum Opt_way{			//优化方式
	CPU = 1,
	MEM = 2
}Opt_way;


typedef struct input_t{			//输入文件
	Server_t server;			//物理服务器参数
	vector<Flavor_t> falvors;	//需要预测的虚拟机参数
	Opt_way option;				//优化方式
	int daySpan;				//预测时间跨度（按天计）
}input_t;

struct flavor_t{					//统计每天flavor个数
	int flavor[15];
};

//Flavor类，描述虚拟机信息：名称 内存 CPU
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

//Server类，描述物理服务器信息：总内存 总CPU 可用内存 可用CPU 已存放的虚拟机列表
class Server {
public:
	std::vector<Flavor> flavors;  //物理服务器已存放虚拟机列表
	///Server构造函数，参数为内存大小和CPU大小
	Server(int mem, int cpu);
	///放置虚拟机函数，参数为虚拟机对象，返回值为是否放置成功
	bool put_flavor(Flavor flavor);
	///获取服务器CPU使用率
	double get_cpu_usage_rate();
	///获取服务器内存使用率
	double get_mem_usage_rate();
private:
	int total_mem;  // 物理服务器总内存
	int total_cpu;  // 物理服务器总CPU
	int free_mem;   // 物理服务器剩余可用内存
	int free_cpu;   // 物理服务器剩余可用CPU
};
//数据结构end


void predict_server(char * info[MAX_INFO_NUM], int info_num, char * data[MAX_DATA_NUM], int data_num, char * filename);

vector<string> string_split(const string &s, const string &seperator);//字符串分割函数
int cheak_dateSpan(string begin, string end);//计算时间间隔
void handle_input(char* info[MAX_INFO_NUM], int info_num, input_t *input_handle);//处理输入文件数据
vector<flavor_t> handle_train(char * readdata[MAX_DATA_NUM], int LineMaxIndex);//处理训练文件数据
vector<vector<double>> dataFilter(vector<Flavor_t> flavors, vector<flavor_t> orig_tra_data);//过滤不需要预测的数据

void Linear_Regression(vector<double> x, vector<double> y, double *b, double *m, int length);//线性回归（使用梯度下降实现）

unordered_map<int, map<string, int>> put_flavors(double* predict, input_t input);//放置虚拟机（这里用了两种方式 一种基于模拟退火实现 ）
unordered_map<int, map<string, int>> test_put(double* predict, input_t input);//一种则是降序首次适应FFD

vector<double> mov_average(vector<double> list_);//滑动平均 这里采用3点滑动平均
void cheak_statistics(double *mean, double *var, vector<double> resultSet);
vector<double> replace_value(vector<double> data, double meanvalue);
vector<double> replace_value2(vector<double> data, double meanvalue);
#endif
