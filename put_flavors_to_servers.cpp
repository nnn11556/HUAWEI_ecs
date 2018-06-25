#include "predict.h"

//#include <string>
//#include <vector>
//#include <algorithm>
//#include <iostream>

////������ĵ���Դ��CPU��Ϊtrue��������ĵ���Դ���ڴ���ΪFalse
//#define CPU true 
//#define MEM false

using namespace std;

///ʹ��ģ���˻��㷨����ѵ���������÷�ʽ
///���������
///map_predict_num_flavors����һ��Ԥ������ĸ��������������key����������ƣ�value�����������
///map_flavor_cpu_mem�����������������������ݣ�keyΪ��������ƣ�value����������ͣ�����name��cpu��mem�ֶΣ�
///server_mem && server_cpu����������ķ�����������CPU���ڴ��С
///CPUorMEM����ʹCPU��������߻���ʹ�ڴ����������
///���������
///res_servers������м���������ŷ��������������ŷ�ʽ����ͨ����Աflavors����ÿ���������д�ŵ������
vector<Server> put_flavors_to_servers(unordered_map<string, int> map_predict_num_flavors, 
	unordered_map<string, Flavor> map_flavor_cpu_mem,int server_mem, int server_cpu, bool CPUorMEM) {

	vector<Flavor> vec_flavors;    //vector���ڴ������Ԥ�������flavor
	for (auto element : map_predict_num_flavors) {
		//��Ԥ���������������������뵽vec_flavors��
		while (element.second-- != 0) {
			vec_flavors.push_back(map_flavor_cpu_mem[element.first]);
		}
	}

	//=========================================================================
	//ģ���˻��㷨�����Ž�
	double min_server = vec_flavors.size() + 1;
	vector<Server> res_servers;  //���ڴ����ý����������ʹ���������٣�
	double T = 100.0;  //ģ���˻��ʼ�¶�
	double Tmin = 1;   //ģ���˻���ֹ�¶�
	double r = 0.999;  //�¶��½�ϵ��
	vector<int> dice;  //���ӣ�ÿ�����Ͷ����ȡvectorǰ����������Ϊÿ���˻���Ҫ����˳��������
	for (int i = 0; i < vec_flavors.size(); i++) {
		dice.push_back(i);
	}
	while (T > Tmin) {
		//cout << T <<endl;
		//Ͷ�����ӣ���vectorǰ������Ϊ3��9�����vec_flavors[3]��vec_flavors[9]���н�����Ϊ�µ�flavors˳��
		std::random_shuffle(dice.begin(), dice.end());
		auto new_vec_flavors = vec_flavors;
		std::swap(new_vec_flavors[dice[0]], new_vec_flavors[dice[1]]);

		//����һ�������������������Լ��뵽��������

		//��ʹ��һ�����������ڷ��������
		vector<Server> servers;
		Server firstserver(server_mem, server_cpu);
		servers.push_back(firstserver);  
		
		//�����������Ҫ�߼�
		//�����ǰ���з��������Ų�������������½�һ�����������ڴ��
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

		//���㱾�η���������ķѷ��������۷���(double��)
		//���ʹ����N������������ǰN-1�����������׷���Ϊ1����N������������Ϊ��Դ������
		//ģ���˻���ǵõ�ȡ�÷�����С�ķ��÷�ʽ
		double server_num;
		//������Ŀ����CPU����MEM����Ҫ�ֿ����ۣ���Դ�����ʼ��㷽����ͬ
		if (CPUorMEM == true)
			server_num = servers.size() - 1 + servers.rbegin()->get_cpu_usage_rate();
		else
			server_num = servers.size() - 1 + servers.rbegin()->get_mem_usage_rate();
		//����������ͣ��򱣴���
		if (server_num < min_server) {
			min_server = server_num;
			res_servers = servers;
			vec_flavors = new_vec_flavors;
		}
		//����������ߣ�����һ�����ʱ���������ֹ�Ż�����ֲ����Ž�
		else {
			if (exp((min_server - server_num) / T) > rand() / RAND_MAX) {
				min_server = server_num;
				res_servers = servers;
				vec_flavors = new_vec_flavors;
			}
		}
		T = r * T;  //һ��ѭ���������¶Ƚ���
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
	
	vector<Flavor> vec_flavors;    //vector���ڴ������Ԥ�������flavor
	for (int i = input.falvors.size()-1 ; i >= 0; i--){
		int num = map_predict_num_flavors[input.falvors[i].falvor_name];
		while (num-- != 0){
			vec_flavors.push_back(map_flavor_cpu_mem[input.falvors[i].falvor_name]);
		}
	}


	//auto new_vec_flavors = vec_flavors;
	//��ʹ��һ�����������ڷ��������
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
	////��ʹ��һ�����������ڷ��������
	//vector<Server> servers;
	//Server firstserver(server_mem, server_cpu);
	//servers.push_back(firstserver);

	////�����������Ҫ�߼�
	////�����ǰ���з��������Ų�������������½�һ�����������ڴ��
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
//	//������������
//	//Ԥ������ĸ������������
//	printf("hello\n");
//	unordered_map<string, int> map_predict_num_flavors;
//	map_predict_num_flavors["flavor1"] = 30;
//	map_predict_num_flavors["flavor2"] = 40;
//	map_predict_num_flavors["flavor3"] = 20;
//	map_predict_num_flavors["flavor4"] = 10;
//	map_predict_num_flavors["flavor5"] = 25;
//
//	//�������������
//	unordered_map<string, Flavor> map_flavor_cpu_mem;
//	map_flavor_cpu_mem["flavor1"] = Flavor("flavor1", 1, 1);
//	map_flavor_cpu_mem["flavor2"] = Flavor("flavor2", 1, 2);
//	map_flavor_cpu_mem["flavor3"] = Flavor("flavor3", 1, 4);
//	map_flavor_cpu_mem["flavor4"] = Flavor("flavor4", 2, 2);
//	map_flavor_cpu_mem["flavor5"] = Flavor("flavor5", 2, 4);
//
//	//��������Դ�����Ϣ
//	int server_cpu = 56;
//	int server_mem = 128;
//	bool CPUorMEM = MEM;
//	//����ģ���˻��㷨�ҵ����ŷ��÷���
//	//cout << "11111";
//	vector<Server> servers = put_flavors_to_servers(map_predict_num_flavors, map_flavor_cpu_mem, server_cpu, server_mem, CPUorMEM);
//	//�����������������ֱ��������Щ�����
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