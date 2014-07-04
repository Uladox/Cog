#include <iostream>
#include "Cog.h"
#include <thread>
#include <list>
#include <stdlib.h>

using namespace std;

struct Sobject{
  slot inslot;
  bool die;
  thread sthread;
  void run();
  void end(){
    sthread.join();
    cout << "ended";
  }
  void start(){
    sthread = thread (&Sobject::run, this);
  }
};


void Sobject::run(){
    while(die != true){
      inslot.eval();
      if(inslot.macromap["die"] == "1")
	die = true;
    }
  }

int main()
{
    Sobject a;
    a.inslot.set_code("a print  b print \"cat\" stradd print \"1 die\" defmacro");
    a.inslot.slotset();
    a.start();
    //hack, DO NOT USE IN REAL PROJECTS!!!!!!
    //a.sthread.join();
    while (true) {
      
    }
}
