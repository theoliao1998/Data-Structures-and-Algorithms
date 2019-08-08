#ifndef __IMPLE__
#define __IMPLE__

#include <iostream>
#include <sstream>
#include <map>
#include <queue> 
#include <vector>
#include <set>
#include <functional>

using namespace std;

struct Order{
	unsigned int ID;
	unsigned int timestamp;
	string Client_name;
	bool Buy_Sell;
	string equity_symbol;
	unsigned int price;
	unsigned int quantity; //original quantity
	unsigned int num;      //left quantity
	int duration;
    unsigned int expired_time;
    bool valid;
};

struct Trade{
	string buyer;
	string seller;
	string equity_symbol;
	unsigned price;
	unsigned num;
	unsigned time;
};

struct Transfers{
	unsigned num_buy;
	unsigned num_sell;
	int MoneyTransferred;
};

struct TimeTrade{
	Order* order_buy;
	Order* order_sell;
	Order* order_sell2;
	int profit;
};

struct cmp_buy
{
    bool operator()(Order* a, Order* b) const
    {
        if (a->price == b->price)
			return a->ID < b->ID;
		else
			return a->price > b->price;
    }
};

struct cmp_sell
{
    bool operator()(Order* a, Order* b) const
    {
        if (a->price == b->price)
			return a->ID < b->ID;
		else
			return a->price <= b->price;
    }
};

struct cmp_trade_less
{
    bool operator()(Trade* a, Trade* b) const
    {
		return a->price < b->price;
    }
};

struct cmp_trade_great
{
    bool operator()(Trade* a, Trade* b) const
    {
        return a->price >= b->price;
    }
};

typedef set<Order*,cmp_buy> Orders_BUY;
typedef set<Order*,cmp_sell> Orders_SELL;
typedef priority_queue<Trade*, vector<Trade*>,cmp_trade_great> Min_Heap;
typedef priority_queue<Trade*, vector<Trade*>,cmp_trade_less> Max_Heap;


struct Trades{
	Min_Heap min_heap;
	Max_Heap max_heap;
};

struct Orders_BUY_and_Sell{
	Orders_BUY* buy;
	Orders_SELL* sell;
};

void addTrade(Trade* td, map<string,Trades*>* Tds){
	map<string,Trades*>::iterator it;
	Trades* cur;
	if((it = Tds->find(td->equity_symbol)) != Tds->end()){
		cur = it->second;
		if(cur->min_heap.size() != cur->max_heap.size()){
			if(cmp_trade_great()(td,cur->max_heap.top()))
				cur->min_heap.push(td);
			else{
				cur->min_heap.push(cur->max_heap.top());
				cur->max_heap.pop();
				cur->max_heap.push(td);
			}
		}
		else{
			if(cmp_trade_less()(td,cur->min_heap.top()))
				cur->max_heap.push(td);
			else{
				cur->max_heap.push(cur->min_heap.top());
				cur->min_heap.pop();
				cur->min_heap.push(td);
			}
		}
	}
	else{
		Trades* Tds_eq = new Trades;
		(Tds_eq->max_heap).push(td);
		Tds->insert(make_pair(td->equity_symbol, Tds_eq));
	}
}

void remove(Order* o, bool B_S, Orders_BUY* Expired_B, Orders_SELL* Expired_S, vector<TimeTrade*> time_trades, unsigned size){
	bool del = true;
	for(unsigned i=0; i<size; i++){
		if(B_S){
			if(o == time_trades[i]->order_buy){
				del = false;
				Expired_B->insert(o);
				break;
			}
		}
		else{
			if(o == time_trades[i]->order_sell || o==time_trades[i]->order_sell2){
				del = false;
				Expired_S->insert(o);
				break;
			}
		}
	}
	if(del)
		delete o;
}

void invalidUpdate(map<string,Orders_BUY_and_Sell*> * Orders_b_s, Orders_BUY* Expired_B, Orders_SELL* Expired_S, unsigned time_cur, vector<TimeTrade*> time_trades, unsigned size){
	//Once the time is updated, the expired orders are invalid 
	auto it = Orders_b_s->begin();
	while(it != Orders_b_s->end()){
		Orders_BUY * o_b = it->second->buy;
		if(it->second->buy){
			Orders_BUY::iterator it1 = o_b->begin();
			while(it1 != o_b->end()){
				if(!(*it1)->valid || (*it1)->expired_time <= time_cur || (*it1)->num <= 0){
					remove(*it1, true, Expired_B, Expired_S, time_trades, size);
					//Expired_B->insert(*it1);
					it1 = o_b->erase(it1);
				}
				else
					++it1;
			}
		}
		Orders_SELL * o_s = it->second->sell;
		if(it->second->sell){
			Orders_SELL::iterator it2 = o_s->begin();
			while(!o_s->empty() && it2 != o_s->end()){
				if(!(*it2)->valid || (*it2)->expired_time <= time_cur || (*it2)->num <= 0){
					remove(*it2, false, Expired_B, Expired_S, time_trades, size);
					//Expired_S->insert(*it2);
					it2 = o_s->erase(it2);
				}
				else
					++it2;
			}
		}
		++it;
	}
}


void transferMoney(string client, unsigned num, unsigned price, map<string, Transfers*> * Tfs, bool B_or_S){
	int coefficient = B_or_S ? (-1) : 1;
	map<string, Transfers*>::iterator it;
	if((it = Tfs->find(client)) != Tfs->end()){
		Transfers * tf = it->second;
		if(B_or_S) tf->num_buy += num;
		else tf->num_sell += num;
		tf->MoneyTransferred += coefficient * num * price;
	}
	else{
		Transfers * tf = new Transfers;
		*tf = {0,0,0};
		if(B_or_S) tf->num_buy += num;
		else tf->num_sell += num;
		tf->MoneyTransferred += coefficient * num * price;
		Tfs->insert(make_pair(client,tf));
	}
}

void implement(bool verbose, bool median, bool midpoint, bool transfer, bool time_travel, vector<string> & eq_symbs, unsigned time_travel_size){
	vector<TimeTrade*> time_trades;
	if(time_travel){
		for(unsigned i=0; i<time_travel_size; i++){
			TimeTrade* tt = new TimeTrade;
			*tt = {NULL,NULL,NULL,0};
			time_trades.push_back(tt);
		}
	}
	unsigned CommissionEarn = 0;
	unsigned TotalMoney = 0;
	unsigned NumTrades = 0;
	unsigned NumShares = 0;
	char t;
	string order, c_name, BorS,eq_symb;
	unsigned time,price,num;
	unsigned time_cur = 0;
	unsigned id = 0;
	int duration;
	bool B_S;
	map<string,Orders_BUY_and_Sell*> * Orders_b_s = new map<string,Orders_BUY_and_Sell*>; 
	Orders_BUY * Expired_B = new Orders_BUY;
	Orders_SELL * Expired_S = new Orders_SELL;
	map<string,Trades*> * Tds = new map<string,Trades*>;
	map<string, Transfers*> * Tfs = new map<string,Transfers*>;
	ios::sync_with_stdio(false);
	cin.tie(0);
	while ((t = cin.get()) && t!='\n'){
		getline(cin,order);
		order = t + order;
		stringstream line(order);
		if (!(line>>time>>c_name>>BorS>>eq_symb>>t>>price>>t>>num>>duration)) break;
		if(median && time > time_cur){
			map<string,Trades*>::iterator it;
			it = Tds->begin();
			Trades* t;
			unsigned median, size1, size2;
			while(it != Tds->end())
			{
				t = it->second;
				size1 = t->max_heap.size();
				size2 = t->min_heap.size();
				if(size1 == size2)
					median = (t->max_heap.top()->price + t->min_heap.top()->price)/2;
				else{
					median = t->max_heap.top()->price;
				}
				cout<<"Median match price of "<<it->first<<" at time "<<time_cur<<" is $"<<median<<endl;
				++it;
			}
		}
		if(midpoint && time > time_cur){
			bool exist_buy, exist_sell;
			unsigned max_buy = 0;
			unsigned min_sell = 0;
			map<string,Orders_BUY_and_Sell*>::iterator it;
			it = Orders_b_s->begin();
			Orders_BUY_and_Sell* t;
			while(it != Orders_b_s->end()){
				exist_buy = false;
				exist_sell = false;
				t = it->second;
				if((!t->buy) and (!t->sell))
					continue;
				else{
					if(t->sell){
						Orders_SELL* s = it->second->sell;
						Orders_SELL::iterator iterCur = s->begin();
						Orders_SELL::iterator iterEnd = s->end();
						Order* cur;
						for (; (iterCur != iterEnd);){
							cur = *iterCur;
							if(!cur->valid){
								remove(cur, false, Expired_B, Expired_S, time_trades, time_travel_size);
								iterCur = s->erase(iterCur);
								continue;
							}
							else if((time_cur >= cur->expired_time) || (cur->num <= 0)){
								remove(cur, false, Expired_B, Expired_S, time_trades, time_travel_size);
								iterCur = s->erase(iterCur);
								continue;
							}
							else{
								min_sell = cur->price;
								exist_sell = true;
								break;
							}
						}
					}
					if(t->buy){
						Orders_BUY* b = it->second->buy;
						Orders_BUY::iterator iterCur = b->begin();
						Orders_BUY::iterator iterEnd = b->end();
						Order* cur;
						for (; (iterCur != iterEnd);){
							cur = *iterCur;
							if(!cur->valid){
								remove(cur, true, Expired_B, Expired_S, time_trades, time_travel_size);
								iterCur = b->erase(iterCur);
								continue;
							}
							else if((time_cur >= cur->expired_time) || (cur->num <= 0)){
								remove(cur, true, Expired_B, Expired_S, time_trades, time_travel_size);
								iterCur = b->erase(iterCur);
								continue;
							}
							else{
								max_buy = cur->price;
								exist_buy = true;
								break;
							}
						}
					}	
				}
				if((!exist_buy) && (!exist_sell)) continue;
				else if ((!exist_buy) || (!exist_sell))
					cout<<"Midpoint of "<<it->first<<" at time "<<time_cur<<" is undefined"<<endl;
				else
					cout<<"Midpoint of "<<it->first<<" at time "<<time_cur<<" is $"<<(max_buy+min_sell)/2<<endl;
				++it;
			}
			
		}

        B_S = (BorS == "BUY") ? 1:0;
        id ++;
		Order* o = new Order;   //create the order
		*o = {id, time, c_name, B_S, eq_symb, price, num, num, duration, 0, true};
		if(duration  < 0){
			o->expired_time = -1;
		}
		else{
			o->expired_time = time + duration;
		}
		
		
		map<string,Orders_BUY_and_Sell*>::iterator it;
		bool have_buy = false;
		bool have_sell = false;
		if((it = Orders_b_s->find(eq_symb)) != Orders_b_s->end()){
			if(it->second->buy) have_buy = true;
			if(it->second->sell) have_sell = true;
		}
			
		if (B_S){
			if(have_sell){
			    Orders_SELL* s = it->second->sell;
			    Orders_SELL::iterator iterCur = s->begin();
				Orders_SELL::iterator iterEnd = s->end();
				Order* cur;
				for (; (iterCur != iterEnd) && (o->num > 0);){
					cur = *iterCur;
					if(cur->price > price) break;
					else if(cur->valid == false) {
						remove(cur, false, Expired_B, Expired_S, time_trades, time_travel_size);
						iterCur = s->erase(iterCur);
						continue;
					}
					else if(cur->expired_time <= time){
						cur->valid = false;
						remove(cur, false, Expired_B, Expired_S, time_trades, time_travel_size);
						iterCur = s->erase(iterCur);
						continue;
					}
					else if(cur->num <= o->num){
						if(verbose){
							cout<<c_name<<" purchased "<<cur->num<<" shares of "<<eq_symb<<" from "<<cur->Client_name<<" for $"<<cur->price<<"/share"<<endl;
						}
						if(median){
							Trade* td = new Trade;
							*td = {c_name, cur->Client_name, eq_symb, cur->price, cur->num, time};
							addTrade(td, Tds);
						}
						if(transfer){
							transferMoney(c_name, cur->num, cur->price, Tfs, true);
							transferMoney(cur->Client_name, cur->num, cur->price, Tfs, false);
						}
						
						unsigned commission = cur->num * cur->price /100;
						commission *= 2;
						CommissionEarn += commission;
						TotalMoney += cur->num * cur->price;
						NumTrades++;
						NumShares += cur->num;
						
						o->num -= cur->num;
						cur->num = 0;
						cur->valid = false;
						remove(cur, false, Expired_B, Expired_S, time_trades, time_travel_size);
						iterCur = s->erase(iterCur);
						if(o->num == 0) o->valid = false;
					}
					else{
						if(verbose){
							cout<<c_name<<" purchased "<<o->num<<" shares of "<<eq_symb<<" from "<<cur->Client_name<<" for $"<<cur->price<<"/share"<<endl;
						}
						if(median){
							Trade* td = new Trade;
							*td = {c_name, cur->Client_name, eq_symb, cur->price, o->num, time};
							addTrade(td, Tds);
						}
						if(transfer){
							transferMoney(c_name, o->num, cur->price, Tfs, true);
							transferMoney(cur->Client_name, o->num, cur->price, Tfs, false);
						}
						unsigned commission = o->num * cur->price /100;
						commission *= 2;
						CommissionEarn += commission;
						TotalMoney += o->num * cur->price;
						NumTrades++;
						NumShares += o->num;
						
						cur->num -= o->num;
						o->num = 0;
						o->valid = false;
						break;
					}
				}
			}
			if(have_buy){
				it->second->buy->insert(o);
			}
			else{
				Orders_BUY* o_b = new Orders_BUY;
				o_b->insert(o);
				if(have_sell){
					it->second->buy = o_b;
				}
				else{
					Orders_BUY_and_Sell* o_b_s = new Orders_BUY_and_Sell;
					*o_b_s={NULL,NULL};
					o_b_s->buy = o_b;
					Orders_b_s->insert(make_pair(eq_symb,o_b_s));
				}
			}	
		}
		else{
			if(have_buy){
				
			    Orders_BUY* b = it->second->buy;
			    Orders_BUY::iterator iterCur = b->begin();
				Orders_BUY::iterator iterEnd = b->end();
				Order* cur;
				for (; (iterCur != iterEnd) && (o->num > 0);){
					cur = *iterCur;
					if(cur->price < price) break;
					else if(cur->valid == false){
						remove(cur, true, Expired_B, Expired_S, time_trades, time_travel_size);
						iterCur = b->erase(iterCur);
						continue;
					}
					else if(cur->expired_time <= time){
						cur->valid = false;
						remove(cur, true, Expired_B, Expired_S, time_trades, time_travel_size);
						iterCur = b->erase(iterCur);
						continue;
					}
					else if(cur->num <= o->num){
						if(verbose){
							cout<<cur->Client_name<<" purchased "<<cur->num<<" shares of "<<eq_symb<<" from "<<c_name<<" for $"<<cur->price<<"/share"<<endl;
						}
						if(median){
							Trade* td = new Trade;
							*td = {cur->Client_name,c_name, eq_symb, cur->price, cur->num, time};
							addTrade(td, Tds);
						}
						if(transfer){
							transferMoney(c_name, cur->num, cur->price, Tfs, false);
							transferMoney(cur->Client_name, cur->num, cur->price, Tfs, true);
						}
						
						unsigned commission = cur->num * cur->price /100;
						commission *= 2;
						CommissionEarn += commission;
						TotalMoney += cur->num * cur->price;
						NumTrades++;
						NumShares += cur->num;
						
						o->num -= cur->num;
						cur->num = 0;
						cur->valid = false;
						remove(cur, true, Expired_B, Expired_S, time_trades, time_travel_size);
						iterCur = b->erase(iterCur);
						if(o->num == 0) o->valid = false;
					}
					else{
						if(verbose){
							cout<<cur->Client_name<<" purchased "<<o->num<<" shares of "<<eq_symb<<" from "<<c_name<<" for $"<<cur->price<<"/share"<<endl;
						}
						if(median){
							Trade* td = new Trade;
							*td = {cur->Client_name, c_name, eq_symb, cur->price, o->num, time};
							addTrade(td, Tds);
						}
						if(transfer){
							transferMoney(c_name, o->num, cur->price, Tfs, false);
							transferMoney(cur->Client_name, o->num, cur->price, Tfs, true);
						}
						
						unsigned commission = o->num * cur->price /100;
						commission *= 2;
						CommissionEarn += commission;
						TotalMoney += o->num * cur->price;
						NumTrades++;
						NumShares += o->num;
						
						cur->num -= o->num;
						o->num = 0;
						o->valid = false;
						break;
					}
				}
			}
			if(have_sell){
				it->second->sell->insert(o);
			}
			else{
				Orders_SELL* o_s = new Orders_SELL;
				o_s->insert(o);
				if(have_buy){
					it->second->sell = o_s;
				}
				else{
					Orders_BUY_and_Sell* o_b_s = new Orders_BUY_and_Sell;
					*o_b_s={NULL,NULL};
					o_b_s->sell = o_s;
					Orders_b_s->insert(make_pair(eq_symb,o_b_s));
				}
			}	
		}
		if(duration==0) o->valid = false;
		
		if(time_travel){
			for(unsigned i = 0; i < time_travel_size; i++){
				if(eq_symbs[i] == eq_symb){
					if(B_S){
						if (!time_trades[i]->order_sell) break;
						if(time_trades[i]->order_buy){
							if(price > time_trades[i]->order_buy->price){
								time_trades[i]->order_buy = o;
								if(time_trades[i]->order_sell2){
									time_trades[i]->order_sell = time_trades[i]->order_sell2;
									time_trades[i]->order_sell2 = NULL;
								}
								if(time_trades[i]->order_sell)
									time_trades[i]->profit = time_trades[i]->order_buy->price - time_trades[i]->order_sell->price;
							}
							else{
								if(time_trades[i]->order_sell2){
									int newprofit = price - time_trades[i]->order_sell2->price;
									if(newprofit > time_trades[i]->profit){
										time_trades[i]->order_sell = time_trades[i]->order_sell2;
										time_trades[i]->order_sell2 = NULL;
										time_trades[i]->order_buy = o;
										time_trades[i]->profit = newprofit;
									}
								}
							}
						}
						else{
							time_trades[i]->order_buy = o;
							time_trades[i]->profit = price - time_trades[i]->order_sell->price;
						}
					}
					else{
						if(time_trades[i]->order_sell){
							if(time_trades[i]->order_buy){
								if (price < time_trades[i]->order_sell->price)
									time_trades[i]->order_sell2 = o;
							}
							else{
								if (price < time_trades[i]->order_sell->price)
									time_trades[i]->order_sell = o;
							}
						}
						else{
							time_trades[i]->order_sell = o;
						}
					}	
				break;
				}
			}
		}
		if(time != time_cur) invalidUpdate(Orders_b_s, Expired_B, Expired_S, time_cur, time_trades, time_travel_size);
		time_cur = time;
	}
	if(median){
		map<string,Trades*>::iterator it;
		it = Tds->begin();
		Trades* t;
		unsigned median, size1, size2;
		while(it != Tds->end())
		{
			t = it->second;
			size1 = t->max_heap.size();
			size2 = t->min_heap.size();
			if(size1 == size2)
				median = (t->max_heap.top()->price + t->min_heap.top()->price)/2;
			else{
				median = t->max_heap.top()->price;
			}
			cout<<"Median match price of "<<it->first<<" at time "<<time_cur<<" is $"<<median<<endl;
			++it;
		}
	}
	if(midpoint){
		bool exist_buy, exist_sell;
		unsigned max_buy = 0;
		unsigned min_sell = 0;
		map<string,Orders_BUY_and_Sell*>::iterator it;
		it = Orders_b_s->begin();
		Orders_BUY_and_Sell* t;
		while(it != Orders_b_s->end()){
			exist_buy = false;
			exist_sell = false;
			t = it->second;
			if((!t->buy) and (!t->sell))
				continue;
			else{
				if(t->sell){
					Orders_SELL* s = it->second->sell;
					Orders_SELL::iterator iterCur = s->begin();
					Orders_SELL::iterator iterEnd = s->end();
					Order* cur;
					for (; (iterCur != iterEnd); ++iterCur){
						cur = *iterCur;
						if(!cur->valid) continue;
						else if((time_cur >= cur->expired_time) || (cur->num <= 0)){
							cur->valid = false;
						}
						else{
							min_sell = cur->price;
							exist_sell = true;
							break;
						}
					}
				}
				if(t->buy){
					Orders_BUY* b = it->second->buy;
					Orders_BUY::iterator iterCur = b->begin();
					Orders_BUY::iterator iterEnd = b->end();
					Order* cur;
					for (; (iterCur != iterEnd); ++iterCur){
						cur = *iterCur;
						if(!cur->valid) continue;
						else if((time_cur >= cur->expired_time) || (cur->num <= 0)){
							cur->valid = false;
						}
						else{
							max_buy = cur->price;
							exist_buy = true;
							break;
						}
					}
				}	
			}
			if((!exist_buy) && (!exist_sell)) continue;
			else if ((!exist_buy) || (!exist_sell))
				cout<<"Midpoint of "<<it->first<<" at time "<<time_cur<<" is undefined"<<endl;
			else
				cout<<"Midpoint of "<<it->first<<" at time "<<time_cur<<" is $"<<(max_buy+min_sell)/2<<endl;
			++it;
		}
		
	}
	cout<<"---End of Day---"<<endl;
	cout<<"Commission Earnings: $"<<CommissionEarn<<endl;
	cout<<"Total Amount of Money Transferred: $"<<TotalMoney<<endl;
	cout<<"Number of Completed Trades: "<<NumTrades<<endl;
	cout<<"Number of Shares Traded: "<<NumShares<<endl;
	if(transfer){
		map<string,Transfers*>::iterator it;
		it = Tfs->begin();
		Transfers* t;
		while(it != Tfs->end()){
			t = it->second;
			cout<<it->first<<" bought "<<t->num_buy<<" and sold "<<t->num_sell<<" for a net transfer of $"<<t->MoneyTransferred<<endl;
			++it;
		}
	}
	if(time_travel){
		for(unsigned i = 0; i < time_travel_size; i++){
			int time1, time2;
			if(time_trades[i]->order_buy && time_trades[i]->order_sell){
				time1 = time_trades[i]->order_sell->timestamp;
				time2 = time_trades[i]->order_buy->timestamp;
			}
			else{
				if(time_trades[i]->order_sell)
					cout<<"BUY "<<time_trades[i]->order_sell->timestamp<<endl;
				if(time_trades[i]->order_buy) 
					cout<<"SELL "<<time_trades[i]->order_buy->timestamp<<endl;
				time1 = time2 = -1;
			}
			cout<<"Time travelers would buy "<<eq_symbs[i]<<" at time: "<<time1<<" and sell it at time: "<<time2<<endl;
		}
	}
	
	map<string,Orders_BUY_and_Sell*>::iterator it1 = Orders_b_s->begin();
	while(it1 != Orders_b_s->end()){
		if(it1->second->buy){
			Orders_BUY::iterator it = it1->second->buy->begin();
			while(it != it1->second->buy->end()){
				delete *it;
				++it;
			}
			delete it1->second->buy;
		} 
		if(it1->second->sell){
			Orders_SELL::iterator it = it1->second->sell->begin();
			while(it != it1->second->sell->end()){
				delete *it;
				++it;
			}
			delete it1->second->sell;
		} 
		delete it1->second;
		++it1;
	}
	delete Orders_b_s;

	map<string,Trades*>::iterator it2 = Tds->begin();
	while(it2 != Tds->end()){
		while(!it2->second->min_heap.empty()){
			delete it2->second->min_heap.top();
			it2->second->min_heap.pop();
		}
		while(!it2->second->max_heap.empty()){
			delete it2->second->max_heap.top();
			it2->second->max_heap.pop();
		}		
		delete it2->second;
		++it2;
	}
	delete Tds;
	
	map<string,Transfers*>::iterator it3 = Tfs->begin();
	while(it3 != Tfs->end()){
		delete it3->second;
		++it3;
	}
	delete Tfs;

	while(!time_trades.empty()){
		delete time_trades.back();
		time_trades.pop_back();
	}
	
	auto it4 = Expired_B->begin();
	while(it4 != Expired_B->end()){
		delete *it4;
		++it4;
	}
	delete Expired_B;
	auto it5 = Expired_S->begin();
	while(it5 != Expired_S->end()){
		delete *it5;
		++it5;
	}
	delete Expired_S;
	
}

#endif
