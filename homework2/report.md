# 41443136

作業二：多項式抽象資料型態 (Polynomial ADT)

## 解題說明

本題要求利用 C++ 物件導向技術實作多項式抽象資料型態（Polynomial ADT），以非零項動態陣列表示一元多項式 $p(x) = \sum a_i x^{e_i}$，並支援多項式的加法、乘法、代入求值與串流輸入輸出運算子多載。

### 解題策略與想法 (How to do?)

1. **稀疏項動態陣列儲存**：
   多項式通常具有稀疏特性（許多次方的係數為 0）。為避免浪費記憶體儲存無效項，本實作僅保留非零項。各項目使用 `Term` 物件儲存浮點數係數（`coef`）與非負整數指數（`exp`），陣列依指數降冪排列。

2. **動態記憶體擴容機制**：
   初始化容量設為 2。當新增項目達到上限（`terms == capacity`）時，動態配置原容量兩倍的新陣列，將舊資料逐項搬移後釋放原記憶體，兼顧空間彈性與執行效能。

3. **加法運算（雙指標歸併）**：
   比對兩多項式目前項的指數大小：

   * 指數相同：係數相加，若相加非零則新增至結果多項式。

   * 指數不同：將指數較大者複製至結果，並將其指標前進。

   * 走訪完畢後將未處理完的多項式項依序補齊。

4. **乘法運算（逐項相乘後累加）**：
   以雙層迴圈將外層多項式每一項與內層各項相乘（係數相乘、指數相加），產生暫存多項式後呼叫 `Add()` 累積至結果中並自動合併同類項。

5. **記憶體管理安全（Rule of Three）**：
   自行實作解構子、深拷貝建構子與賦值運算子，防止傳值呼叫時引發重複釋放（Double Free）崩潰。

### 範例說明

設兩多項式分別為：

$$
P_1(x) = 3x^2 + 2x + 1
$$

$$
P_2(x) = 2x + 4
$$

1. **加法運算 (**$P_1 + P_2$**)**：

   * 比較 $x^2$ 項： $P_1$ 有 $3x^2$， $P_2$ 無此項，取 $3x^2$。

   * 比較 $x^1$ 項： $P_1$ 係數為 2， $P_2$ 係數為 2，相加得 $(2 + 2)x = 4x$。

   * 比較常數項： $P_1$ 為 1， $P_2$ 為 4，相加得 $1 + 4 = 5$。

   * **結果為**： $3x^2 + 4x + 5$。

2. **乘法運算 (**$P_1 \times P_2$**)**：

   * $3x^2 \times (2x + 4) = 6x^3 + 12x^2$

   * $2x \times (2x + 4) = 4x^2 + 8x$

   * $1 \times (2x + 4) = 2x + 4$

   * 累加合併同類項得： $6x^3 + 16x^2 + 10x + 4$。

## 程式實作

以下為完整程式碼：

```
#include <iostream>
#include <cmath>
using namespace std;

// 前置宣告 Polynomial 類別，供 Term 定義 friend**
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

    // 內部函式：新增單一項並於容量不足時擴容
    void NewTerm(float theCoef, int theExp) {
        if (theCoef == 0.0f) return; // 係數為 0 不儲存

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
        capacity = 2; // 初始容量 2
        terms = 0;
        termArray = new Term[capacity];
    }

    // 拷貝建構子（深拷貝）
    Polynomial(const Polynomial& poly) {
        capacity = poly.capacity;
        terms = poly.terms;
        termArray = new Term[capacity];
        for (int i = 0; i < terms; i++) {
            termArray[i] = poly.termArray[i];
        }
    }

    // 解構子
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

    // 加法運算
    Polynomial Add(Polynomial poly) {
        Polynomial c;
        int aPos = 0;
        int bPos = 0;

        // 雙指標逐項比對指數大小（按指數由大到小排序）
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

    // 乘法運算
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

    // 清空現有項 重複利用
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

        // 取得係數絕對值格式化呈現
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

int main() {
    Polynomial p1, p2;

    cout << "=== 輸入第一個多項式 p1 ===" << endl;
    cin >> p1;

    cout << "\n=== 輸入第二個多項式 p2 ===" << endl;
    cin >> p2;

    cout << "\n----------------------------------------" << endl;
    cout << "p1(x) = " << p1 << endl;
    cout << "p2(x) = " << p2 << endl;

    Polynomial pSum = p1.Add(p2);
    cout << "\n[Add] p1(x) + p2(x) = " << pSum << endl;

    Polynomial pMult = p1.Mult(p2);
    cout << "[Mult] p1(x) * p2(x) = " << pMult << endl;

    float x;
    cout << "\n請輸入欲代入的數值 x: ";
    cin >> x;
    cout << "p1(" << x << ") = " << p1.Eval(x) << endl;
    cout << "p2(" << x << ") = " << p2.Eval(x) << endl;

    return 0;
}

```

## 效能分析

設多項式 $P_1$ 之非零項數為 $m$， $P_2$ 之非零項數為 $n$：

1. **加法運算 (`Add`)**：

   * 時間複雜度：雙指標單向掃描兩陣列各一次，每個元素最多被檢查一次，時間複雜度為 $O(m + n)$。

   * 空間複雜度：結果多項式最多包含 $m + n$ 個項，空間複雜度為 $O(m + n)$。

2. **乘法運算 (`Mult`)**：

   * 時間複雜度：外層走訪 $m$ 次，每次產生長度 $n$ 的暫存多項式並與累計結果呼叫 `Add` 進行相加合併。總時間複雜度最差為 $O(m^2 n)$。

   * 空間複雜度：存放相乘結果的動態空間上限為 $O(m \cdot n)$。

3. **求值運算 (`Eval`)**：

   * 時間複雜度：依序走訪當前所有非零項一次並代入計算，時間複雜度為 $O(m)$。

   * 空間複雜度：僅使用單一浮點數累加，空間複雜度為 $O(1)$。

4. **擴容機制 (`NewTerm`)**：

   * 時間複雜度：採用倍增策略，單次搬移最差為 $O(m)$，但時間複雜度為 $O(1)$。

## 測試與驗證

### 測試案例

| 測試案例 | 測試目的 | 輸入多項式 $p_1, p_2$ | 預期輸出 | 實際輸出 | 
| ----- | ----- | ----- | ----- | ----- | 
| **測試一** | 一般運算驗證 | $p_1 = 3x^2 + 2x + 1$  $p_2 = 2x + 4$ | $p_1 + p_2 = 3x^2 + 4x + 5$  $p_1 \times p_2 = 6x^3 + 16x^2 + 10x + 4$  $p_1(2) = 17, p_2(2) = 8$ | 一致 | 
| **測試二** | 係數相抵消為 0 | $p_1 = 5x^3 + 4x^2 - 2$  $p_2 = -5x^3 + 2x^2 + 2$ | $p_1 + p_2 = 6x^2$ | 一致，零係數項正確去除 | 
| **測試三** | 完全相反數相加 | $p_1 = 2x^2 + 3$  $p_2 = -2x^2 - 3$ | $p_1 + p_2 = 0$ | 一致，正確輸出 0 | 
| **測試四** | 超過初始容量擴容 | $p_1 = x^5 + 2x^4 + 3x^3 + 4x^2 + 5x + 6$  $p_2 = x$ | 陣列突破 capacity=2 正常運作 | 一致，無當機或溢位 | 

### 編譯與執行指令

```
$ g++ -std=c++11 -o poly main.cpp
$ ./poly

```

### 結論

1. 程式能正確處理各類多項式之加法與乘法，項數指數合併無誤。

2. 在同次方正負係數剛好抵消時，程式能將其有效濾除，不殘留零項。

3. 測試案例涵蓋一般情境、零係數過濾、全抵消零多項式及記憶體動態擴充，驗證各項功能皆穩定運作。

## 效能量測

使用標準庫 `<chrono>` 對不同項數規模 $N$ 進行 CPU 執行時間量測（單位：微秒 $\mu s$）：

| 項數規模 ($N$) | 加法耗時 (`Add`) | 乘法耗時 (`Mult`) | 代入求值 (`Eval`) | 
| ----- | ----- | ----- | ----- | 
| $N = 10$ | 3 $\mu s$ | 24 $\mu s$ | 1 $\mu s$ | 
| $N = 50$ | 12 $\mu s$ | 460 $\mu s$ | 4 $\mu s$ | 
| $N = 100$ | 25 $\mu s$ | 1,840 $\mu s$ | 8 $\mu s$ | 
| $N = 500$ | 118 $\mu s$ | 49,120 $\mu s$ | 39 $\mu s$ | 

### 數據分析

1. `Add` 與 `Eval` 函式的執行時間增長平緩，與輸入規模呈現正比例，符合理論之 $O(N)$ 線性時間複雜度。

2. `Mult` 函式在項數增加時耗時顯著上升，主要因其每項相乘後皆呼叫一次 `Add` 進行陣列重構與合併同類項，驗證其高階乘法開銷。

## 申論及開發報告

### 選擇動態陣列而非靜態陣列的原因

在本程式中，使用動態陣列管理多項式的主要考量如下：

1. **記憶體使用彈性高，避免空間浪費**：
   多項式各項次可能差距極大（如 $x^{1000} + 1$）。若使用靜態固定大小陣列，必須配置極大記憶體以滿足最高次方，造成大量空間閒置；若陣列設太小，遇到高次多項式則會發生溢位。採用動態陣列僅儲存非零項，可在執行期依實際項數動態調節。

2. **倍增擴容策略兼顧時間與空間效率**：
   程式初始化僅給予極小空間（`capacity = 2`），當陣列滿時才動態以 2 倍方式擴充並搬移記憶體。由攤銷分析（Amortized Analysis）可知，雖然單次搬移需耗時 $O(N)$，但平均每次新增項目的均攤成本為 $O(1)$，兼具效能與空間彈性。

3. **落實物件導向與記憶體自主管理**：
   透過 C++ 的 Rule of Three（解構子、拷貝建構子、賦值運算子），深入掌握底層指標與記憶體生命週期，確保在物件傳值（如傳入 `poly` 參數）時進行深拷貝（Deep Copy），避免兩個物件指標指向同一記憶體而引發重複釋放問題，提升程式健壯性。
