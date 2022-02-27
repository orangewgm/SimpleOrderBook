
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string>
#include <fstream>
#include<iostream>
#include<vector>
#include<map>
#include<queue>
#include<deque>
#include<set>
using namespace std;




struct Order {
	char order_type;
	int order_id;
	char exec_type;
	int exec_num;
	double exec_price;
};

struct Buyentry
{
	double price;
	deque<Order*> q;
};

struct Sellentry
{
	double price;
	deque<Order*> q;
};

struct se_less {
public:
	bool operator()(const Sellentry* se1, const Sellentry* se2) const{
		return se1->price < se2->price;
	}
};

struct be_greater {
public:
	bool operator()(const Buyentry* be1, const Buyentry* be2) const{
		return be1->price > be2->price;
	}
};

map<int, Order*> id_to_order;// ������ - ���� map�����ڼ��ٳ���
map<double, Buyentry*> price_to_buy; // �۸� - ��entry�����ڼӿ��¶����Ĳ���
map<double, Sellentry*> price_to_sell; // �۸� - ����entry�����ڼӿ��¶����Ĳ���
set<Buyentry*, be_greater> buyside;// ����
set<Sellentry*, se_less> sellside;// ����


void outputEntrys() {
	cout << "=================" << endl << "ASK" << endl;
	if (sellside.size() > 0) {
		for (auto it = sellside.rbegin(); it != sellside.rend(); ++it) {
			Sellentry entry = **it;
			if (entry.q.size() > 0 && entry.q.back()->exec_num > 0) {
				cout << entry.price << ":";
				for (auto rit = entry.q.rbegin(); rit != entry.q.rend(); ++rit)
				{
					if ((*rit)->exec_num <= 0) continue;
					cout << (*rit)->exec_num;
					cout << " ";
				}
			}
			cout << endl;
		}

	}
	else {
		cout << endl << endl;
	}

	cout << "------------" << endl;
	if (buyside.size() > 0) {
		if (buyside.size() > 0) {
			for (auto it = buyside.begin(); it != buyside.end(); ++it) {
				Buyentry entry = **it;
				if (entry.q.size() > 0 && entry.q.back()->exec_num > 0) {
					cout << entry.price << ":";
					for (auto rit = entry.q.rbegin(); rit != entry.q.rend(); ++rit)
					{	
						if ((*rit)->exec_num <= 0) continue;
						cout << (*rit)->exec_num;
						cout << " ";
					}
				}
				cout << endl;
			}
		}

	}
	else {
		cout << endl << endl;
	}


	cout << "BID" << endl << "=================" << endl;
}



void cancelOrder(Order* o) { //O(1) 
	auto ex_order = id_to_order.find(o->order_id);//���Ҹö���
	
	if (ex_order != id_to_order.end()) {//�ҵ�
		(*(ex_order->second)).exec_num -= o->exec_num;

		if ((*(ex_order->second)).exec_num <= 0) {//������������ڵ���������������map��ɾ��
			id_to_order.erase(o->order_id);
		}
	}
}


void updateBuy(Order* o) { // �������м۸����O(1���������¼۸�������O��nlogn�� 

	auto buyentry = price_to_buy.find(o->exec_price);
	if (buyentry != price_to_buy.end()) { //��Buyside �иü۸��Buyentry
		(*buyentry->second).q.push_front(o); // �ڸ�Buyentry�Ķ����м������order
	}
	else {
		Buyentry* newbuyentry = new Buyentry({ o->exec_price, deque<Order*>(1, o)}); //����һ����buyentry����order�����buyentry�Ķ�����
		buyside.insert(newbuyentry);//�����buyentry���뵽buyside
		price_to_buy[o->exec_price] = newbuyentry;//����price_to_buy ���map�����µ�buyentry���뵽���map

	}


	id_to_order[o->order_id] = o; //���� id_to_order ���map�����µ�order���뵽���map
}

void updateSell(Order* o) { // �������м۸������O(1���������¼۸���������O��nlogn��
	auto sellentry = price_to_sell.find(o->exec_price);
	if (sellentry != price_to_sell.end()) {
		(*sellentry->second).q.push_front(o);
		
	}
	else {
		Sellentry* newsellentry = new Sellentry({ o->exec_price,deque<Order*>(1,o) });
		sellside.insert(newsellentry);
		price_to_sell[o->exec_price] = newsellentry;
	}

	id_to_order[o->order_id] = o;

}


void tradeBuy(Order* o) { //���ף�������Ϊ��Ҫ�����¶��� O��nlogn��
	
	while (sellside.size() > 0 && o->exec_num>0) { //�������йҵ�ʱ
		auto firstSellEntry = *sellside.begin(); //�ó���������ͼ۹ҵ�entry
		if (o->exec_price < firstSellEntry->price || o->exec_num <= 0) break; //�޷����
		while (o->exec_price >= firstSellEntry->price && o->exec_num > 0) { //���Դ��
			while (firstSellEntry->q.size() > 0) {
				auto currExOrder = firstSellEntry->q.back();//�õ����ȼ�����еĶ���
				if (o->exec_num >= currExOrder->exec_num) { //�򵥵�������ڵ��ڹҵ�����
					o->exec_num -= currExOrder->exec_num;
					currExOrder->exec_num = 0; //�ҵ������������Ϊ0
					firstSellEntry->q.pop_back();//�ڶ����е��������� 
					id_to_order.erase(currExOrder->order_id);//���ڸ������Ѿ���գ���id_to_order ��ɾ����

				}
				else {
					currExOrder->exec_num -= o->exec_num; //���ٵ�ǰ����������
					o->exec_num = 0; //���������Ϊ0
					
					break;//���ټ������
				}
			}
		}
		if (firstSellEntry->q.size() <= 0) { //����ü۸�����ж������������ˣ����sellside�е�����sellentry
			sellside.erase(sellside.begin());
			price_to_sell.erase(firstSellEntry->price);//���ڸ�entry�Ѿ���ʧ����price_to_sell��ɾ��
		}
	}
	if (o->exec_num > 0) {//δ���������δ�ܴ��
		updateBuy(o);
	}



}

void tradeSell(Order* o) {//���ף���������Ϊ��Ҫ�����¶��� O��nlogn��
	while (buyside.size() > 0 && o->exec_num > 0) { //�����йҵ�ʱ
		auto firstBuyEntry = *buyside.begin(); //�ó��򷽵���߼۹ҵ�entry
		if (o->exec_price > firstBuyEntry->price || o->exec_num <= 0) break; //�޷����
		while (o->exec_price <= firstBuyEntry->price && o->exec_num > 0) { //���Դ��
			while (firstBuyEntry->q.size() > 0) {
				auto currExOrder = firstBuyEntry->q.back();//�õ����ȼ�����еĶ���
				if (o->exec_num >= currExOrder->exec_num) { //�򵥵�������ڵ��ڹҵ�����
					o->exec_num -= currExOrder->exec_num;
					currExOrder->exec_num = 0; //�ҵ����������Ϊ0
					firstBuyEntry->q.pop_back();//�ڶ����е������� 
					id_to_order.erase(currExOrder->order_id);//���ڸ����Ѿ���գ���id_to_order ��ɾ����

				}
				else {
					currExOrder->exec_num -= o->exec_num; //���ٵ�ǰ�ҵ��򵥵�����
					o->exec_num = 0; //�����������Ϊ0
					break;//���ټ������
				}
			}
		}
		if (firstBuyEntry->q.size() <= 0) { //����ü۸�����ж������������ˣ����buyside��ɾ����buyentry
			buyside.erase(buyside.begin());//ɾ����entry
			price_to_buy.erase(firstBuyEntry->price);//���ڸ�entry�Ѿ���ʧ����price_to_buy��ɾ��
		}
	}
	if (o->exec_num > 0) {//δ���������δ�ܴ��
		updateSell(o);
	}



}


void updateOder(Order* o){
	Order order = *o;
	if (order.order_type == 'A') {
		if (order.exec_type == 'B') {
			tradeBuy(o);
		}
		else {
			tradeSell(o);
		}
	}
	else if (order.order_type == 'X') {
		cancelOrder(o);
	}
}












int order_book() {
	vector<Order*> order_list;
	
	FILE* fp = 0;
	fp = fopen(".\\test.txt", "r");
	char order_type;
	int order_id;
	char exec_type;
	int exec_num;
	double exec_price;

	cout << "Given order sequence:" << endl;
	while (fscanf(fp, "%c,%d,%c,%d,%lf\n", &order_type, &order_id, &exec_type, &exec_num, &exec_price) != EOF) {
		Order* o = new Order({ order_type,order_id,exec_type,exec_num,exec_price });
		cout << o->order_type << " " << o->order_id << " " << o->exec_type << " " << o->exec_num << " " << o->exec_price<<endl;
		order_list.push_back(o);
	}


	cout << "Results after executing each entire order:" << endl;

	for (int i = 0; i < order_list.size(); i++) {
		cout << "After executing order" << i + 1 <<": "<< order_list[i]->order_type << " "<< order_list[i]->order_id << " " << 
			order_list[i]->exec_type << " " 
			<< order_list[i]->exec_num << " " << order_list[i]->exec_price << endl;
		updateOder(order_list[i]);
		outputEntrys();
		cout << endl;
	}
	

	
	
	return 0;
}

