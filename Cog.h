#ifndef COG_H_INCLUDED
#define COG_H_INCLUDED

#include <map>
#include <string>
#include <list>
using namespace std;

struct slot
{
    map<string, string> macromap;
    list<slot> slotlist; //original name, right?
    list<slot*> currentlist;
    string code;
    void slotset(){
    currentlist.push_back(this);
    }
    void set_code(string newcode);
    void eval();
    slot prev_obj();
    slot slot_last(int pos);
    slot neg_slotlast(int pos);
    void inset_obj(string word);
    //string object_code();
    string get_macs();

    struct macrokeeper
    {
        list<pair<string, slot>> macroslist;
        string& operator[](string value);
        list<pair<string, slot> >::iterator find(string value);
        macrokeeper& macl(string value);
    } mackeep;
};


#endif // COG_H_INCLUDED
