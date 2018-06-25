#include "predict.h"

//#include <string>
//#include <vector>
//#include <algorithm>
//#include <iostream>

////如果关心的资源是CPU则为true，如果关心的资源是内存则为False
//#define CPU true 
//#define MEM false

using namespace std;

///使用模拟退火算法找最佳的虚拟机放置方式
///输入参数：
///map_predict_num_flavors：上一步预测出来的各种虚拟机数量，key是虚拟机名称，value是虚拟机数量
///map_flavor_cpu_mem：程序输入的虚拟机类型数据，key为虚拟机名称，value是虚拟机类型（包括name，cpu，mem字段）
///server_mem && server_cpu：程序输入的服务器参数，CPU和内存大小
///CPUorMEM：是使CPU利用率最高还是使内存利用率最高
///输出参数：
///res_servers：存放有计算出的最优服务器中虚拟机存放方式，可通过成员flavors访问每个服务器中存放的虚拟机
vector<Server> put_flavors_to_servers(unordered_map<string, int> map_predict_num_flavors, 
	unordered_map<string, Flavor> map_flavor_cpu_mem,int server_mem, int server_cpu, bool CPUorMEM) {

	vector<Flavor> vec_flavors;    //vector用于存放所有预测出来的flavor
	for (auto element : map_predict_num_flavors) {
		//将预测出来的所有虚拟机都加入到vec_flavors中
		while (element.second-- != 0) {
			vec_flavors.push_back(map_flavor_cpu_mem[element.first]);
		}
	}

	//=========================================================================
	//模拟退火算法找最优解
	double min_server = vec_flavors.size() + 1;
	vector<Server> res_servers;  //用于存放最好结果（服务器使用数量最少）
	double T = 100.0;  //模拟退火初始温度
	double Tmin = 1;   //模拟退火终止温度
	double r = 0.999;  //温度下降系数
	vector<int> dice;  //骰子，每次随机投掷，取vector前两个变量作为每次退火需要交换顺序的虚拟机
	for (int i = 0; i < vec_flavors.size(); i++) {
		dice.push_back(i);
	}
	while (T > Tmin) {
		//cout << T <<endl;
		//投掷骰子，如vector前两个数为3和9，则把vec_flavors[3]和vec_flavors[9]进行交换作为新的flavors顺序
		std::random_shuffle(dice.begin(), dice.end());
		auto new_vec_flavors = vec_flavors;
		std::swap(new_vec_flavors[dice[0]], new_vec_flavors[dice[1]]);

		//把上一步计算出来的虚拟机尝试加入到服务器中

		//先使用一个服务器用于放置虚拟机
		vector<Server> servers;
		Server firstserver(server_mem, server_cpu);
		servers.push_back(firstserver);  
		
		//放置虚拟机主要逻辑
		//如果当前所有服务器都放不下虚拟机，就新建一个服务器用于存放
		for (auto element : new_vec_flavors) {
			auto iter = servers.begin();
			for (; iter != servers.end(); ++iter) {
				if (iter->put_flavor(element)) {
					break;
				}
			}
			if (iter == servers.end()) {
				Server newserver(server_mem, server_cpu);
				newserver.put_flavor(element);
				servers.push_back(newserver);
			}
		}

		//计算本次放置虚拟机耗费服务器评价分数(double型)
		//如果使用了N个服务器，则前N-1个服务器贡献分数为1，第N个服务器分数为资源利用率
		//模拟退火就是得到取得分数最小的放置方式
		double server_num;
		//对于题目关心CPU还是MEM，需要分开讨论，资源利用率计算方法不同
		if (CPUorMEM == true)
			server_num = servers.size() - 1 + servers.rbegin()->get_cpu_usage_rate();
		else
			server_num = servers.size() - 1 + servers.rbegin()->get_mem_usage_rate();
		//如果分数更低，则保存结果
		if (server_num < min_server) {
			min_server = server_num;
			res_servers = servers;
			vec_flavors = new_vec_flavors;
		}
		//如果分数更高，则以一定概率保存结果，防止优化陷入局部最优解
		else {
			if (exp((min_server - server_num) / T) > rand() / RAND_MAX) {
				min_server = server_num;
				res_servers = servers;
				vec_flavors = new_vec_flavors;
			}
		}
		T = r * T;  //一次循环结束，温度降低
	}
	return res_servers;
}

map<string,int> count_flavor(Server servers)
{
	map<string, int> map_server_flavor;
	for (auto flavor : servers.flavors) {
		if (map_server_flavor.find(flavor.name) != map_server_flavor.end())
			map_server_flavor[flavor.name]++;
		else map_server_flavor[flavor.name] = 1;
	}
	return map_server_flavor;
}

vector<Server> test_put_flavors_to_servers(unordered_map<string, int> map_predict_num_flavors,
	unordered_map<string, Flavor> map_flavor_cpu_mem, int server_mem, int server_cpu, bool CPUorMEM, input_t input)
{
	
	vector<Flavor> vec_flavors;    //vector用于存放所有预测出来的flavor
	for (int i = input.falvors.size()-1 ; i >= 0; i--){
		int num = map_predict_num_flavors[input.falvors[i].falvor_name];
		while (num-- != 0){
			vec_flavors.push_back(map_flavor_cpu_mem[input.falvors[i].falvor_name]);
		}
	}


	//auto new_vec_flavors = vec_flavors;
	//先使用一个服务器用于放置虚拟机
	vector<Server> servers;
	while (vec_flavors.size() > 0){

		vector <vector<vector<int>>>res;
		for (int i = 0; i <= vec_flavors.size(); i++){
			vector<vector<int>> temp1;
			for (int j = 0; j <= server_cpu; j++){
				vector<int> temp2;
				for (int k = 0; k <= server_mem; k++){
					temp2.push_back(0);
				}
				temp1.push_back(temp2);
			}
			res.push_back(temp1);
		}
		Server firstserver(server_mem, server_cpu);
		//servers.push_back(firstserver);
		for (int i = 1; i <= vec_flavors.size(); i++){
			for (int j = 1; j <= server_cpu; j++){
				for (int k = 1; k <= server_mem; k++){
					int temp1 = vec_flavors[i - 1].cpu;
					int temp2 = vec_flavors[i - 1].mem;
					int value = CPUorMEM == true ? temp1 : temp2;
					if ((temp1 <= j) && (temp2 <= k)){
						res[i][j][k] = max(res[i - 1][j][k], res[i - 1][j - temp1][k - temp2] + value);
					}
					else{
						res[i][j][k] = res[i - 1][j][k];
					}
				}
			}
		}
		int tempcpu = server_cpu;
		if(vec_flavors.size() <= 20){
			int a = 1;
		}
		int tempmem = server_mem;
		int n = vec_flavors.size();
		//vector<int> earsemap;
		for (int i = n; i > 1; i--){
			if (res[i][tempcpu][tempmem] == res[i - 1][tempcpu][tempmem]){

			}
			else{
				firstserver.put_flavor(vec_flavors[i - 1]);
				tempcpu -= vec_flavors[i-1].cpu;
				tempmem -= vec_flavors[i-1].mem;
			}
		}
		if (res[1][tempcpu][tempmem] != 0){
			firstserver.put_flavor(vec_flavors[0]);
		}
		//delete element
		vector<Flavor>::iterator iter;
		int index = 0;
		for (auto element : firstserver.flavors){
			for (iter = vec_flavors.begin(); iter != vec_flavors.end();){
				if (element.name == iter->name){
					iter = vec_flavors.erase(iter);
					break;
				}
				else{
					iter++;
				}
			}
		}
		//for (iter = vec_flavors.begin(); iter != vec_flavors.end();){
		//	index = 0;
		//	for (auto element : firstserver.flavors){
		//		if (element.name == iter->name){
		//			iter = vec_flavors.erase(iter);
		//			index = 1;
		//			break;
		//		}
		//	}
		//	if(index == 0) iter++;
		//}
		servers.push_back(firstserver);
	}
	//auto new_vec_flavors = vec_flavors;
	////先使用一个服务器用于放置虚拟机
	//vector<Server> servers;
	//Server firstserver(server_mem, server_cpu);
	//servers.push_back(firstserver);

	////放置虚拟机主要逻辑
	////如果当前所有服务器都放不下虚拟机，就新建一个服务器用于存放
	//for (auto element : new_vec_flavors) {
	//	auto iter = servers.begin();
	//	for (; iter != servers.end(); ++iter) {
	//		if (iter->put_flavor(element)) {
	//			break;
	//		}
	//	}
	//	if (iter == servers.end()) {
	//		Server newserver(server_mem, server_cpu);
	//		newserver.put_flavor(element);
	//		servers.push_back(newserver);
	//	}
	//}
	return servers;
}





unordered_map<int, map<string, int>> test_put(double* predict, input_t input)
{
	unordered_map<string, int> map_predict_num_flavors;
	unordered_map<string, Flavor> map_flavor_cpu_mem;
	for (int i = 0; i < input.falvors.size(); i++){
		map_predict_num_flavors[input.falvors[i].falvor_name] = predict[i];
		map_flavor_cpu_mem[input.falvors[i].falvor_name] =
			Flavor(input.falvors[i].falvor_name, input.falvors[i].mem, input.falvors[i].cpu);
	}
	int server_cpu = input.server.CPU;
	int server_mem = input.server.MEM;
	bool CPUorMEM = input.option == CPU ? true : false;
	vector<Server> servers = test_put_flavors_to_servers(map_predict_num_flavors, map_flavor_cpu_mem,
		server_mem, server_cpu, CPUorMEM, input);
	unordered_map<int, map<string, int>> put_result;
	for (int i = 1; i <= servers.size(); i++){
		put_result[i] = count_flavor(servers[i - 1]);
	}
	return put_result;
}


unordered_map<int, map<string, int>> put_flavors(double* predict, input_t input)
{
	unordered_map<string, int> map_predict_num_flavors;
	unordered_map<string, Flavor> map_flavor_cpu_mem;
	for (int i = 0; i < input.falvors.size(); i++){
		//map_predict_num_flavors[input.falvors[i].falvor_name] = predict[i];
		map_predict_num_flavors[input.falvors[i].falvor_name] = 5;
		map_flavor_cpu_mem[input.falvors[i].falvor_name] = 
			Flavor(input.falvors[i].falvor_name,input.falvors[i].mem,input.falvors[i].cpu);
	}
	int server_cpu = input.server.CPU;
	int server_mem = input.server.MEM;
	bool CPUorMEM = input.option == CPU ? true : false;
	vector<Server> servers = test_put_flavors_to_servers(map_predict_num_flavors, map_flavor_cpu_mem,
		server_mem, server_cpu, CPUorMEM,input);
	unordered_map<int, map<string, int>> put_result;
	for (int i = 1; i <= servers.size(); i++){
		put_result[i] = count_flavor(servers[i-1]);
	}
	return put_result;
}



//
//int main() {
//	//程序运用例子
//	//预测出来的各种虚拟机数量
//	printf("hello\n");
//	unordered_map<string, int> map_predict_num_flavors;
//	map_predict_num_flavors["flavor1"] = 30;
//	map_predict_num_flavors["flavor2"] = 40;
//	map_predict_num_flavors["flavor3"] = 20;
//	map_predict_num_flavors["flavor4"] = 10;
//	map_predict_num_flavors["flavor5"] = 25;
//
//	//各种虚拟机参数
//	unordered_map<string, Flavor> map_flavor_cpu_mem;
//	map_flavor_cpu_mem["flavor1"] = Flavor("flavor1", 1, 1);
//	map_flavor_cpu_mem["flavor2"] = Flavor("flavor2", 1, 2);
//	map_flavor_cpu_mem["flavor3"] = Flavor("flavor3", 1, 4);
//	map_flavor_cpu_mem["flavor4"] = Flavor("flavor4", 2, 2);
//	map_flavor_cpu_mem["flavor5"] = Flavor("flavor5", 2, 4);
//
//	//服务器资源相关信息
//	int server_cpu = 56;
//	int server_mem = 128;
//	bool CPUorMEM = MEM;
//	//调用模拟退火算法找到最优放置方法
//	//cout << "11111";
//	vector<Server> servers = put_flavors_to_servers(map_predict_num_flavors, map_flavor_cpu_mem, server_cpu, server_mem, CPUorMEM);
//	//输出各个服务器里面分别放置了哪些虚拟机
//	/*cout << "22222";*/
//	int server_count = 1;
//	for (auto element : servers) {
//		cout << server_count++ << "\n";
//		for (auto flavor : element.flavors) {
//			std::cout << flavor.name << " " << 1 << " ";
//		}
//		cout << "\n";
//		cout << "CPU:" << element.get_cpu_usage_rate() << "  MEM:" << element.get_mem_usage_rate() << "\n";
//		cout << endl;
//	}
//	return 0;
//}