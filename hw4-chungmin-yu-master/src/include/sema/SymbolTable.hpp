#include<vector>
#include<string>
#include"AST/PType.hpp"
#include"AST/constant.hpp"
#define TABLE_PROGRAM 1
#define TABLE_FUNCTION 2
#define TABLE_FORLOOP 3
#define TABLE_OTHER 0

#define ENTRY_PROGRAM 0
#define ENTRY_FUNCTION 1
#define ENTRY_PARAMETER 2
#define ENTRY_VARIABLE 3
#define ENTRY_LOOPVAR 4
#define ENTRY_CONSTANT 5
#define ENTRY_NOTFOUND 6

using namespace std;


class SymbolEntry {
  public:
    string name;
    int kind;
    int level;
    PType type;
    string attribute;
    int pass;
  public:
    SymbolEntry(string p_name, int p_kind, int p_level, PType p_type, string p_attribute, int p_pass)
      :name(p_name), kind(p_kind), level(p_level), type(p_type), attribute(p_attribute), pass(p_pass){};

};

class SymbolTable {
  private:
    int kind;
    vector<SymbolEntry> entries;    
  public:
    SymbolTable(int p_kind) : kind(p_kind){};
    void addSymbol(string p_name, int p_kind, int p_level, PType p_type, string p_attribute, int p_pass){
      SymbolEntry newSymbolEntry(p_name, p_kind, p_level, p_type, p_attribute, p_pass);
      entries.push_back(newSymbolEntry); 
    }
    int getKind(){ return kind; }
    void changeState(int n){ kind = n; }
    SymbolEntry getSymbolEntry(string p_name){
      uint32_t i;
      for(i = 0; i < entries.size(); i++){
        if(p_name == entries[i].name){
          break;
        }
      }
      return entries[i];
    }
    int lookup(string p_name){
      for(uint32_t i = 0; i < entries.size(); i++){
        if(p_name == entries[i].name){
          return entries[i].kind;
        }
      }
      return ENTRY_NOTFOUND;
    }
    void dumpDemarcation(const char ch){
      for (size_t i = 0; i < 110; ++i) {
        printf("%c", ch);
      }
      puts("");
    }          
    void printTable(){
      const char * kinds[]= {"program", "function", "parameter", "variable", "loop_var", "constant"};
      dumpDemarcation('=');
      printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type", "Attribute");
      dumpDemarcation('-');
      for(uint32_t i = 0; i < entries.size(); i++){
        printf("%-33s", entries[i].name.c_str());
        printf("%-11s", kinds[entries[i].kind]);
        printf("%d", entries[i].level);
        if(entries[i].level == 0)
          printf("%-10s", "(global)");
        else
          printf("%-10s", "(local)");
        printf("%-17s", entries[i].type.getPTypeCString());
        printf("%-11s", entries[i].attribute.c_str());
        puts("");
      }
      dumpDemarcation('-');
    }

};

class SymbolManager {
  private:
    vector <SymbolTable *> tables;
  public:
    void addSymbol(string p_name, int p_kind, PType p_type, string p_attribute, int p_pass){
      tables.back()->addSymbol(p_name, p_kind, tables.size()-1, p_type, p_attribute, p_pass);
    }
    int getTopKind(){ return tables.back()->getKind(); }
    void changeState(int n){ tables.back()->changeState(n); }
    SymbolEntry getSymbolEntry(string p_name){
      int i;
      for(i = tables.size() - 1; i >= 0; i--){
        if(tables[i]->lookup(p_name) != ENTRY_NOTFOUND){
            break;
        }
      }
      return tables[i]->getSymbolEntry(p_name);
    }
    int lookup(string p_name, bool global){
      if(global){
        for(int i = tables.size() - 1; i >= 0; i--){
          if(tables[i]->lookup(p_name) != ENTRY_NOTFOUND){
            return tables[i]->lookup(p_name);
          }
        }
      }
      else{
          return tables.back()->lookup(p_name);
      }
      return ENTRY_NOTFOUND;
    }
    bool Lookup(string p_name){
      for(int i = tables.size() - 1; i >= 0; i--){
        if(tables[i]->getKind() != 3){
          continue;
        }
        if(tables[i]->lookup(p_name) != ENTRY_NOTFOUND){
          return true;
        }
      }
      return false;
    }          
    void pushScope(SymbolTable *new_scope){ tables.push_back(new_scope); }
    void popScope(int op){
      SymbolTable* temp = tables.back();
      if(op){
        tables.back()->printTable();
      }
      tables.pop_back();
      free(temp);
    }

};

class FunctionEntry{
  public:
    string name;
    vector<PType> para_type;
    PType ret_type;
  public:
    FunctionEntry(string p_name, vector<PType> p_para_type, PType p_ret_type)
      :name(p_name), para_type(p_para_type), ret_type(p_ret_type){};
};

class FunctionManager{
  public:
    int fnum;
    vector<FunctionEntry> tables;    
  public:
    FunctionManager() { fnum = 0; }
    void addFunction(string p_name, vector<PType> p_para_type, PType p_ret_type){
      FunctionEntry newFunctionEntry(p_name, p_para_type, p_ret_type);
      tables.push_back(newFunctionEntry);
    }  
    void addFuncNum(){ fnum++; }
    void minusFuncNum(){ fnum--; }
    int getFuncNum(){ return fnum; }
    FunctionEntry lookup(int n) { return tables[n]; }
    FunctionEntry Lookup(string p_name){
      uint32_t i;
      for( i = 0; i < tables.size(); i++){
        if(p_name == tables[i].name){
          break;
        }
      }
      return tables[i];
    }

};
