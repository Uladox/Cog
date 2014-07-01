#include <iostream>
#include "Cog.h"
#include <thread>
using namespace std;

struct Sobject{
  slot inslot;
  bool die;
  thread sthread;
  void end(){
    die = true;
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
    sthread.join();
  }
};

int main()
{
    Sobject a;
    a.inslot.set_code("a print  b print \"cat\" stradd print \"1 die\" defmacro");
    a.inslot.slotset();
    a.start();
    a.end();
}
