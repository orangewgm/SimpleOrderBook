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

	int pa = 0; //指向原始
	int pb = 0; //指向目标
	vector<Order> res;
	while (pa < a.size() || pb < b.size()) { //O(n)时间复杂度
		int i = pa;
		while (pa < a.size() && pb < b.size() && a[pa].exec_type != b[pb].exec_type) { //找到买卖相同的位置
			pa++;
		}
		if (i != pa) {
			int i = pa - 1;
			while (i >= 0 && a[i].exec_type != b[pb].exec_type) {//买卖不同，撤单
				Order* xo = new Order({ 'X',a[pa].order_id,a[pa].exec_type,a[pa].exec_num,a[pa].exec_price });
				res.push_back(*xo);
				i--;
			}
		}
		while (pa < a.size() && pb < b.size() && a[pa].exec_type == b[pb].exec_type) { //买卖相同
			if (a[pa].exec_price == b[pb].exec_price) {//价格相同
				if (a[pa].exec_num != b[pb].exec_num) {//数量不同
					Order* mo = new Order({ 'M',a[pa].order_id,a[pa].exec_type,b[pb].exec_num,a[pa].exec_price });
					res.push_back(*mo);
				}
				pa++;
				pb++;
			}
			else {//价格不同,撤单
				Order* xo = new Order({ 'X',a[pa].order_id,a[pa].exec_type,a[pa].exec_num,a[pa].exec_price });
				res.push_back(*xo);
				pa++;
			}
		}
		//pa走到了尽头
		if (pa >= a.size()){
			while (pb < b.size()) { //加单
				Order* ao = new Order({ 'A',fake_order_num--,b[pb].exec_type,b[pb].exec_num,b[pb].exec_price });
				res.push_back(*ao);
				pb++;
			}
		}
		//pa未到尽头，但此时买卖不同
		else {
			while (pb < b.size() && a[pa].exec_type != b[pb].exec_type) {//加单
				Order* ao = new Order({ 'A',fake_order_num--,b[pb].exec_type,b[pb].exec_num,b[pb].exec_price });
				res.push_back(*ao);
				pb++;
			}
		}

		//pb走到了尽头,代表target已经满足
		if (pb >= b.size()) {
			while (pa < a.size()) {//撤单
				Order* xo = new Order({ 'X',a[pa].order_id,a[pa].exec_type,a[pa].exec_num,a[pa].exec_price });
				res.push_back(*xo);
				pa++;
			}
		}

		//pb未走到尽头，但此时买卖不同
		else{
			while (pa < a.size() && a[pa].exec_type != b[pb].exec_type) {//撤单
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