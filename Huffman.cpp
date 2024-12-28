/**************************************************************
     >> !!!Исходный текст из файла .txt в ANSI!!! <<
файл без символов 0x0D,0x0A (конец строки - конец текста)

Считаются частотности символов в целых числах:
    например 'f' встретилось 2 раза ей ставится частотность 2

Даллее строится дерево для кода Хаффмана

Далее строится СУФФИКСНЫЙ код Хаффмана с помощью DFS
    для изменения переделать .push() и .del() в codeList в конец
***************************************************************/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// АЛФАВИТЫ ===================================================
 
// исходный
const unsigned N = 33; // мощность исходного алфавита
const char* l1 = "ЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮ ";

// конечный
const unsigned Q = 2; // мощность конечного алфавита
const char* l2 = "01";

// АЛФАВИТЫ ===================================================


// список для хранения кода
// кастомный только потому, что сам как-то получился из класса ниже
class codeList
{
    // узел списка
    struct CLN {
        char c = '?';
        CLN* next = NULL;
    };

public:
    CLN* head = NULL; // ламповая голова

    // пуш в начало
    void push(char c)
    {
        CLN* tmp = new CLN;
        unsigned q = 0;
        tmp->c = c;

        tmp->next = head; // цепляем
        head = tmp;
    }

    // удаляет первый элемент
    void del()
    {
        CLN* toDel = head;
        head = head->next;
        delete toDel;
    }

    void print()
    {
        for (CLN* p = head; p != NULL; p = p->next) {
            cout << p->c;
        }
    }
};


// узел дерева
struct treeNode {
    unsigned fr = 0; // частота (сколько раз встретилось)
    char c = '!'; // исходный символ заполняется для листьев
    treeNode* links[Q] = {NULL};
};

// узел списка
struct TLN {
    treeNode* node = NULL;
    TLN* next = NULL;
};

// список деревьев
class treeList
{
public:
    TLN* head = NULL; // ламповая голова
    
    // N листьев через конструктор
    treeList(const char* l1)
    {
        for (unsigned n = 0; n < N; n++) {
            TLN* tmp = new TLN;
            tmp->node = new treeNode;

            // заполнение узла дерева
            tmp->node->fr = 0;
            tmp->node->c = l1[n];
            for (unsigned i = 0; i < Q; i++) {
                tmp->node->links[i] = NULL;
            }

            tmp->next = head; // цепляем
            head = tmp;
        }
    }

    // в начало
    void push(unsigned fr, char c)
    {
        TLN* tmp = new TLN;
        unsigned q = 0;
        tmp->node = new treeNode;

        // заполнение узла дерева
        tmp->node->fr = fr;
        tmp->node->c = c;
        for (unsigned i = 0; i < Q; i++) {
            tmp->node->links[i] = NULL;
        }

        tmp->next = head; // цепляем
        head = tmp;
    }

    // Построение вершины с частотой = сумме частот первых cntr вершин
    void sigma(unsigned cntr)
    {        
        treeNode* newNode = new treeNode; // узел, где будет сумма частот
        unsigned q = 0;
        newNode->fr = head->node->fr;
        newNode->links[q] = head->node;
        q++;
        cntr--;
        TLN* toDel; // узел попал под программу реновации
        while (cntr) {
            toDel = head;
            head = head->next;
            delete toDel;

            // получить частоту с текущего
            newNode->fr += head->node->fr;

            // прицепить к новому
            newNode->links[q] = head->node;
            q++;
            cntr--;
        }

        // подмена последнего узла
        head->node = newNode;

        // заполнение оставшихся ссылок
        while (q < Q) {
            newNode->links[q] = NULL;
            q++;
        }
    }

    // сортировка по частотам
    void fr_sort()
    {
        TLN** con = &head; // магия
        TLN* p1 = head; // указатель
        TLN* p2 = head->next; // указатель
        TLN* brdr = NULL; // барьер
        while (p2 != brdr) {
            while (p2 != brdr) {
                if (p1->node->fr > p2->node->fr) {
                    p1->next = p2->next;
                    p2->next = p1;
                    *con = p2;

                    con = &p2->next;
                    p2 = p1->next;
                }
                else {
                    con = &p1->next;
                    p1 = p2;
                    p2 = p2->next;
                }
            }

            brdr = p1; // стена двигается к началу
            con = &head;
            p1 = head;
            p2 = head->next;
        }
    }

    //// сортировка по символам
    //void c_sort()
    //{
    //    TLN** con = &head; // магия
    //    TLN* p1 = head;
    //    TLN* p2 = head->next;
    //    TLN* brdr = NULL; // барьер
    //    while (p2 != brdr) {
    //        while (p2 != brdr) {
    //            if (p1->node->c > p2->node->c) {
    //                p1->next = p2->next;
    //                p2->next = p1;
    //                *con = p2;

    //                // к сравнению следующих 2х
    //                con = &p2->next;
    //                p2 = p1->next;
    //            }
    //            else {
    //                // к сравнению следующих 2х
    //                con = &p1->next;
    //                p1 = p2;
    //                p2 = p2->next;
    //            }
    //        }

    //        brdr = p1; // сдвиг барьера
    //        con = &head;
    //        p1 = head;
    //        p2 = head->next;
    //    }
    //}

    void print()
    {
        for (TLN* p = head; p != NULL; p = p->next) {
            cout << p->node->c << ' ' << p->node->fr << endl;
        }
    }

    unsigned size()
    {
        unsigned size = 0;
        for (TLN* p = head; p != NULL; p = p->next)
            size++;

        return size;
    }

    // Построение кодов через обход в глубину
    codeList code; // тут формируется код символа
    void spcl_DFS(treeNode* p, char c = ';')
    {
        if (p) {
            code.push(c); // добавить к коду
            if (p->links[0]) {
                for (unsigned q = 0; q < Q; q++) {
                    spcl_DFS(p->links[q], l2[q]);
                }
            }
            else {
                // лист - вывод кода
                cout << p->c << " ";
                code.print();
                cout << endl;
            }
            code.del(); // убрать добавленное
        }
    }

    /*void swap(treeListNode* n1, treeListNode* n2) 
    {
        char c_tmp = n1->node->c;
        n1->node->c = n2->node->c;
        n2->node->c = c_tmp;

        unsigned fr_tmp = n1->node->fr;
        n1->node->fr = n2->node->fr;
        n2->node->fr = fr_tmp;

        treeNode* link_tmp;
        for (unsigned i = 0; i < Q; i++) {
            link_tmp = n1->node->links[i];
            n1->node->links[i] = n2->node->links[i];
            n2->node->links[i] = link_tmp;
        }
    }*/
};

int main()
{
    setlocale(LC_ALL, "rus");

    // чтение исходного текста в строку
    ifstream inF("file.txt");
    string txt;
    getline(inF, txt);
    inF.close();

    // СОЗДАНИЕ ЛИСТЬЕВ:
    treeList lst(l1); // создание списка

    // определение частот
    for (unsigned i = 0; i < txt.size(); i++) {
        // поиск символа txt[i] в списке
        bool nFnd = 1; // буква не найдена
        for (TLN* p = lst.head; nFnd && (p != NULL); p = p->next) {
            if (txt[i] == p->node->c) {
                p->node->fr++;
                nFnd = 0;
            }   
        }

        // в списке нет символа txt[i]
        if (nFnd)
            cout << "в исходном алфавите нет символа '" << txt[i] << "'" << endl;
    }

    // собственно магия построения дерева из списка ===========
    
    // первый шаг
    lst.fr_sort();
    //lst.print();
    unsigned rem = N % (Q - 1);
    if ((Q == 2) || (rem == 1)) {
        lst.sigma(Q);
    }
    else if (rem == 0){
        lst.sigma(Q - 1);
    }
    else {
        lst.sigma(rem);
    }

    // пока в списке не останется 1 вершина
    while (lst.size() != 1) {
        lst.fr_sort();
        lst.sigma(Q);
    }

    // ========================================================

    lst.spcl_DFS(lst.head->node);
}
