
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

map<int, Order*> id_to_order;// 订单号 - 订单 map，用于加速撤单
map<double, Buyentry*> price_to_buy; // 价格 - 买方entry，用于加快新订单的插入
map<double, Sellentry*> price_to_sell; // 价格 - 卖方entry，用于加快新订单的插入
set<Buyentry*, be_greater> buyside;// 买盘
set<Sellentry*, se_less> sellside;// 卖盘


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
	auto ex_order = id_to_order.find(o->order_id);//查找该订单
	
	if (ex_order != id_to_order.end()) {//找到
		(*(ex_order->second)).exec_num -= o->exec_num;

		if ((*(ex_order->second)).exec_num <= 0) {//如果撤销量大于等于现有量，则在map中删除
			id_to_order.erase(o->order_id);
		}
	}
}


void updateBuy(Order* o) { // 插入已有价格的买单O(1），插入新价格的买单最差O（nlogn） 

	auto buyentry = price_to_buy.find(o->exec_price);
	if (buyentry != price_to_buy.end()) { //在Buyside 有该价格的Buyentry
		(*buyentry->second).q.push_front(o); // 在该Buyentry的队列中加入这个order
	}
	else {
		Buyentry* newbuyentry = new Buyentry({ o->exec_price, deque<Order*>(1, o)}); //构造一个新buyentry并将order插入该buyentry的队列中
		buyside.insert(newbuyentry);//将这个buyentry插入到buyside
		price_to_buy[o->exec_price] = newbuyentry;//更新price_to_buy 这个map，将新的buyentry插入到这个map

	}


	id_to_order[o->order_id] = o; //更新 id_to_order 这个map，将新的order插入到这个map
}

void updateSell(Order* o) { // 插入已有价格的卖单O(1），插入新价格的卖单最差O（nlogn）
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


void tradeBuy(Order* o) { //交易，买，最坏情况为需要插入新订单 O（nlogn）
	
	while (sellside.size() > 0 && o->exec_num>0) { //当卖方有挂单时
		auto firstSellEntry = *sellside.begin(); //拿出卖方的最低价挂单entry
		if (o->exec_price < firstSellEntry->price || o->exec_num <= 0) break; //无法撮合
		while (o->exec_price >= firstSellEntry->price && o->exec_num > 0) { //可以撮合
			while (firstSellEntry->q.size() > 0) {
				auto currExOrder = firstSellEntry->q.back();//拿到最先加入队列的订单
				if (o->exec_num >= currExOrder->exec_num) { //买单的数额大于等于挂的卖单
					o->exec_num -= currExOrder->exec_num;
					currExOrder->exec_num = 0; //挂的卖单数量清空为0
					firstSellEntry->q.pop_back();//在队列中弹出该卖单 
					id_to_order.erase(currExOrder->order_id);//由于该卖单已经清空，在id_to_order 中删除它

				}
				else {
					currExOrder->exec_num -= o->exec_num; //减少当前卖单的数量
					o->exec_num = 0; //买单数额清空为0
					
					break;//不再继续撮合
				}
			}
		}
		if (firstSellEntry->q.size() <= 0) { //如果该价格的所有订单都消耗完了，则从sellside中弹出该sellentry
			sellside.erase(sellside.begin());
			price_to_sell.erase(firstSellEntry->price);//由于该entry已经消失，在price_to_sell中删除
		}
	}
	if (o->exec_num > 0) {//未交易完或者未能撮合
		updateBuy(o);
	}



}

void tradeSell(Order* o) {//交易，卖，最坏情况为需要插入新订单 O（nlogn）
	while (buyside.size() > 0 && o->exec_num > 0) { //当买方有挂单时
		auto firstBuyEntry = *buyside.begin(); //拿出买方的最高价挂单entry
		if (o->exec_price > firstBuyEntry->price || o->exec_num <= 0) break; //无法撮合
		while (o->exec_price <= firstBuyEntry->price && o->exec_num > 0) { //可以撮合
			while (firstBuyEntry->q.size() > 0) {
				auto currExOrder = firstBuyEntry->q.back();//拿到最先加入队列的订单
				if (o->exec_num >= currExOrder->exec_num) { //买单的数额大于等于挂的卖单
					o->exec_num -= currExOrder->exec_num;
					currExOrder->exec_num = 0; //挂的买单数量清空为0
					firstBuyEntry->q.pop_back();//在队列中弹出该买单 
					id_to_order.erase(currExOrder->order_id);//由于该买单已经清空，在id_to_order 中删除它

				}
				else {
					currExOrder->exec_num -= o->exec_num; //减少当前挂的买单的数量
					o->exec_num = 0; //卖单数额清空为0
					break;//不再继续撮合
				}
			}
		}
		if (firstBuyEntry->q.size() <= 0) { //如果该价格的所有订单都消耗完了，则从buyside中删除该buyentry
			buyside.erase(buyside.begin());//删除该entry
			price_to_buy.erase(firstBuyEntry->price);//由于该entry已经消失，在price_to_buy中删除
		}
	}
	if (o->exec_num > 0) {//未交易完或者未能撮合
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

