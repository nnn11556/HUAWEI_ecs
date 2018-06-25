#include "predict.h"


//#define MAXDPNUM 800
//
//auto res = new double[MAXDPNUM][MAXDPNUM][MAXDPNUM];

input_t input_handle = {0};

int sum_value(double* data){
	int sumval = 0;
	for (int i = 0; i < 15; i++){
		sumval += (int)data[i];
	}
	return sumval;
}


vector<double> create_x(int length){
	vector<double> x;
	for (int i = 0; i < length; i++)
		x.push_back(i/(double)length);
	return x;
}

void predict_ecs(vector<vector<double>> traindata, double* predict,int length,int span)
{	
	vector<double> x = create_x(length);
	for (int j = 0; j < traindata.size(); j++){
		double b = 0;
		double m = 0;
		Linear_Regression(x, traindata[j], &b, &m, length);
		//vector<double>::iterator iter;
		//for (iter = traindata[j].end() - span + 1; iter < traindata[j].end(); iter++){
		//	predict[j] += *iter;
		//}
		for (int i = 0; i < span; i++){
			double temp = x.back();
			predict[j] += (x.back() + i + 1)*m + b;
		}

		predict[j] = round(predict[j]);
	}
}

void make_result(char* result_file, unordered_map<int, map<string, int>> map_result, 
				double* predict, vector<Flavor_t> input_flavors){
	//虚拟机数量
	int sumval = sum_value(predict);
	strcat(result_file,to_string(sumval).c_str());
	strcat(result_file, "\n");
	for (int i = 0; i < input_flavors.size(); i++){
		strcat(result_file, input_flavors[i].falvor_name.c_str());
		strcat(result_file, " ");
		strcat(result_file, to_string((int)predict[i]).c_str());
		strcat(result_file, "\n");
	}
	strcat(result_file, "\n");
	strcat(result_file, to_string(map_result.size()).c_str());
	strcat(result_file, "\n");
	unordered_map<int, map<string, int>>::iterator iter;
	map<string, int>::iterator iter2;
	map<string, int>::iterator iter3;
	//for (iter = map_result.begin(); iter != map_result.end(); iter++){
	for (int i = 1; i <= map_result.size();i++){
		map<string, int> temp = map_result[i];
		strcat(result_file, to_string(i).c_str());
		strcat(result_file, " ");
		iter3 = temp.end();
		iter3--;
		/*iter3--;*/
		for (iter2 = temp.begin(); iter2 != temp.end(); iter2++){
			strcat(result_file, iter2->first.c_str());
			strcat(result_file, " ");
			strcat(result_file, to_string(iter2->second).c_str());
			if (iter2 != iter3) strcat(result_file, " ");
		}
		strcat(result_file, "\n");
	}

}

char result_file[MAX_DATA_NUM];

//你要完成的功能总入口
void predict_server(char * info[MAX_INFO_NUM], int info_num, char * data[MAX_DATA_NUM], int data_num, char * filename)
{
	double predict[15] = { 0 };
	handle_input(info, info_num, &input_handle);
	//#if 1
	//if (input_handle.falvors.size () != 3) return;
	//#endif
	vector<flavor_t> trainVector = handle_train(data, data_num);
	vector<vector<double>> traindata = dataFilter(input_handle.falvors, trainVector);
	//vector<vector<double>> average_Data;
	//double mean_value = 0.0;
	//double var_value = 0.0;
	//for (int i = 0; i < traindata.size(); i++){
	//	vector<double> temp = traindata[i];
	//	//cheak_statistics(&mean_value, &var_value, temp);
	//	temp = replace_value2(temp, mean_value);
	//	//if (var_value > 100) 
	//	//	temp = mov_average(temp);	
	//	average_Data.push_back(temp);
	//}
	predict_ecs(traindata, predict, trainVector.size(), input_handle.daySpan);
	for (int i = 0; i < traindata.size(); i++){
		cout << (i + 1) << ":";
		cout << predict[i] << endl;
	}
	unordered_map<int, map<string, int>> map_result = put_flavors(predict, input_handle);
	// 需要输出的内容
	make_result(result_file, map_result, predict, input_handle.falvors);

	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(result_file, filename);
}
