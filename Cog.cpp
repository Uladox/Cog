/*#This file is part of Cog
#
#	Copyright 2014 Uladox
#
#    Cog is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    Cog is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Cog.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <list>
#include <stdlib.h>
#include <iterator>

using namespace std;

struct slot
{
    list<slot> slotlist; //original name, right?
    list<slot*> currentlist;
    list<slot*> currentobj;
    string code;
    void slotset(){
        currentlist.push_back(this);
        currentobj.push_back(this);
    }
    void set_code(string newcode){
        code = newcode;
    }
    void code_maker(string& tempcode, bool& to_end);
    void string_maker(string& tempcode, char& firstchar);
    void eval();
    void wordeval(string word, string& tempcode);
    void stringeval(string word);
};


//determines what to do with each section of parsed string
//how to understand: test each statement
//do not hope to understand this mess
//only I can know its true power
//even though it is very simple
//it is a lot of code
void slot::stringeval(string word)
{
    slot tempslot;
    tempslot.set_code(word);
    currentlist.back()->slotlist.push_back(tempslot);
    currentlist.back()->slotlist.back().slotset();
    currentobj.push_back(&currentlist.back()->slotlist.back());
}
void slot::wordeval(string word, string& tempcode)
{
    if(word.compare("print") == 0){
        cout << currentobj.back()->code;
    }else if (word.compare("eval") == 0){
        currentobj.back()->eval();
    }else if (word.compare("(") == 0){
        slot tempslot;
        currentlist.back()->slotlist.push_back(tempslot);
        currentlist.back()->slotlist.back().slotset();
        currentlist.push_back(&currentlist.back()->slotlist.back());
    }else if (word.compare("<<(") == 0){
        currentlist.push_back(currentobj.back());
    }else if (word.compare(">>(") == 0){
        slot tempslot;
        currentlist.back()->slotlist.push_back(tempslot);
        currentlist.back()->slotlist.back().slotset();
        currentlist.push_back(&currentlist.back()->slotlist.back());
        currentlist.back()->slotlist.push_back(*currentobj.back());
    }else if (word.compare(")") == 0){
        if(currentlist.size() == 0){
            cout << "\nError, Too many )\'s?";
             exit (EXIT_FAILURE);
        }
        currentobj.push_back(currentlist.back());
        currentlist.pop_back();
    }else if (word.compare("cdr") == 0){
        slot tempslot = *currentobj.back();
        slotlist.push_back(tempslot);
        slotlist.back().slotset();
        currentobj.push_back(&slotlist.back());
        currentobj.back()->slotlist.pop_front();
    }else if (word.compare("car") == 0){
        slot tempslot = currentobj.back()->slotlist.front();
        slotlist.push_back(tempslot);
        slotlist.back().slotset();
        currentobj.push_back(&slotlist.back());
    }else if (word.compare("reverse") == 0){
        slot tempslot = *currentobj.back();
        slotlist.push_back(tempslot);
        slotlist.back().slotset();
        currentobj.push_back(&slotlist.back());
        currentobj.back()->slotlist.reverse();
    }else if (word.compare("pop_back") == 0){
        currentobj.pop_back();
    }else if (word.compare("to_end") == 0){
        currentlist.back()->slotlist.push_back(slotlist.back());
        currentobj.push_back(&currentlist.back()->slotlist.back());
    }else if (word.compare("pop_prev") == 0){
        list<slot*>::iterator spot = currentobj.begin();
        advance(spot, currentobj.size() - 2);
        currentobj.erase(spot);
    }else if (word.compare("delete") == 0){
        list<slot>::iterator spot = slotlist.end();
        while(&*spot != currentobj.back()){
            spot--;
        }
        currentobj.pop_back();
        slotlist.erase(spot);
    }else if (word.compare("clean") == 0){
        //backref = currentobj.back();
        //for(int i)
        //list<slot>::iterator spot = slotlist.end();
        //for(int i = currentobj.size(); i > 1; --i ){
          //  if(&*spot != backref)
       //         slotlist.erase(spot);
       // }
    }else if (word.compare("this") == 0){
        currentlist.back()->slotlist.push_back(*this);
        currentobj.push_back(&currentlist.back()->slotlist.back());
    }else{
        //if it is not a statement, it must be a variable and a new
        //slot is created at current scope.
        slot tempslot;
        tempslot.set_code(word);
        currentlist.back()->slotlist.push_back(tempslot);
        currentlist.back()->slotlist.back().slotset();
        currentobj.push_back(&currentlist.back()->slotlist.back());
    }
}

    //finds end of string including tabs as \t and new lines as \n etc.
    //also \" is not counted as end, but " is.
    //parses one char at a time.
    void slot::string_maker(string& tempcode, char& firstchar)
    {
        bool backslashed = false;
        bool to_end_string = true;
        string finalized_string;
        while(to_end_string){
            tempcode.erase(0, 1);
            firstchar = tempcode[0];
            if(backslashed == false){
                if(firstchar == '\\'){
                    backslashed = true;
                    } else if(firstchar == '\"'){
                        //this is the en of the string
                        //second unbackslashed "
                        tempcode.erase(0, 1);
                        stringeval(finalized_string);
                        to_end_string = false;
                    }else{
                        //adds char so if char is 'a' and
                        //string is "bc", the ouput is "bca"
                        finalized_string.append(1, firstchar);
                        }
                    }else if(backslashed == true){
                        backslashed = false;
                        if(firstchar == '\\'){
                            finalized_string += "\\";
                        } else if(firstchar == 'n'){
                            finalized_string += "\n";
                        } else if(firstchar == 't'){
                            finalized_string += "\t";
                        }

                    }

                }
            }

    //finds bit of string before next whitespace and sends it to wordeval
    void slot::code_maker(string& tempcode, bool& to_end)
    {

        size_t tempspace = tempcode.find(" ");
        size_t temptab = tempcode.find("\t");
        size_t tempnewln = tempcode.find("\n");
        //finds closest whitespace
        size_t ultimatesize = (tempspace < tempnewln ? tempspace : tempnewln);
        ultimatesize = (ultimatesize < temptab ? ultimatesize : temptab);
        //if not at end of string, cut off part and eval, else eval
        if(ultimatesize != string::npos){
            string evenmoretempcode = tempcode;
            evenmoretempcode.erase(ultimatesize, tempcode.length());
            tempcode.erase(0, evenmoretempcode.length()+1);
            wordeval(evenmoretempcode, tempcode);
        }else{
            to_end = false;
            wordeval(tempcode, tempcode);
        }

    }



    void slot::eval()
    {
        string tempcode = code;
        bool to_end = true;
        while(to_end){
            char firstchar = tempcode[0];
            //whitespace ignorer loop (duh)
            while(firstchar == ' '||firstchar == '\t'||firstchar == '\n'){
                tempcode.erase(0, 1);
                firstchar = tempcode[0];
            }
            //if string
            if(firstchar == '\"'){
                 string_maker(tempcode, firstchar);
            }else{
                code_maker(tempcode, to_end);
        }
    }
}



int main()
{

    slot a;
    a.slotset();
    a.set_code(" a \" a print\" eval this print");
    a.eval();
    //cout <<     a.slotlist.size();
    //a.set_code("\"a print\" eval");
    /*
    slot* b = &a;
    slot c;
    c.set_code("\"a print\" eval");
    b->slotlist.push_back(c);
    b->slotlist.back().eval();
*/
}
