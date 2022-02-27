#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <iostream>
#include <set>
#include <algorithm>
#include <iterator>
#include<vector>

using namespace std;

struct Order {
	char order_type;
	int order_id;
	char exec_type;
	int exec_num;
	double exec_price;
	
};






int modify() {
	FILE* fp = 0;
	fp = fopen(".\\original-test.txt", "r"); 
	char order_type;
	int order_id;
	char exec_type;
	int exec_num;
	double exec_price;

	int fake_order_num = 999999999;

	vector<Order> a,b;

	cout << "Current set of outstanding orders: " << endl;
	while (fscanf(fp, "%c,%d,%c,%d,%lf\n", &order_type, &order_id, &exec_type, &exec_num, &exec_price) != EOF) {
		Order* o = new Order({ order_type,order_id,exec_type,exec_num,exec_price });
		cout << o->order_type << " " << o->order_id << " " << o->exec_type << " " << o->exec_num << " " << o->exec_price << endl;
		a.push_back(*o);
		
	}


	fp = fopen(".\\target.txt", "r");

	cout << "Desired set of outstanding orders:" << endl;
	while (fscanf(fp, "%c,%d,%c,%d,%lf\n", &order_type, &order_id, &exec_type, &exec_num, &exec_price) != EOF) {
		Order* o = new Order({ order_type,order_id,exec_type,exec_num,exec_price });
		cout << o->order_type << " " << o->order_id << " " << o->exec_type << " " << o->exec_num << " " << o->exec_price << endl;
		b.push_back(*o);
	}

	int pa = 0; //ָ��ԭʼ
	int pb = 0; //ָ��Ŀ��
	vector<Order> res;
	while (pa < a.size() || pb < b.size()) { //O(n)ʱ�临�Ӷ�
		int i = pa;
		while (pa < a.size() && pb < b.size() && a[pa].exec_type != b[pb].exec_type) { //�ҵ�������ͬ��λ��
			pa++;
		}
		if (i != pa) {
			int i = pa - 1;
			while (i >= 0 && a[i].exec_type != b[pb].exec_type) {//������ͬ������
				Order* xo = new Order({ 'X',a[pa].order_id,a[pa].exec_type,a[pa].exec_num,a[pa].exec_price });
				res.push_back(*xo);
				i--;
			}
		}
		while (pa < a.size() && pb < b.size() && a[pa].exec_type == b[pb].exec_type) { //������ͬ
			if (a[pa].exec_price == b[pb].exec_price) {//�۸���ͬ
				if (a[pa].exec_num != b[pb].exec_num) {//������ͬ
					Order* mo = new Order({ 'M',a[pa].order_id,a[pa].exec_type,b[pb].exec_num,a[pa].exec_price });
					res.push_back(*mo);
				}
				pa++;
				pb++;
			}
			else {//�۸�ͬ,����
				Order* xo = new Order({ 'X',a[pa].order_id,a[pa].exec_type,a[pa].exec_num,a[pa].exec_price });
				res.push_back(*xo);
				pa++;
			}
		}
		//pa�ߵ��˾�ͷ
		if (pa >= a.size()){
			while (pb < b.size()) { //�ӵ�
				Order* ao = new Order({ 'A',fake_order_num--,b[pb].exec_type,b[pb].exec_num,b[pb].exec_price });
				res.push_back(*ao);
				pb++;
			}
		}
		//paδ����ͷ������ʱ������ͬ
		else {
			while (pb < b.size() && a[pa].exec_type != b[pb].exec_type) {//�ӵ�
				Order* ao = new Order({ 'A',fake_order_num--,b[pb].exec_type,b[pb].exec_num,b[pb].exec_price });
				res.push_back(*ao);
				pb++;
			}
		}

		//pb�ߵ��˾�ͷ,����target�Ѿ�����
		if (pb >= b.size()) {
			while (pa < a.size()) {//����
				Order* xo = new Order({ 'X',a[pa].order_id,a[pa].exec_type,a[pa].exec_num,a[pa].exec_price });
				res.push_back(*xo);
				pa++;
			}
		}

		//pbδ�ߵ���ͷ������ʱ������ͬ
		else{
			while (pa < a.size() && a[pa].exec_type != b[pb].exec_type) {//����
				Order* xo = new Order({ 'X',a[pa].order_id,a[pa].exec_type,a[pa].exec_num,a[pa].exec_price });
				res.push_back(*xo);
				pa++;
			}
		}
	}

	cout << "Results:" << endl;
	for (int i = 0;i<res.size();i++) {
		cout << res[i].order_type << "," << res[i].order_id << "," << res[i].exec_type << "," << res[i].exec_num
			<< "," << res[i].exec_price << endl;
	}
	return 0;
}