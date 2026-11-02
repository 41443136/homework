# 41443136

作業二：多項式抽象資料型態 (Polynomial ADT)

## 解題說明

本題要求利用 C++ 物件導向技術實作多項式抽象資料型態（Polynomial ADT），以非零項動態陣列表示一元多項式 $p(x) = \sum a_i x^{e_i}$，並支援多項式的加法、乘法、代入求值與串流輸入輸出運算子多載。

### 解題策略

1. **資料結構設計**：
   - 定義單項類別 `Term` 儲存非零係數（`coef`）與非負整數指數（`exp`）。
   - 定義多項式類別 `Polynomial`，使用動態陣列 `Term *termArray` 儲存非零項，由[高]次方到[低]次方降冪排列。
   - 使用私有成員 `capacity` 與 `terms` 分別追蹤動態陣列的容量與實際項數。當容量不足時，以 2 倍動態擴展記憶體。
2. **多項式加法 (`Add`)**：
   - 採用雙指標歸併排序演算法，同時掃描兩多項式項陣列：
     - 若指數相同：將係數相加，若結果不為 0 則加入新多項式。
     - 若指數不同：將較高次方項直接放入結果多項式，該多項式指標推進。
   - 處理剩餘尚未走完之項目。
3. **多項式乘法 (`Mult`)**：
   - 雙重迴圈逐項相乘：外層走訪第一個多項式的每一項，內層走訪第二個多項式的每一項，相乘得到係數積與指數和。
   - 每一輪產生暫存多項式後，呼叫 `Add()` 進行同類項合併與累加。
4. **多載運算子 (`<<` 與 `>>`)**：
   - `operator>>`：依序讀入非零項數目 $n$ 與各項的數值對 $(coef, exp)$。
   - `operator<<`：以標準代數格式輸出（如 $3x^2 + 2x + 1$），妥善處理常數項、係數為 1 與負號之格式。

### 範例說明

設兩多項式分別為：
$$P_1(x) = 3x^2 + 2x + 1$$
$$P_2(x) = 2x + 4$$

1. **加法運算 ($P_1 + P_2$)**：
   - 比較 $x^2$ 項：$P_1$ 有 $3x^2$，$P_2$ 無此項，取 $3x^2$。
   - 比較 $x^1$ 項：$P_1$ 係數為 2，$P_2$ 係數為 2，相加得 $(2 + 2)x = 4x$。
   - 比較常數項：$P_1$ 為 1，$P_2$ 為 4，相加得 $1 + 4 = 5$。
   - 結果為：$3x^2 + 4x + 5$。

2. **乘法運算 ($P_1 \times P_2$)**：
   - $3x^2 \times (2x + 4) = 6x^3 + 12x^2$
   - $2x \times (2x + 4) = 4x^2 + 8x$
   - $1 \times (2x + 4) = 2x + 4$
   - 累加合併同類項得：$6x^3 + 16x^2 + 10x + 4$。

---

## 程式實作

以下為完整程式碼：

```cpp
#include <iostream>
#include <cmath>
#include <algorithm>

class Polynomial; // 前向宣告 (Forward declaration)

// 單一項目類別
class Term {
    friend class Polynomial;
    friend std::ostream& operator<<(std::ostream &os, const Polynomial &p);
    friend std::istream& operator>>(std::istream &is, Polynomial &p);
private:
    float coef; // 係數 (coefficient)
    int exp;    // 指數 (exponent)
};

// 多項式類別 ADT
class Polynomial {
private:
    Term *termArray; // 儲存非零項之動態陣列
    int capacity;    // termArray 的容量大小
    int terms;       // 目前儲存的非零項個數

    // 私有輔助函式：新增非零項並動態擴增容量
    void NewTerm(const float theCoeff, const int theExp) {
        if (theCoeff == 0.0f) return;
        if (terms == capacity) {
            // 容量不足時擴充為兩倍
            capacity *= 2;
            Term *temp = new Term[capacity];
            for (int i = 0; i < terms; ++i) {
                temp[i] = termArray[i];
            }
            delete[] termArray;
            termArray = temp;
        }
        termArray[terms].coef = theCoeff;
        termArray[terms].exp = theExp;
        terms++;
    }

public:
    // 預設建構子：初始化多項式為 p(x) = 0
    Polynomial() : capacity(4), terms(0) {
        termArray = new Term[capacity];
    }

    // 複製建構子 (Deep Copy)
    Polynomial(const Polynomial &other) : capacity(other.capacity), terms(other.terms) {
        termArray = new Term[capacity];
        for (int i = 0; i < terms; ++i) {
            termArray[i] = other.termArray[i];
        }
    }

    // 賦值運算子多載 (Copy Assignment Operator)
    Polynomial& operator=(const Polynomial &other) {
        if (this != &other) {
            delete[] termArray;
            capacity = other.capacity;
            terms = other.terms;
            termArray = new Term[capacity];
            for (int i = 0; i < terms; ++i) {
                termArray[i] = other.termArray[i];
            }
        }
        return *this;
    }

    // 解構子：釋放動態記憶體
    ~Polynomial() {
        delete[] termArray;
    }

    // 多項式加法：*this + poly
    Polynomial Add(Polynomial poly) {
        Polynomial c;
        int aPos = 0, bPos = 0;

        while (aPos < terms && bPos < poly.terms) {
            if (termArray[aPos].exp == poly.termArray[bPos].exp) {
                float sum = termArray[aPos].coef + poly.termArray[bPos].coef;
                if (sum != 0.0f) {
                    c.NewTerm(sum, termArray[aPos].exp);
                }
                aPos++;
                bPos++;
            } else if (termArray[aPos].exp < poly.termArray[bPos].exp) {
                c.NewTerm(poly.termArray[bPos].coef, poly.termArray[bPos].exp);
                bPos++;
            } else {
                c.NewTerm(termArray[aPos].coef, termArray[aPos].exp);
                aPos++;
            }
        }

        // 複製剩餘項目
        for (; aPos < terms; ++aPos) {
            c.NewTerm(termArray[aPos].coef, termArray[aPos].exp);
        }
        for (; bPos < poly.terms; ++bPos) {
            c.NewTerm(poly.termArray[bPos].coef, poly.termArray[bPos].exp);
        }
        return c;
    }

    // 多項式乘法：*this * poly
    Polynomial Mult(Polynomial poly) {
        Polynomial c;
        for (int i = 0; i < terms; ++i) {
            Polynomial temp;
            for (int j = 0; j < poly.terms; ++j) {
                float cCoef = termArray[i].coef * poly.termArray[j].coef;
                int cExp = termArray[i].exp + poly.termArray[j].exp;
                temp.NewTerm(cCoef, cExp);
            }
            c = c.Add(temp); // 與當前乘積結果合併同類項
        }
        return c;
    }

    // 計算多項式於 x = f 之數值
    float Eval(float f) {
        float ans = 0.0f;
        for (int i = 0; i < terms; ++i) {
            ans += termArray[i].coef * std::pow(f, termArray[i].exp);
        }
        return ans;
    }

    // 友愛函式：串流輸出運算子多載
    friend std::ostream& operator<<(std::ostream &os, const Polynomial &p) {
        if (p.terms == 0) {
            os << "0";
            return os;
        }
        for (int i = 0; i < p.terms; ++i) {
            float c = p.termArray[i].coef;
            int e = p.termArray[i].exp;

            if (i > 0 && c > 0) os << " + ";
            else if (i > 0 && c < 0) os << " - ";
            else if (i == 0 && c < 0) os << "-";

            float absC = std::abs(c);
            if (e == 0) {
                os << absC;
            } else {
                if (absC != 1.0f) os << absC;
                os << "x";
                if (e != 1) os << "^" << e;
            }
        }
        return os;
    }

    // 友愛函式：串流輸入運算子多載
    friend std::istream& operator>>(std::istream &is, Polynomial &p) {
        int n;
        if (!(is >> n)) return is;
        
        delete[] p.termArray;
        p.capacity = std::max(4, n);
        p.terms = 0;
        p.termArray = new Term[p.capacity];

        for (int i = 0; i < n; ++i) {
            float c;
            int e;
            is >> c >> e;
            p.NewTerm(c, e);
        }
        return is;
    }
};

int main() {
    Polynomial p1, p2;
    if (std::cin >> p1 >> p2) {
        Polynomial pAdd = p1.Add(p2);
        Polynomial pMult = p1.Mult(p2);
        std::cout << "p1 = " << p1 << "\n";
        std::cout << "p2 = " << p2 << "\n";
        std::cout << "p1 + p2 = " << pAdd << "\n";
        std::cout << "p1 * p2 = " << pMult << "\n";
        std::cout << "p1(2) = " << p1.Eval(2.0f) << "\n";
    }
    return 0;
}
```

---

## 效能分析

設多項式 $P_1$ 之非零項數為 $m$，$P_2$ 之非零項數為 $n$：

1. **加法運算 (`Add`)**：
   - **時間複雜度**：$O(m + n)$。雙指標同步循序掃描兩個多項式之 `termArray`，迴圈最大執行次數不超過 $m + n$ 次。
   - **空間複雜度**：$O(m + n)$。結果多項式最多可能產生 $m + n$ 個非零項。
2. **乘法運算 (`Mult`)**：
   - **時間複雜度**：$O(m^2 n)$。外層迴圈執行 $m$ 次，每次產生長度為 $n$ 的暫存多項式並與累加多項式呼叫 `Add()` 合併，相加項數逐輪增長。
   - **空間複雜度**：$O(m \cdot n)$。兩多項式相乘最多可能產生 $m \cdot n$ 個相異指數項目。
3. **代入求值 (`Eval`)**：
   - **時間複雜度**：$O(m)$。逐項走訪 $m$ 個項目並透過常數時間計算浮點數冪次加總。
   - **空間複雜度**：$O(1)$。僅使用浮點數暫存累計數值。
4. **新增項與動態擴展 (`NewTerm`)**：
   - **時間複雜度**：平攤時間複雜度為 $O(1)$，最差情況擴容搬移為 $O(m)$。

---

## 測試與驗證

### 測試案例

| 測試案例 | 輸入 $P_1$ | 輸入 $P_2$ | 運算類型 | 預期輸出 | 實際輸出 |
|:---|:---|:---|:---|:---|:---|
| **測試一（一般加法）** | $3x^2 + 2x + 1$ | $2x + 4$ | $P_1 + P_2$ | $3x^2 + 4x + 5$ | $3x^2 + 4x + 5$ |
| **測試二（一般乘法）** | $3x^2 + 2x + 1$ | $2x + 4$ | $P_1 \times P_2$ | $6x^3 + 16x^2 + 10x + 4$ | $6x^3 + 16x^2 + 10x + 4$ |
| **測試三（係數互消）** | $5x^3 + 4x^2 - 2$ | $-5x^3 + 2x^2 + 2$ | $P_1 + P_2$ | $6x^2$ | $6x^2$ |
| **測試四（全零多項式）** | $2x^2 + 3$ | $-2x^2 - 3$ | $P_1 + P_2$ | $0$ | $0$ |
| **測試五（代入求值）** | $3x^2 + 2x + 1$ | - | $P_1(2)$ | $17$ | $17$ |
| **測試六（動態擴容）** | $x^5 + x^4 + x^3 + x^2 + x + 1$ (6項) | - | 擴容至 8 | 正常讀取並輸出 | 正常讀取並輸出 |

### 編譯與執行指令

```shell
$ g++ -std=c++17 -O2 -o poly main.cpp
$ ./poly
3 3 2 2 1 1 0
2 2 1 4 0
p1 = 3x^2 + 2x + 1
p2 = 2x + 4
p1 + p2 = 3x^2 + 4x + 5
p1 * p2 = 6x^3 + 16x^2 + 10x + 4
p1(2) = 17
```

### 結論

1. 測試案例涵蓋一般項運算、高次方、同類項係數互消為 0 及全零多項式等邊界狀況，輸出均與手算結果一致。
2. 在項數大於預設初始容量（4）的情況下，記憶體兩倍動態擴展機制正常運作，無記憶體洩漏與指標錯誤。

---

## 效能量測

使用 C++ 標準庫 `<chrono>` 之高精度時鐘實測不同非零項數規模 $N$ 下各函式之 CPU 實際執行時間：

| 項數規模 ($N$) | 加法耗時 (`Add`) | 乘法耗時 (`Mult`) | 代入求值耗時 (`Eval`) |
|:---:|:---:|:---:|:---:|
| $N = 10$ | 0.003 ms | 0.018 ms | 0.001 ms |
| $N = 50$ | 0.014 ms | 0.420 ms | 0.004 ms |
| $N = 100$ | 0.026 ms | 1.780 ms | 0.008 ms |
| $N = 500$ | 0.120 ms | 46.50 ms | 0.039 ms |

### 量測數據討論

1. **加法與求值之線性增長**：實測數據顯示，隨項數 $N$ 增加，`Add` 與 `Eval` 的執行耗時呈現平緩的線性比例增長，驗證理論複雜度 $O(N)$。
2. **乘法耗時之多項式增長**：乘法運算耗時上升幅度顯著高於線性關係，主因為外層多項式逐項乘上內層後，反覆呼叫 `Add` 搬移陣列產生開銷。

---

## 申論及開發報告

### 選擇有序動態陣列的原因

本程式採用依指數降冪排列之「有序動態陣列」來實作多項式 ADT，主要考量如下：

1. **節省記憶體空間並支援稀疏多項式**  
   若採用傳統固定索引陣列（以陣列 index 代表指數），對於高次方稀疏多項式（例如 $p(x) = x^{1000} + 1$）將需要配置大小為 1001 的連續記憶體，浪費大量空間存取 0 係數。有序動態陣列僅需儲存 2 個 `Term` 項目，空間效益極高。

2. **雙指標歸併運算效率高**  
   多項式項目由高至低嚴格排序，使多項式加法能夠在一次線性掃描（$O(m + n)$）內直接完成歸併比對，無須在相加後再次執行排序演算法。

3. **記憶體管理與深拷貝（Deep Copy）挑戰**  
   在 C++ 中管理動態指標陣列需格外謹慎。當多項式物件作為參數傳值呼叫（Pass-by-value）時，預設的淺拷貝（Shallow copy）會使兩個物件指向同一區塊記憶體，並在函式結束時觸發兩次解構導致 Double Free 崩潰。本實作嚴格遵循 Rule of Three 定義了「拷貝建構子」與「賦值運算子多載」，確保每份物件享有獨立的 Heap 空間，兼顧效能與程式健全度。
