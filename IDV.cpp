#include <iostream>
#include "Cog.h"
#include <thread>
using namespace std;

struct Sobject{
  slot inslot;
  bool die;
  thread sthread;
  void end(){
    sthread.join();
  }
  void run(){
    while(die != true){
      inslot.eval();
      if(inslot.macromap["die"] == "1")
	die = true;
    }
  }
  void start(){
    sthread = thread (&Sobject::run, this);
  }
};

int main()
{
    Sobject a;
    a.inslot.set_code("a print  b print \"cat\" stradd print \"1 die\" defmacro");
    a.inslot.slotset();
    a.start();
    string i;
    cin >> i;
    a.end();
}
