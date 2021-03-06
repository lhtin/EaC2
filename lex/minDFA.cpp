#include <iostream>
#include <set>
#include <algorithm>
#include <iterator>
#include "all.hpp"

SetSet minDFA::split (const SetSet& P, const Set& p, DFA& dfa) {
  Set temp1;
  auto begin = p.begin();
  temp1.insert(*begin);
  for (auto it = next(begin); it != p.end(); it++) {
    bool allOK = true;
    for (char c : dfa.chars) {
      FA::State* end1 = dfa.getEndState(*begin, c);
      FA::State* end2 = dfa.getEndState(*it, c);
      if (end1 != end2) {
        allOK = false;
        break;
      }
    }
    if (allOK) {
      temp1.insert(*it);
    }
  }
  Set temp2;
  set_difference(p.begin(), p.end(), temp1.begin(), temp1.end(), inserter(temp2, temp2.begin()));

  SetSet result;
  result.insert(temp1);
  if (!temp2.empty()) {
    result.insert(temp2);
  }
  return result;
}

Set minDFA::findP (FA::State* s, SetSet& T) {
  for (const Set& item : T) {
    if (item.count(s) > 0) {
      return item;
    }
  }
  return Set();
}

bool compare (FA::State* a, FA::State* b) {
  return a->n < b->n;
}

minDFA::minDFA (DFA& dfa) {
  chars = dfa.chars;

  SetSet T;
  SetSet P;

  Set all(dfa.S.begin(), dfa.S.end());
  Set DA(dfa.SA.begin(), dfa.SA.end());
  Set Rest;
  set_difference(all.begin(), all.end(), DA.begin(), DA.end(), inserter(Rest, Rest.begin()));

  T.insert(DA);

  if (!Rest.empty()) {
    T.insert(Rest);
    while (P.size() != T.size()) {
      P = T;
      T = SetSet();
      for (const Set &p : P) {
        SetSet temp = split(P, p, dfa);
        T.insert(temp.begin(), temp.end());
      }
    }
  }

  Map M;
  Set SA;
  for (const Set& item : T) {
    State* s = newState();
    S.push_back(s);
    M[item] = s;
    if (item.count(dfa.s0) > 0) {
      s0 = s;
    }
    Set inter;
    set_intersection(dfa.SA.begin(), dfa.SA.end(), item.begin(), item.end(), inserter(inter, inter.begin()), compare);
    if (!inter.empty()) {
      SA.insert(s);
    }
  }

  for (Delta* item : dfa.deltas) {
    Set t1 = findP(item->start, T);
    State* start = M[t1];
    Set t2 = findP(item->end, T);
    State* end = M[t2];
    addDelta(start, item->accept, end);
  }
  this->SA = vector<State*>(SA.begin(), SA.end());
}
void minDFA::print () {
  cout << "-----minDFA start-----" << endl;
  cout << FA::toString() << endl;
  cout << "-----minDFA end-----" << endl;
}