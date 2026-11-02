#include <iostream>
#include <cmath>

using namespace std;

// 前置宣告 Polynomial 類別，供 Term 定義 friend*(
class Polynomial;

// Term 類別：表示多項式中的單一項
class Term {
    friend class Polynomial; // 允許 Polynomial 存取私有變數
    friend istream& operator>>(istream& is, Polynomial& poly);
    friend ostream& operator<<(ostream& os, const Polynomial& poly);
private:
    float coef; // 係數
    int exp;    // 指數
};

class Polynomial {
    friend istream& operator>>(istream& is, Polynomial& poly);
    friend ostream& operator<<(ostream& os, const Polynomial& poly);

private:
    Term* termArray; // 儲存非零項的動態陣列
    int capacity;    // 動態陣列總容量
    int terms;       // 目前儲存的非零項個數

    // 內部函式：新增單一項跟 容量不足時擴容
    void NewTerm(float theCoef, int theExp) {
        if (theCoef == 0.0f) return; 

        // 當陣列滿時，將容量擴展為原來的 2 倍
        if (terms == capacity) {
            capacity *= 2;
            Term* temp = new Term[capacity];
            for (int i = 0; i < terms; i++) {
                temp[i] = termArray[i];
            }
            delete[] termArray;
            termArray = temp;
        }

        termArray[terms].coef = theCoef;
        termArray[terms].exp = theExp;
        terms++;
    }

public:
    // 預設建構子：初始化為零多項式
    Polynomial() {
        capacity = 2;//初始容量2
        terms = 0;
        termArray = new Term[capacity];
    }

    Polynomial(const Polynomial& poly) {
        capacity = poly.capacity;
        terms = poly.terms;
        termArray = new Term[capacity];
        for (int i = 0; i < terms; i++) {
            termArray[i] = poly.termArray[i];
        }
    }

    // 解構
    ~Polynomial() {
        delete[] termArray;
    }

    // 賦值運算子多載（深拷貝）**
    Polynomial& operator=(const Polynomial& poly) {
        if (this == &poly) return *this;
        delete[] termArray;
        capacity = poly.capacity;
        terms = poly.terms;
        termArray = new Term[capacity];
        for (int i = 0; i < terms; i++) {
            termArray[i] = poly.termArray[i];
        }
        return *this;
    }

    Polynomial Add(Polynomial poly) {
        Polynomial c;
        int aPos = 0;
        int bPos = 0;

        // 雙指標逐項比對指數大小（前提：項目皆按指數由大到小排序）
        while (aPos < terms && bPos < poly.terms) {
            if (termArray[aPos].exp == poly.termArray[bPos].exp) {
                float sum = termArray[aPos].coef + poly.termArray[bPos].coef;
                if (sum != 0.0f) {
                    c.NewTerm(sum, termArray[aPos].exp);
                }
                aPos++;
                bPos++;
            }
            else if (termArray[aPos].exp > poly.termArray[bPos].exp) {
                c.NewTerm(termArray[aPos].coef, termArray[aPos].exp);
                aPos++;
            }
            else {
                c.NewTerm(poly.termArray[bPos].coef, poly.termArray[bPos].exp);
                bPos++;
            }
        }

        // 加入剩餘的項
        for (; aPos < terms; aPos++) c.NewTerm(termArray[aPos].coef, termArray[aPos].exp);
        for (; bPos < poly.terms; bPos++) c.NewTerm(poly.termArray[bPos].coef, poly.termArray[bPos].exp);

        return c;
    }

    Polynomial Mult(Polynomial poly) {
        Polynomial c;

        // 遍歷 this 的每一項，分別與 poly 的每一項相乘
        for (int i = 0; i < terms; i++) {
            Polynomial temp;
            for (int j = 0; j < poly.terms; j++) {
                float newCoef = termArray[i].coef * poly.termArray[j].coef;
                int newExp = termArray[i].exp + poly.termArray[j].exp;
                temp.NewTerm(newCoef, newExp);
            }
            // 累加乘積並合併同類項
            c = c.Add(temp);
        }

        return c;
    }

    // 代入求值：計算 p(f)
    float Eval(float f) {
        float ans = 0.0f;
        for (int i = 0; i < terms; i++) {
            ans += termArray[i].coef * pow(f, termArray[i].exp);
        }
        return ans;
    }
};

// 多載輸入運算子 
istream& operator>>(istream& is, Polynomial& poly) {
    int n;
    cout << "輸入非零項的數量: ";
    if (!(is >> n)) return is;

    // 清空現有項
    poly.terms = 0;

    cout << "請依序輸入各項的 [係數 指數]（依指數由大到小排序）:\n";
    for (int i = 0; i < n; i++) {
        float c;
        int e;
        is >> c >> e;
        poly.NewTerm(c, e);
    }
    return is;
}

// 多載輸出運算子 
ostream& operator<<(ostream& os, const Polynomial& poly) {
    if (poly.terms == 0) {
        os << "0";
        return os;
    }

    for (int i = 0; i < poly.terms; i++) {
        float c = poly.termArray[i].coef;
        int e = poly.termArray[i].exp;

        // 輸出正負號與間隔
        if (i > 0) {
            if (c > 0) os << " + ";
            else os << " - ";
        }
        else {
            if (c < 0) os << "-";
        }

        // 取得係數絕對值做格式化呈現
        float absC = abs(c);
        if (e == 0) {
            os << absC;
        }
        else if (e == 1) {
            if (absC == 1.0f) os << "x";
            else os << absC << "x";
        }
        else {
            if (absC == 1.0f) os << "x^" << e;
            else os << absC << "x^" << e;
        }
    }
    return os;
}

// 測試與驗證主程式
int main() {
    Polynomial p1, p2;

    cout << "=== 輸入第一個多項式 p1 ===" << endl;
    cin >> p1;

    cout << "\n=== 輸入第二個多項式 p2 ===" << endl;
    cin >> p2;

    cout << "\n----------------------------------------" << endl;
    cout << "p1(x) = " << p1 << endl;
    cout << "p2(x) = " << p2 << endl;

    // 測試加法
    Polynomial pSum = p1.Add(p2);
    cout << "\n[Add] p1(x) + p2(x) = " << pSum << endl;

    // 測試乘法
    Polynomial pMult = p1.Mult(p2);
    cout << "[Mult] p1(x) * p2(x) = " << pMult << endl;

    // 測試代入求值
    float x;
    cout << "\n請輸入欲代入的數值 x: ";
    cin >> x;
    cout << "p1(" << x << ") = " << p1.Eval(x) << endl;
    cout << "p2(" << x << ") = " << p2.Eval(x) << endl;

    return 0;
} 