//
// Created by 丁乐华 on 2020/1/24.
//

#ifndef EAC2_LEX_HPP
#define EAC2_LEX_HPP
#include <string>
#include <vector>
#include <set>
#include <queue>
#include <map>
#include <fstream>
#include <utility>
using namespace std;

const char EMPTY = '\0';

//// 正则表达式
enum Operator {
  CONNECT,  // 连接 ab
  SELECT,   // 选择 |
  CLOSURE,  // 闭包 *
  GROUP,    // 分组 (...)
  LEAF      // 字符集合，包括单个字符或者[]包括的字符
};

class RENode {
public:
  Operator op;
  set<char> chars;
  RENode* left;
  RENode* right;
  RENode* child;
  string re;

  RENode (string& re, int start, int end);
  ~RENode ();
  void print (int tabs = 0);
  void expandRange (string& rre, int start, int end);
};

class RETree {
public:
  RENode head;

  explicit RETree (string& re);
  void print ();
};

//// 有限自动机
class FA {
public:
  int id;
  class State {
  public:
    int n;

    explicit State (int id);
  };
  class Delta {
  public:

    State* start;
    set<char> accept;
    State* end;

    Delta (State* start, char accept, State* end);
    Delta (State* start, const set<char>& accept, State* end);
  };

  set<char> chars;
  vector<Delta*> deltas;
  vector<State*> S;
  vector<State*> SA;
  State* s0;

  FA ();
  ~FA ();

  virtual void print ();
  void addDelta (State* start, char accept, State* end);
  void addDelta (State* start, const set<char>& accept, State* end);
  State* newState ();
};

//// RE2NFA
class RE2NFA: public FA {
public:
  class Node {
  public:
    State* s0;
    State* end;
  };

  State* end;

  explicit RE2NFA (RENode& node);
  RE2NFA::Node construct (RENode& node);
  void print () override;
};

//// NFA2DFA
class NFA2DFA: public FA {
private:
  vector<vector<State*>*> Q;
  map<vector<State*>*, State*> M;
  RE2NFA* nfa;

public:
  explicit NFA2DFA (RE2NFA& nfa);
  void epsilonClosure (vector<State*>* q);
  vector<State*>* closure (vector<State*>* q1, char accept);
  State* getEndState (State* start, char accept);
  set<char> getAccept (State* start, State* end);
  void print () override;
  vector<State*>* findQ (vector<State*>* q);
  void spread (vector<State*>* q);
};

//// minDFA
using Set = set<FA::State*>;
using SetSet = set<Set>;
using Map = map<Set, FA::State*>;

class minDFA: public FA {
public:
  explicit minDFA (NFA2DFA& dfa);
  SetSet split (const SetSet& P, const Set& p, NFA2DFA& dfa);
  Set findP (State* s, SetSet& T);
  void print () override;
};

class WrapFA {
private:
  FA* fa;
  FA::State* current;
public:
  explicit WrapFA (FA* fa);
  bool accept (char c);
  bool isFinish ();
  void reset ();
};
const int BUFFER_MAX = 1024;
enum TokenType {
  VARIABLE,
  KEYWORD,
  SPACE,
  INTEGER,
  STRING,
  UNKNOW
};

class Token {
public:
  string lex;
  TokenType type;
  string typeDesc;
  Token (TokenType type, string lex);
};

class Lex {
private:
  vector<pair<WrapFA, TokenType>> list;
  char buf[BUFFER_MAX * 2];
  int at;
  ifstream sourceFile;

public:
  Lex (string specPath, string sourcePath);
  ~Lex ();
  void addRE (string& re, string& category);
  Token* nextToken ();
};



#endif //EAC2_LEX_HPP