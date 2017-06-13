
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

class rule 
{
public:
   char left_part;
   string right_part;
   rule() {
   }
   rule(char l_str, string r_str) {
      left_part = l_str;
      right_part  = r_str;
   }
};

class grammar
{
public:
  char start_symbol;
  vector <char> terminal;  // Здесь применяется vector, а не unordered_set т.к. нужно перебирать все элементы
  vector <char> nonterminal;
  long t_size;
  long n_size;
  vector <rule> rules;
};

class cell {
public:
    char ch;
    long num;
    cell() {
      ch = 'e';
      num = 0;
    }
    cell(char c, long n) {
      ch = c;
      num = n;
    }
};

class situation {
public:
  char left_part;
  string right_a;   
  string right_b;
  char symbol;
  situation() {
  }
  situation(char l_p, string r_a, string r_b, char sym) {
    left_part = l_p;
    right_a = r_a;
    right_b = r_b;
    symbol = sym;
  }
};

grammar gr;

int check_symbol(char c) {
  for(long i = 0; i < gr.nonterminal.size(); i++) {
    if(gr.nonterminal[i] == c) {
      return 1;
    }
  }
  return 0;
}

int check_e(vector <char> v) {
  for(long i = 0; i < v.size(); i++) {
    if(v[i] == 'e') {
      return 1;
    }
  }
  return 0;
}

int check_situation(situation s, vector <situation> mult) {
  for (long i = 0; i < mult.size(); i++)
  {
    if((mult[i].left_part == s.left_part) && (mult[i].right_a == s.right_a) && (mult[i].right_b == s.right_b) && (mult[i].symbol == s.symbol)) {
      return 1;
    }
  }
  return 0; // 1 - принадлежит, 0 - не принадлежит
}

int check_r(vector <situation> I, char sym) {
  for(long i = 0; i < I.size(); i++) {
    if((I[i].right_b == "") && (I[i].symbol == sym)) {
      for(long k = 0; k < gr.rules.size(); k++) {
        if((I[i].left_part == gr.rules[k].left_part) && (I[i].right_a == gr.rules[k].right_part) && (I[i].left_part != 'S')) {
          return (k + 1);
        }  
      }
    }
  }
  return 0;
}

int check_sym(char sym, vector <situation> mult) {
  for (long i = 0; i < mult.size(); i++)
  {
    if((mult[i].right_b != "") && (mult[i].right_b[0] == sym)) {
      return 1;
    }
  }
  return 0; // 1 - принадлежит, 0 - не принадлежит
}

bool check_equality(vector <situation> v1, vector <situation> v2) {
  for(long i = 0; i < v1.size(); i++) {
    bool flag = false;
    for(long j = 0; j < v2.size(); j++) {
      if((v1[i].left_part == v2[j].left_part) && (v1[i].right_a == v2[j].right_a) && (v1[i].right_b == v2[j].right_b) && (v1[i].symbol == v2[j].symbol)) {
        flag = true;
        break;
      }
    }
    if(!flag) {
      return 0;
    }
  }
  return 1;
}

int check_mult_situation(vector <situation> mult, vector < vector <situation> > v) {
  for (long i = 0; i < v.size(); i++) {
    if(check_equality(mult, v[i])) {
      return 1;
    }
  }
  return 0;
}

vector < vector <situation> > C;  // множество множеств ситуаций, каждое множество ситуаций по сути соответствует состоянию ДКА, распознающего активные префиксы
vector < vector <char> > first_sym;

int return_num(char c) {
  long k = -1;
  for(long i = 0; i < gr.t_size; i++) {
    if(c == gr.terminal[i]) {
      k = i;
      break;
    }
  }
  for(long i = 0; i < gr.n_size; i++) {
    if(c == gr.nonterminal[i]) {
      k = i + gr.t_size;
      break;  
    }
  }
  return k;
}

// Вспомогательные функции для построения таблицы

int first() {
  bool flag, ind, sh;
  vector <char> tr;
  do {
    flag = false;   // flag - continue
    for(long i = 0; i < gr.n_size; i++) {
      for(long j = 0; j < gr.rules.size(); j++) {
        if(gr.rules[j].left_part == gr.nonterminal[i]) {
          ind = true;  // ind - nonstop
          long l = gr.rules[j].right_part.size();
          for(long k = 0; k < l; k++) {
            tr = first_sym[i + gr.t_size];
            long rk;
            for(long p = 0; p < gr.t_size; p++) {
              if(gr.rules[j].right_part[k] == gr.terminal[p]) {
                rk = p;
                break;
              }
            }
            for(long p = 0; p < gr.n_size; p++) {
              if(gr.rules[j].right_part[k] == gr.nonterminal[p]) {
                rk = p + gr.t_size;
                break;
              }
            }
            for(long p = 0; p < first_sym[rk].size(); p++) {
              bool sh = false;;
              for(long r = 0; r < first_sym[i + gr.t_size].size(); r++) {
                if(first_sym[rk][p] == first_sym[i + gr.t_size][r]) {
                  sh = true;
                  break;
                }
              }
              if((!sh) && (first_sym[rk][p] != 'e')) {
                first_sym[i + gr.t_size].push_back(first_sym[rk][p]);
                flag = true;
              }
            }
            if(!check_e(first_sym[rk])) {
              ind = false;
              break;
            }
          }
        if(ind) {
          flag = true;
          first_sym[i + gr.t_size].push_back('e');
        }
      }
      }
    }
  } while(flag);
  return 0;
}


vector <char> first_sequence(string seq) {  // FIRST для цепочки
  vector <char> res;
  bool flag = true;  // flag - nonstop
  long sym, l = seq.size();
  for(long i = 0; i < l; i++) {
    sym = return_num(seq[i]);
    for(long j = 0; j < first_sym[sym].size(); j++) {
      bool ind = false;
      for(long k = 0; k < res.size(); k++) {
        if(res[k] == first_sym[sym][j]) {
          ind = true;
          break;
        }
      }
      if((!ind) && (first_sym[sym][j] != 'e')) {
        res.push_back(first_sym[sym][j]);
      }
    }
    if(find(first_sym[sym].begin(), first_sym[sym].end(), 'e') == first_sym[sym].end()) {
      flag = false;
      break;
    }
  }
  if(flag) {
    res.push_back('e');
  }
  return res;
}

vector <situation> closure(vector <situation> I) {
  char sym;
  long count = 0;
  bool flag = true;
  do {
    flag = false;
    for(long i = 0; i < I.size(); i++) {
      if((I[i].right_b != "") && (check_symbol(I[i].right_b[0]))) {
        sym = I[i].right_b[0];
        vector <char> first_s = first_sequence(I[i].right_b.substr(1, I[i].right_b.size() - 1) + I[i].symbol);
        for(long j = 0; j < gr.rules.size(); j++) {
          if(gr.rules[j].left_part == sym) {
            for(long k = 0; k < gr.t_size; k++) {
              situation check = situation(sym, "", gr.rules[j].right_part, gr.terminal[k]);
              if((find(first_s.begin(), first_s.end(), gr.terminal[k]) != first_s.end()) && (!check_situation(check, I))) { 
                I.push_back(check);
                flag = true;
              }
            }
          } 
        }
      }
    }
  }
  while(flag);
  return I;  
}

vector <situation> build_goto(vector <situation> I, char sym) {  // переход в другое состояние по символу 
  vector <situation> J;
  situation check;
  for(long i = 0; i < I.size(); i++) {
    if((I[i].right_b != "") && (I[i].right_b[0] == sym)) {
      check.left_part = I[i].left_part;
      check.right_a = I[i].right_a;
      check.right_a += sym;
      check.right_b = I[i].right_b.substr(1, I[i].right_b.size() - 1);
      check.symbol = I[i].symbol;
      J.push_back(check);
    }
  }
  return closure(J);
}

int items() {
  bool flag = true;
  vector <situation> sample; 
  sample.push_back(situation('S', "", string(1, gr.start_symbol), '$'));
  C.push_back(closure(sample));
  do {
    flag = false;
    for(long i = 0; i < C.size(); i++) {
      for(long j = 0; j < (gr.t_size + gr.n_size); j++) {
        vector <situation> check = build_goto(C[i], ((j < gr.t_size) ? gr.terminal[j] : gr.nonterminal[j - gr.t_size]));
        if((check.size() > 0) && (!check_mult_situation(check, C))) {
          C.push_back(check);
          flag = true;
        }
      }
    }   
  } while(flag);
  return 0;
}

// Окончание вспомогательных функций
    
class table {
public:
    vector < vector <cell> > t; 

    table() {
    }

    // Договоренность о порядке именования столбцов: сначала идут столбцы терминальных символов в порядке грамматики, затем нетерминальных в порядке грамматики

    table(grammar gr) {

        
        gr.nonterminal.push_back('S');                                         // пополнение грамматики, символ S - зарезервированный
        gr.rules.push_back(rule('S', string(1, gr.start_symbol)));

        items();       

        long cond_num = C.size();
        long res;

        cell ce;

        vector <cell> h(gr.t_size + gr.n_size, ce);
        t.assign(cond_num, h); 

        for(long i = 0; i < cond_num; i++) {  // проверка для каждого состояния
          for(long j = 0; j < gr.t_size; j++) { // строится таблица action
            if(check_sym(gr.terminal[j], C[i])) {  // 1-ый случай                             
              for(long k = 0; k < C.size(); k++) {  // здесь проверка принадлежности множества goto
                if(check_equality(build_goto(C[i], gr.terminal[j]), C[k])) {
                  t[i][j].ch = 's';
                  t[i][j].num = k;
                  break;
                }
              }      
            }

            // 2-ой случай разбираем в цикле

            res = check_r(C[i], gr.terminal[j]);
            if(res) {
              t[i][j].ch = 'r';
              t[i][j].num = res - 1;
            }

            if(check_situation(situation('S', string(1, gr.start_symbol), "", '$'), C[i])) {  // 3-ий случай
              t[i][gr.t_size - 1].ch = 'a';
            } 
          }
        }

        for(long i = 0; i < cond_num; i++) {
          for(long j = gr.t_size; j < (gr.t_size + gr.n_size); j++) { // строится таблица agoto
            vector <situation> check = build_goto(C[i], gr.nonterminal[j - gr.t_size]);
            for(long k = 0; k < C.size(); k++) {
              if(check_equality(check, C[k])) {
                t[i][j].num = k;
                break;
              }
            }
          }
        }
    }

    // ВНИМАНИЕ! Состояния маркируются как натуральные числа

    // тут надо будет допиливать всякие функции
    // t весь должен быть по дефолту заполнен 'e' (сделано)
    // a - accept
    // e - error
    // si - shift i
    // ri - reduce i

};

vector <long> output;

/* Начинается описание анализатора */ 

class action_return_value {
public:
    int ind; 
    /*
    возвращаемое значение
    -1 - error
    1 - shift S
    2 - reduce A->y
    3 - accept 
    по дефолту = -1
    */
    int add;
    /*
    дополнение
    символ состояния S - если  ind = 1
    номер правила грамматики - если ind = 2
    по дефолту = -1
    */
    action_return_value () {
        ind = add = -1;
    }
    action_return_value(int v_ind, int v_add) {
        ind = v_ind;
        add = v_add;
    }
};

action_return_value action(long cond, char cur, table tab) {
  action_return_value act;
  long k = -1;
  for(long i = 0; i < gr.t_size; i++) {
    if(cur == gr.terminal[i]) {
      k = i;
      break;
    }
  }
  if(k == -1) {
    return act;
  }
  char symbol = tab.t[cond][k].ch;
  printf("ACTION: %c%li \n\n", tab.t[cond][k].ch, tab.t[cond][k].num);
  if(symbol == 'e') {
    act.ind = -1;
  }
  else if(symbol == 's') {
    act.ind = 1;
    act.add = tab.t[cond][k].num;
  }
  else if(symbol == 'r') {
    act.ind = 2;
    act.add = tab.t[cond][k].num;
  }
  else if(symbol == 'a') {
    act.ind = 3;  
  }
  else {
    act.ind = -1;
  }
  return act;
}

long agoto(long cond, char cur, table tab) {   // возвращает -1 в случае ошибки, иначе  состояние
  long k = -1;
  for(long i = 0; i < gr.n_size; i++) {
    if(cur == gr.nonterminal[i]) {
      k = i + gr.t_size;
    }
  }
  return ((k == -1) ? -1 : tab.t[cond][k].num);
  return -1; 
}

int analyze_sequence(string seq, vector <char> mag, table tab) {
  char cur, i = 0;
  long cond;
  action_return_value act;
  while(true) {
    cur = seq[i];
    act = action(mag.back(), cur, tab);
    if(act.ind == -1) {
        return -1;
    }
    if(act.ind == 1) {
        mag.push_back(seq[i]);
        mag.push_back((char) (act.add));
        ++i;
    }
    if(act.ind == 2) {
        long r = gr.rules[act.add].right_part.size();
        for(long j = 0; j < (2 * r); j++) {
            mag.pop_back();
        }
        cond = agoto(mag.back(), gr.rules[act.add].left_part, tab);
        if(cond == -1) {
           return -2;
        }
        mag.push_back(gr.rules[act.add].left_part);
        mag.push_back((char) cond);
        output.push_back(act.add);  // выходная цепочка
    }
    if(act.ind == 3) {
        return 0;
    }
  }
  return -3;
}


/* Заканчивается описание анализатора */


int main() {

    char right[100];
    string right_s;
    char left;
    string seq;  // входная лента
    string res;  // выходная лента
    string ter;
    vector <char> magazine;
    char cur;
    long k;

    long exit_code, l;

    // На вход подается  грамматика

    // Построение FIRST

    printf("Задайте грамматику\nSet grammar\n");
    printf("nonterminal:\n");
    getline(cin, ter);
    for(long i = 0; i < ter.size(); i++) {
      gr.nonterminal.push_back(ter[i]);
    }
    printf("\nterminal:\n");
    ter.clear();
    getline(cin, ter);
    for(long i = 0; i < ter.size(); i++) {
      gr.terminal.push_back(ter[i]);
    }
    printf("\nstart symbol:\n");
    scanf("%c", &gr.start_symbol);
    printf("\nrules:\n");
    scanf("%li\n", &k);
    for(long i = 0; i < k; i++) {
      scanf("%c->%s", &left, &right);
      if(i < (k - 1)) {
        scanf("\n");
      }
      l = strlen(right);
      for(long j = 0; j < l; j++) {
        right_s += right[j];
      }
      gr.rules.push_back(rule(left, right_s));
      right_s.clear();
    }

    gr.t_size = gr.terminal.size();
    gr.n_size = gr.nonterminal.size();

    vector <char> v;
    first_sym.assign(gr.t_size + gr.n_size, v);

    for(long i = 0; i < gr.t_size; i++) {
      first_sym[i].push_back(gr.terminal[i]);
    }

    for(long i = 0; i < gr.n_size; i++) {
      for(long j = 0; j < gr.rules.size(); j++) {
        if((gr.rules[j].left_part == gr.nonterminal[i]) && (gr.rules[j].right_part == "e")) {
          first_sym[i + gr.t_size].push_back('e');
          break;
        }
      } 
    }

    first();

    table tab(gr);

    printf("\n\nTABLE:\n\n");

    for(long i = 0; i < C.size(); i++) {
      for(long j = 0; j < gr.t_size; j++) {
        printf("%c%li  ", tab.t[i][j].ch, tab.t[i][j].num);
      }
      printf("       ");
      for(long j = 0; j < gr.n_size; j++) {
        printf("%li  ", tab.t[i][j + gr.t_size].num);
      }
      printf("\n\n");
    } 
    magazine.push_back(0);

    printf("LR-анализатор готов к работе\nLR-analyzer is ready to perform\n\n");
    printf("Введите цепочку(Введите 0 чтобы завершить работу)\nPrint the sequence(Print 0 to end work)\n");
    cin >> seq;
    exit_code = analyze_sequence(seq, magazine, tab);
    if(exit_code == -1) {
        printf("Ошибка при работе функции action\nError during action function work\n");
    }
    if(exit_code == -2) {
        printf("Ошибка при работе функции goto\nError during goto function work\n");
    }
    if(exit_code == -3) {
        printf("Неизвестная ошибка при работе анализатора\nUnknown error during analyzer's work\n");
    }
    
	return 0;
}
