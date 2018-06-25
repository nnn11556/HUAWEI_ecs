#include "predict.h"
//ʹ���ݶ��½��������Իع�


//template<class T>
////func:��ȡ���鳤��
////param:
////data:     ��������
////return:
////int:		s���鳤��
//static int length(T& data)
//{
//	return sizeof(data) / sizeof(data[0]);
//}



//func:����ÿ�εľ������
//param:
//b:		�ؾ�
//m:		б��
//*x:		x������
//*y:		y������
//length:   ���ݳ���
static double compute_error(double b, double m, vector<double> x, vector<double> y, int length)
{
	double totalError = 0.0;
	int data_length = length;
	for (int i = 0; i < data_length; i++) totalError += (y[i] - m*x[i] - b)*(y[i] - m*x[i] - b);
	return totalError / data_length;
}

//func:�����ݶ�ֵ
//param:
//current_b:		��ǰ�ع鷽���еĽؾ�
//current_m��		��ǰ�ع鷽���е�б��
//x��				x������
//y:				y������
//rate��			ѧϰ����
//length��			���ݳ���
//return:
//���ݶȷ�����µĺ��b��m
static void compute_gradient(double *current_b, double *current_m, vector<double> x, vector<double> y, double rate, int length)
{
	double N = length;
	double b_gradient = 0.0;
	double m_gradient = 0.0;
	double temp;
	for (int i = 0; i < N; i++){
		b_gradient += (-2 / N)*(y[i] - *current_m*x[i] - *current_b);
		temp = (-2 / N)*x[i] * (y[i] - *current_m*x[i] - *current_b);
		m_gradient += (-2 / N)*x[i]*(y[i] - *current_m*x[i] - *current_b);
	}
	*current_b = *current_b - (rate*b_gradient);
	*current_m = *current_m - (rate*m_gradient);
	
}


static void optimizer(vector<double> x, vector<double> y, double* start_b, double * start_m, double learning_rate, int num_iter, int length)
{
	//double b = *start_b;
	//double m = *start_m;
	double error = 0;
	while (num_iter){
		compute_gradient(start_b, start_m, x, y, learning_rate, length);
		error = ((num_iter--) % 50 == 1) ? compute_error(*start_b, *start_m, x, y, length) : error;
		if (num_iter % 100 == 0) printf("error:%f\n", error);
	}
	//*start_b = b;
	//*start_m = m;
}

//func�����Իع����ں���
//param:
//x:		x������
//y:		y������
//b:		�ؾ�
//m:		б��
//length:	���ݳ���
//return:
//���Իع�ֱ�߷��̵�b��m
extern void Linear_Regression(vector<double> x, vector<double> y, double *b, double *m, int length)
{
	//��ʼ����
	double learning_rate = 0.001;
	*b = 0.03;
	*m = 0.03;
	int num_iter = 500;
	//printf("%f,%f", x[0], x[1]);
	optimizer(x, y, b, m, learning_rate, num_iter, length);
}


//void main()
//{
//	double b = 1;
//	double m = 1;
//	double x[3] = { 1, 2, 3 };
//	double y[3] = { 1, 2, 3 };
//	Linear_Regression(x, y, &b, &m, length(x));
//	printf("%f--%f\n", b, m);
//}
