#include <cassert>
#include <iomanip>
#include <numeric>
#include <vector>
#include <iostream>
#include <valarray>

using namespace std;

template<typename T>
class LeastSquares {
public:
    LeastSquares(const vector<T> &x, const vector<T> &y) {
        assert(x.size() == y.size());

        n = x.size();
        sum_x = accumulate(x.begin(), x.end(), 0.0);
        sum_y = accumulate(y.begin(), y.end(), 0.0);
        sum_x2 = accumulate(x.begin(), x.end(), 0.0,
                            [](double a, double b) { return a + b * b; });
        sum_xy = 0.0;
        for (int i = 0; i < n; ++i) sum_xy += x[i] * y[i];
    }

    double m() {
        return (sum_y * sum_x2 - sum_x * sum_xy) / (n * sum_x2 - sum_x * sum_x);
    }

    double c() {
        return (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
    }

private:
    int n;
    double sum_x, sum_y, sum_x2, sum_xy;
};

template<typename T>
class NewtonRaphson {
public:
    double solve(auto f, auto fd, double x0, double eps) {
        double x1 = x0 - f(x0) / fd(x0);
        try {
            while (abs(x1 - x0) > eps) {
                x0 = x1;
                x1 = x0 - f(x0) / fd(x0);
            }
        } catch (...) {
            cout << "Error: " << x0 << endl;
        }
        return x1;
    }
};

int lab_02() {
    double FOS = 1.35;

    cout << "ENTER DESIRED FOS: " << endl;
    cin >> FOS;

    int n = 4;

    vector<double> size(n);
    vector<double> strength(n);

    size = {25, 50, 75, 100};
    strength = {18, 10, 7, 6};

    double Wg = 4.5;
    double Hp = 3.0;
    double unitWeight = 2.6 * 1e3 * 9.81;
    double miningHeight = 200.0;

    vector<double> y, x;
    for (int i = 0; i < n; i++) {
        y.push_back(log(strength[i])), x.push_back(log(size[i]));
    }


    cout << "ln(l)\t\tln(S)" << endl;
    for (int i = 0; i < n; i++) {
        cout << x[i] << "\t\t" << y[i] << endl;
    }

    LeastSquares<double> ls(x, y);
    double m = ls.m();
    double c = ls.c();

    cout << endl;
    cout << "m = " << m << endl;
    cout << "c = " << c << endl;

    cout << endl;
    cout << "a = " << -c << endl;
    cout << "k = " << exp(m) << endl;

    auto k = exp(m);
    auto a = -c;

    double S1 = k * pow(1000, -a);

    cout << endl;
    cout << "S1 = " << S1 << endl;

    double A1, A2, A3, A4;

    A1 = 0.36 * S1 * 1e6 / Hp;
    A2 = 0.64 * S1 * 1e6 - FOS * unitWeight * miningHeight;
    A3 = -2 * FOS * unitWeight * miningHeight * Wg;
    A4 = -1 * FOS * unitWeight * miningHeight * Wg * Wg;

    cout << fixed << setprecision(2) << endl;
    cout << "A1 = " << A1 << endl;
    cout << "A2 = " << A2 << endl;
    cout << "A3 = " << A3 << endl;
    cout << "A4 = " << A4 << endl;

    auto g = [=](double x) {
        return A1 * x * x * x + A2 * x * x + A3 * x + A4;
    };
    auto dg = [=](double x) {
        return 3 * A1 * x * x + 2 * A2 * x + A3;
    };

    NewtonRaphson<double> nr;
    cout << endl;
    cout << "Width of Pillar = " << nr.solve(g, dg, 100.0, 1e-4) << endl;

    // Assignment Part
    double ROLL_NUMBER_LAST_DIGIT = 3;  // 18MI31033

    vector<double> galleryWidths = {3, 3.6, 4, 4.2, 4.8};
    vector<double> verticalDepths = {
            60 + ROLL_NUMBER_LAST_DIGIT,
            90 + ROLL_NUMBER_LAST_DIGIT,
            120 + ROLL_NUMBER_LAST_DIGIT,
            150 + ROLL_NUMBER_LAST_DIGIT,
            240 + ROLL_NUMBER_LAST_DIGIT,
            360 + ROLL_NUMBER_LAST_DIGIT,
    };
    vector<vector<double>> answerTable;

    for (auto &d: verticalDepths) {
        vector<double> pillarWidths;
        for (auto &w: galleryWidths) {
            A1 = 0.36 * S1 * 1e6 / Hp;
            A2 = 0.64 * S1 * 1e6 - FOS * unitWeight * d;
            A3 = -2 * FOS * unitWeight * d * w;
            A4 = -1 * FOS * unitWeight * d * w * w;

            auto f = [=](double x) {
                return A1 * x * x * x + A2 * x * x + A3 * x + A4;
            };
            auto df = [=](double x) {
                return 3 * A1 * x * x + 2 * A2 * x + A3;
            };

            pillarWidths.push_back(nr.solve(f, df, 100.0, 1e-4));
        }
        answerTable.push_back(pillarWidths);
    }

    cout << endl << "\t\t";
    for (auto &w: galleryWidths)
        cout << fixed << setprecision(1) << w << "    \t\t";

    cout << endl;
    cout << "--------------------------------------------------------------------------------------" << endl;

    int i = 0;
    for (auto &row: answerTable) {
        cout << (int) verticalDepths[i++] << "\t\t";

        for (auto &col: row)
            cout << fixed << setprecision(3) << col << "\t\t";

        cout << endl;
    }

    return 0;
}