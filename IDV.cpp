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

int main()
{
  add_sobj("a print \"1 die\" defmacro");
  Sglobal.back().start();
    while (true) {
      
    }
}

