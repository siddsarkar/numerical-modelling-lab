#include <vector>
#include <valarray>
#include <iostream>

#define ND 0
#define MAX_ITERATIONS 10000

using namespace std;

template<typename T>
class Project {
public:
    Project(const string &name,
            const vector<T> &year,
            const vector<T> &operating_cost,
            const vector<T> &capital_cost,
            const vector<T> &revenue,
            const T tax_rate,
            const T discount_rate
    ) {
        this->name = name;
        this->year = year;
        this->operating_cost = operating_cost;
        this->capital_cost = capital_cost;
        this->revenue = revenue;
        this->discount_rate = discount_rate;
        this->tax_rate = tax_rate;
    }

    float getNpv() {
        float npv = 0.0;
        for (int i = 0; i < year.size(); ++i) {
            auto cf = ((revenue[i] - operating_cost[i]) * (1 - tax_rate / 100) - capital_cost[i]);
            npv += cf / pow(1 + discount_rate / 100, year[i]);
        }
        return npv;
    }

    float getIrr() {
        float r = 2;
        float eps = 0.01;

        for (int x = 0; x < MAX_ITERATIONS; ++x) {
            float n = 0.0;
            float d = 0.0;
            for (int i = 0; i < year.size(); ++i) {
                auto cf = ((revenue[i] - operating_cost[i]) * (1 - tax_rate / 100) - capital_cost[i]);
                n += cf / pow(1 + r / 100, year[i]);
                d += ((-year[i]) * cf) / pow(1 + r / 100, year[i] + 1);
            }

            if (abs(n / d) < eps)
                return r;
            else
                r -= n / d;
        }

        return r;
    }

    string getName() {
        return name;
    }

private:
    string name;
    vector<T> year;
    vector<T> operating_cost;
    vector<T> capital_cost;
    vector<T> revenue;
    T discount_rate;
    T tax_rate;
};


int main() {

    Project p1 = Project("Panihati Coal Block",
                         {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
                         {ND, ND, ND, 30, 32, 33, 36, 37, 38, 34, 35},
                         {100, 85, 30, ND, ND, ND, ND, ND, ND, ND, ND},
                         {ND, ND, ND, 80, 85, 90, 88, 92, 96, 75, 80},
                         33.20, 15.5);

    cout << p1.getName() << endl;
    cout << "NPV: " << p1.getNpv() << endl;
    cout << "IRR: " << p1.getIrr() << endl << endl;


    Project p2 = Project("Ekchakra Coal Block",
                         {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
                         {ND, ND, 35, 30, 32, 33, 36, 37, 38, 34, 35},
                         {150, 65, ND, ND, ND, ND, ND, ND, ND, ND, ND},
                         {ND, ND, 65, 65, 95, 90, 76, 99, 88, 77, 94},
                         33.8, 14.5);

    cout << p2.getName() << endl;
    cout << "NPV: " << p2.getNpv() << endl;
    cout << "IRR: " << p2.getIrr() << endl << endl;

    Project p3 = Project("Remuna Coal Block",
                         {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
                         {ND, 45, 35, 30, 32, 33, 36, 37, 38, 34, 35},
                         {200, ND, ND, ND, ND, ND, ND, ND, ND, ND, ND},
                         {ND, 80, 85, 75, 91, 95, 87, 95, 79, 81, 97},
                         35.50, 15.0);

    cout << p3.getName() << endl;
    cout << "NPV: " << p3.getNpv() << endl;
    cout << "IRR: " << p3.getIrr() << endl << endl;

    Project p4 = Project("Bhadradri Coal Block",
                         {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
                         {ND, 51, 53, 36, 33, 39, 40, 42, 46, 43, 53},
                         {200, ND, ND, ND, ND, ND, ND, ND, ND, ND, ND},
                         {ND, 90, 95, 80, 117, 96, 119, 95, 129, 83, 95},
                         34.50, 15.5);

    cout << p4.getName() << endl;
    cout << "NPV: " << p4.getNpv() << endl;
    cout << "IRR: " << p4.getIrr() << endl;


    return 0;
}