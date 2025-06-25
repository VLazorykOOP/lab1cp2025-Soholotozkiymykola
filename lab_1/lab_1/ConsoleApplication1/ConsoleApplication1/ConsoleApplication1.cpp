#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <string>
using namespace std;

// ======== ВИНЯТКИ =========

// Клас для обробки помилок, коли значення x або y виходить за межі [-10, 10]
class ErrorRange {
    string str = "ErrorRange";
public:
    double value;
    ErrorRange(double val) : value(val) {}
    void Message() {
        cout << "ErrorRange: x or y out of range: " << value << " (expected -10 <= x <= 10)" << endl;
    }
};

// Клас для обробки помилки відкриття файлу
class ErrorNoFile {
    string filename;
public:
    ErrorNoFile(const string& file) : filename(file) {}
    void Message() {
        cout << "ErrorNoFile: cannot open file " << filename << endl;
    }
};

// Базовий клас помилки для логічної або математичної помилки
class ErrorKrl {
    string str = "ErrorKrl";
protected:
    double x;
public:
    ErrorKrl(double dx) : x(dx) {}
    virtual void Message() {
        cout << "ErrorKrl: invalid computation at x = " << x << endl;
    }
    virtual double setFun() {
        return x + 1;
    }
};

// Похідний клас, що розширює ErrorKrl, додаючи y і змінене повідомлення
class ErrorDKrl : public ErrorKrl {
    string str = "ErrorDKrl";
    double y;
public:
    ErrorDKrl(double dx, double dy) : ErrorKrl(dx), y(dy) {}
    void Message() override {
        cout << "ErrorDKrl: invalid text or logic x = " << x << ", y = " << y << endl;
    }
    double setFun() override {
        return x + y;
    }
};

// ======= ДАНІ ТА ІНТЕРПОЛЯЦІЯ =======

// Структура для зберігання пари значень x та value
struct DataRow {
    double x;
    double value;
};

// Функція для завантаження даних з файлу у вектор структур DataRow
vector<DataRow> loadData(const string& filename) {
    vector<DataRow> data;
    ifstream file(filename);
    if (!file.is_open()) throw ErrorNoFile(filename);

    double x, val;
    while (file >> x >> val)
        data.push_back({ x, val });

    return data;
}

// Лінійна інтерполяція по значенню x
double interpolate(double x, const vector<DataRow>& data) {
    if (x < data.front().x || x > data.back().x)
        throw ErrorRange(x);

    for (size_t i = 1; i < data.size(); ++i) {
        if (x <= data[i].x) {
            double x0 = data[i - 1].x, y0 = data[i - 1].value;
            double x1 = data[i].x, y1 = data[i].value;
            return y0 + (y1 - y0) * (x - x0) / (x1 - x0); // Формула лінійної інтерполяції
        }
    }

    return data.back().value; // Якщо x точно дорівнює останньому значенню
}

// ======= ТЕКСТОВА ФУНКЦІЯ ==========

// Перетворення тексту у числове значення
double Gtext(const string& word) {
    if (word == "set") return 1.0;
    if (word == "get") return 2.0;
    if (word == "aet") return 1.0;
    if (word == "vet") return 2.0;
    if (word == "zet") return 3.0;
    return 0.0;
}

// Обчислення на основі текстового введення
double CText(const string& text) {
    if (text.empty() || Gtext(text) == 0.0)
        throw ErrorDKrl(0.0, 0.0); // Генерація помилки при неправильному тексті
    return Gtext("set") + Gtext("get") - Gtext(text);
}

// ======= ОСНОВНІ ФУНКЦІЇ ==========

// Основна формула обчислення варіанту, з перевіркою меж
double Variant(double k, double x, double y) {
    if (x < -10 || x > 10) throw ErrorRange(x);
    if (y < -10 || y > 10) throw ErrorRange(y);
    if (k == 0) throw ErrorKrl(k);
    return 0.8973 * y + 0.1027 * k;
}

// Основна функція зчитує файли, виконує обчислення і обробляє винятки
double computeVariant(double k, double x, double y, const string& text) {
    try {
        string path = "D:/Універ/2 курс/Не моє/lab_1/";

        // Завантаження трьох наборів даних з файлів
        vector<DataRow> dat1 = loadData(path + "dat1.dat"); // U(x)
        vector<DataRow> dat2 = loadData(path + "dat2.dat"); // T(x)
        vector<DataRow> dat3 = loadData(path + "dat3.dat"); 

        // Інтерполяція значень та обчислення
        double Ux = interpolate(x, dat1);
        double Tx = interpolate(x, dat2);
        double f = CText(text);
        double result = Variant(k, x, y);

        return result + Ux + Tx + f;
    }
    // Обробка різних типів помилок
    catch (ErrorRange& e) {
        e.Message();
        return e.value / 2;
    }
    catch (ErrorNoFile& e) {
        e.Message();
        return -1;
    }
    catch (ErrorDKrl& e) {
        e.Message();
        return e.setFun();
    }
    catch (ErrorKrl& e) {
        e.Message();
        return e.setFun();
    }
    catch (...) {
        cout << "Unknown error occurred." << endl;
        return -999;
    }
}

// ======= MAIN ==========

int main() {
    double k, x, y;
    string text;

    // Ввід користувача
    cout << "Enter value of k: ";
    cin >> k;

    cout << "Enter value of x: ";
    cin >> x;

    cout << "Enter value of y: ";
    cin >> y;

    cout << "Enter text value (e.g., aet, vet, zet): ";
    cin >> text;

    // Обчислення результату з усіма перевірками
    double result = computeVariant(k, x, y, text);

    cout << "\nResult: Variant(k, x, y, text) = " << result << endl;

    return 0;
}
