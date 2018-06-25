#include "predict.h"

//func：按某字符分割字符串
//param: 
//s:			待分割字符串
//seperator：	分割字符
//return:
//result:       分割后的字符串集合
vector<string> string_split(const string &s, const string &seperator){
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()){
		//找到字符串中首个不等于分隔符的字母；
		int flag = 0;
		while (i != s.size() && flag == 0){
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
			if (s[i] == seperator[x]){
				++i;
				flag = 0;
				break;
			}
		}

		//找到又一个分隔符，将两个分隔符之间的字符串取出；
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0){
			for (string_size x = 0; x < seperator.size(); ++x)
			if (s[j] == seperator[x]){
				flag = 1;
				break;
			}
			if (flag == 0)
				++j;
		}
		if (i != j){
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}


//func：处理输入文件数据
//param:
//*info[MAX_INFO_NUM]:	输入文件数据（按行）
//info_num:				输入文件行数
//return:
//input_t:              输入文件handle
void handle_input(char* info[MAX_INFO_NUM], int info_num, input_t *input_handle)
{
	int nowline = 0;
	char* pnowline;
	vector<string> split_str;
	//读取物理服务器参数（第一行）
	pnowline = info[nowline++];
	split_str = string_split(pnowline, " ");
	input_handle->server.CPU = atoi(split_str[0].c_str());
	input_handle->server.MEM = atoi(split_str[1].c_str());
	nowline++;//空一行
	//读取虚拟机种类
	pnowline = info[nowline++];
	int flavor_num = atoi(&pnowline[0]);
	//#if 1
	//if (flavor_num >= 3) return;
	//#endif
	for (int i = 0; i < flavor_num; i++){
		//读取虚拟机属性保存到Flavor_t中
		Flavor_t tempFlavor;
		pnowline = info[nowline++];
		string nowline_str = pnowline;
		//去掉string末尾的换行符
		nowline_str.erase(remove(nowline_str.begin(), nowline_str.end(), '\n'), nowline_str.end());
		split_str = string_split(nowline_str, " ");
		tempFlavor.falvor_name = split_str[0];
		tempFlavor.cpu = atoi(split_str[1].c_str());
		tempFlavor.mem = atoi(split_str[2].c_str()) / 1024;
		input_handle->falvors.push_back(tempFlavor);
	}
	nowline++;//空一行
	pnowline = info[nowline++];
	//读取优化方式
	input_handle->option = ((pnowline[0] == 'C') || (pnowline[0] == 'c')) ? CPU : MEM;
	nowline++;//空一行
	//读取时间跨度
	pnowline = info[nowline++];
	string begin = pnowline;
	begin.erase(remove(begin.begin(), begin.end(), '\n'), begin.end());
	pnowline = info[nowline++];
	string end = pnowline;
	end.erase(remove(end.begin(), end.end(), '\n'), end.end());
	input_handle->daySpan = cheak_dateSpan(begin, end);
}



//func:处理训练文件数据
//param:
//readdata：		读取的训练数据(按行）
//lineMax：		训练数据行数
//return:		
//vector:		按天统计每个flavor数量
vector<flavor_t> handle_train(char * readdata[MAX_DATA_NUM], int LineMaxIndex)
{
	int DateSpan;
	int index = 0;
	vector<flavor_t> flav_vector;
	string line;
	string PrevDate = "";
	vector<string> sep_str;
	for (int j = 0; j < LineMaxIndex; j++){
		line = readdata[j];
		sep_str = string_split(line, "\t");
		string flavorN = sep_str[1].substr(6);
		string TempDate = sep_str[2].substr(0, 10);
		//cout << line;
		//cout << flavorN << " " << TempDate << endl;
		if (PrevDate != ""){
			DateSpan = cheak_dateSpan(PrevDate, TempDate);
		}
		else {
			DateSpan = 1;
		}
		if (PrevDate == "" || DateSpan != 0){
			flavor_t f = { 0 };
			int i = 0;
			while (i<DateSpan - 1){
				flav_vector.push_back(f);
				i++;
			}
			index = atoi(flavorN.c_str());
			if (index <= 15){
				f.flavor[index - 1]++;
				flav_vector.push_back(f);
			}
		}
		else if (cheak_dateSpan(PrevDate, TempDate) == 0){
			index = atoi(flavorN.c_str());
			if (index <= 15){
				int VectorSize = flav_vector.size();
				flav_vector[VectorSize - 1].flavor[index - 1]++;
			}
		}
		PrevDate = TempDate;
	}
	vector<flavor_t>::iterator iter = flav_vector.begin();
	flav_vector.erase(iter);
	flavor_t f = {0};
	flav_vector.push_back(f);
	return flav_vector;
}


int flavor2int(string flavorname){
	int result = 0;
	//string temp = "1";
	string temp = flavorname.substr(6);
	result = atoi(temp.c_str());
	return result;
}

//滑动平均
vector<double> mov_average(vector<double> list_) {
	int length = 3;
	double value;
	double sumvalue;
	float rate;
	value = list_[0];
	vector<double> newlist;
	rate = 1 / (float)length;
	newlist.push_back(value);
	value = 0.333*(list_[0] + list_[1] + list_[2]);
	newlist.push_back(value);
	int i;
	for (i = 2; i<list_.size() - 2; i++) {
		vector<double> va(&list_[i - length / 2], &list_[i + 1 + length / 2]);
		sumvalue = accumulate(va.begin(), va.end(), 0);
		
		value = rate * sumvalue;
		newlist.push_back(value);
	}
	vector<double> va(&list_[i - length / 2], &list_[i + length / 2]);
	vector<double>::iterator iter;
	for (double iter : va){
		//cout << iter;
		value += rate*(iter);
	}
	newlist.push_back(value);
	newlist.push_back(list_.back());
	return newlist;

}

void cheak_statistics(double *mean, double *var,vector<double> resultSet){
	double sum = accumulate(begin(resultSet), end(resultSet), 0.0);
	*mean = sum / resultSet.size(); //均值  

	double accum = 0.0;
	for_each(std::begin(resultSet), std::end(resultSet), [&](const double d) {
		accum += (d - *mean)*(d - *mean);
	});
	*var = accum / resultSet.size();
}

vector<double> replace_value(vector<double> data, double meanvalue){
	for (int i = 0; i < data.size(); i++)
		data[i] = abs(data[i] - meanvalue)>10 ? data[i]*0.5 : data[i];
	return data;
}
//

vector<double> replace_value2(vector<double> data, double meanvalue){
	//for (int i = 0; i < data.size(); i++)
	//	data[i] = abs(data[i] - meanvalue)>10 ? data[i] * 0.5 : data[i];
	vector<double> result;
	vector<double> ave_vec;
	double result_ave = 0.0;
	for (int i = 1; i < data.size() - 2; i++){
		double temp1 = abs(data[i + 1] - data[i]) + abs((data[i] - data[i - 1]));
		result_ave += temp1;
		vector<double> va(&data[i - 1], &data[i + 2]);
		double temp2 = accumulate(begin(va), end(va), 0.0);
		ave_vec.push_back(temp2/3.0);
		result.push_back(temp1);
	}
	result_ave = result_ave / (data.size() - 2);
	for (int i = 0; i < result.size(); i++){
		if (result[i] >= result_ave * 5)
			data[i + 1] = data[i + 1] >= (ave_vec[i] * 2) ? ave_vec[i] * 1.5 : data[i + 1];
	}
	return data;
}


//根据input过滤数据
vector<vector<double>> dataFilter(vector<Flavor_t> flavors, vector<flavor_t> orig_tra_data){
	int datalen = orig_tra_data.size();
	int inputlen = flavors.size();
	int flavorindex = 0;
	vector<vector<double>> result;
	for (int j = 1; j < 16; j++){
		if (flavorindex<inputlen){
			if (j == flavor2int(flavors[flavorindex].falvor_name)){
				vector<double> tempvec;
				for (int i = 0; i<datalen; i++){
					double temp = (double)orig_tra_data[i].flavor[j - 1];
					tempvec.push_back((double)orig_tra_data[i].flavor[j-1]);

				}
				//for (int i = 0; i < datalen; i++){
				//	cout << tempvec[i];
				//	if (i == datalen - 1) cout << "\n";
				//}
					
				result.push_back(tempvec);
			flavorindex++;
			}
			
		}

	}
	//    for (int i = 0;i < inputlen;i++){
	//        for (int j = 0;j < datalen;j++){
	//            int *OutVecnow = result[i];
	//            cout << OutVecnow[j]<< " ";
	//        }
	//
	//        cout << endl<<"date"<<i<<": ";
	//    }
	return result;
}

