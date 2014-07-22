#include <iostream>
#include "Cog.h"
#include <thread>
#include <list>
#include <stdlib.h>
#include <mutex>

using namespace std;

struct Sobject{
	slot inslot;
	bool die;
	thread sthread;
	void run(){
		while(die != true){
			inslot.eval();
			if(inslot.macromap["die"] == "1")
				die = true;
		}
	}
	void end(){
		sthread.join();
	}
	void start(){
		sthread = thread (&Sobject::run, this);
	}
};


list<Sobject> Sglobal;
std::mutex Sglobal_mutex;
void add_sobj(string content){
	Sglobal_mutex.lock();
	Sglobal.emplace_back();
	Sglobal.back().inslot.set_code(content);
	Sglobal.back().inslot.slotset();
	Sglobal_mutex.unlock();
}
std::mutex Start_mutex;
void start_all(){
	if(Start_mutex.try_lock()){
		list<Sobject>::iterator startiter  = Sglobal.begin();
		while(startiter != Sglobal.cend()){
			(&*startiter)->start();
			advance(startiter, 1);
		}
		Start_mutex.unlock();
	}
}

int main()
{
	slot a;
	a.set_code("( ( red <<( moo ) dog . ) a . ) cat . ( cat a dog ) ? car");
	a.slotset();
	a.eval();
	list<slot> b;
	slot s1;
	slot s2;
	slot s3;
	s1.set_code("cat");
	s2.set_code("m");
	s3.set_code("dog");
	b.push_back(s1);
	b.push_back(s2);
	b.push_back(s3);
	cout << a.macromap.search(b);
    //cout << "\n" << a.get_macs()<<"\n";
    //n["frog"];
    //cout << n["cat"];
    //cout << "woah";

    /*
  add_sobj(" a print 1 die ");
  add_sobj("b print 1 die");
  start_all();
    while (true) {

    }*/
}

