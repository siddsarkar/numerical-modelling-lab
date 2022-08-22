#include <iomanip>
#include "pbPlot/pbPlots.hpp"
#include "pbPlot/supportLib.hpp"

using namespace std;

/**
 * Problem 1: f(x) = 4e^4x + 3e^3x + 2e^2x + e^x, x in [1,2]
 */
class F1 {
public:
    constexpr static double TV = 3361.582962;

    static double eval(double x) {
        return (4 * exp(4 * x)) + (3 * exp(3 * x)) + (2 * exp(2 * x)) + exp(x);
    }
};

/**
 * Problem 2: f(x) = x^3 + 2x + 1/x, x in [e,5]
 */
class F2 {
public:
    constexpr static double TV = 160.8208443;

    static double eval(double x) {
        return pow(x, 3) + 2 * x + 1 / x;
    }
};

/**
 * @brief Calculates the integral of f(x) from a to b using the trapezoidal rule.
 *
 * @param n - number of intervals
 * @param a - the lower bound of the integral
 * @param b - the upper bound of the integral
 * @param f - the function to integrate
 * @return the area under the curve
 */
double trapezoidal(int n, double (*f)(double), double a, double b) {
    double h = (b - a) / n;

    double Tn = f(a) + f(b);        // first and last term
    for (int i = 1; i < n; ++i)      // middle terms
        Tn += 2 * f(a + i * h);
    Tn *= h / 2;

    return Tn;
}

/**
 * @brief Calculates the integral of f(x) from a to b using the Simpson's ⅓ rule.
 *
 * @param n - number of intervals
 * @param a - the lower bound of the integral
 * @param b - the upper bound of the integral
 * @param f - the function to integrate
 * @return the area under the curve
 */
double simpsons(int n, double (*f)(double), double a, double b) {
    double h = (b - a) / n;

    double Tn = f(a) + f(b);        // first and last term
    for (int i = 1; i < n; ++i)
        if (i % 2)
            Tn += 4 * f(a + i * h);     // odd terms
        else
            Tn += 2 * f(a + i * h);     // even terms
    Tn *= h / 3;

    return Tn;
}

/**
 * @brief Use pbPlot library to plot the percent error vs. n for the given function.
 *
 * @param xs - the x values to plot
 * @param ys - the y values to plot
 * @param fn - function number (1 or 2)
 * @param rule - the rule to use for the integration (trapezoidal or simpsons)
 * @returns true if the plot was successfully generated, false otherwise.
 */
bool plot(vector<double> &xs, vector<double> &ys, int fn, const string &rule) {
    bool success;
    auto *errorMessage = new StringReference();

    RGBABitmapImage *container = CreateImage(900, 600, GetWhite());
    RGBABitmapImageReference *imageReference = CreateRGBABitmapImageReference();

    ScatterPlotSeries *series = GetDefaultScatterPlotSeriesSettings();
    series->xs = &xs;
    series->ys = &ys;
    series->linearInterpolation = true;
    series->lineType = toVector(L"dashed");
    series->lineThickness = 2;
    series->color = GetBlack();

    ScatterPlotSettings *settings = GetDefaultScatterPlotSettings();
    settings->width = 900;
    settings->height = 600;
    settings->autoBoundaries = true;
    settings->autoPadding = false;
    settings->xPadding = 100;
    settings->yPadding = 75;
    settings->title = fn == 1
                      ? rule == "trapezoidal"
                        ? toVector(L"Function 1 - Trapezoidal Rule")
                        : toVector(L"Function 1 - Simpsons Rule")
                      : rule == "simpsons"
                        ? toVector(L"Function 2 - Simpsons Rule")
                        : toVector(L"Function 2 - Trapezoidal Rule");
    settings->xLabel = toVector(L"intervals (n)");
    settings->yLabel = toVector(L"error (%)");
    settings->scatterPlotSeries->push_back(series);

    success = DrawScatterPlotFromSettings(imageReference, settings, errorMessage);

    if (success) {
        DrawImageOnImage(container, imageReference->image, 40, 0);

        vector<double> *pngData = ConvertToPNG(container);
        WriteToFile(pngData, rule + "_f" + to_string(fn) + ".png");
        DeleteImage(imageReference->image);
    } else {
        cerr << "Error: ";
        for (wchar_t c: *errorMessage->string)
            wcerr << c;
        cerr << endl;
    }

    return success;
}

int main() {
    int n = 100;

    vector<double> xs, ytf1, ysf1, xt, ytf2, ysf2;

    for (int i = 10; i < n; i += 1) {
        double err;

        xt.push_back(i);

        err = abs(trapezoidal(i, F1::eval, 1, 2) - F1::TV) / F1::TV * 100;
        ytf1.push_back(err);

        err = abs(trapezoidal(i, F2::eval, exp(1), 5) - F2::TV) / F2::TV * 100;
        ytf2.push_back(err);


        if (!(i % 2)) {
            xs.push_back(i);

            err = abs(simpsons(i, F2::eval, exp(1), 5) - F2::TV) / F2::TV * 100;
            ysf2.push_back(err);

            err = abs(simpsons(i, F1::eval, 1, 2) - F1::TV) / F1::TV * 100;
            ysf1.push_back(err);
        }
    }


    cout << "Trapezoidal Rule: Function 1" << endl;
    cout << "T10=" << fixed << setprecision(4) << trapezoidal(10, F1::eval, 1, 2) << " E10=" << fixed << setprecision(4)
         << abs(trapezoidal(10, F1::eval, 1, 2) - F1::TV) / F1::TV * 100 << endl;
    cout << "T100=" << fixed << setprecision(4) << trapezoidal(100, F1::eval, 1, 2) << " E100=" << fixed
         << setprecision(4) << abs(trapezoidal(100, F1::eval, 1, 2) - F1::TV) / F1::TV * 100 << endl;


    cout << endl << "Simpsons Rule: Function 1" << endl;
    cout << "T10=" << fixed << setprecision(4) << simpsons(10, F1::eval, 1, 2) << " E10=" << fixed << setprecision(7)
         << abs(simpsons(10, F1::eval, 1, 2) - F1::TV) / F1::TV * 100 << endl;
    cout << "T100=" << fixed << setprecision(7) << simpsons(100, F1::eval, 1, 2) << " E100=" << fixed << setprecision(7)
         << abs(simpsons(100, F1::eval, 1, 2) - F1::TV) / F1::TV * 100 << endl;

    cout << endl << "Trapezoidal Rule: Function 2" << endl;
    cout << "T10=" << fixed << setprecision(4) << trapezoidal(10, F2::eval, exp(1), 5) << " E10=" << fixed
         << setprecision(4) << abs(trapezoidal(10, F2::eval, exp(1), 5) - F2::TV) / F2::TV * 100 << endl;
    cout << "T100=" << fixed << setprecision(4) << trapezoidal(100, F2::eval, exp(1), 5) << " E100=" << fixed
         << setprecision(4) << abs(trapezoidal(100, F2::eval, exp(1), 5) - F2::TV) / F2::TV * 100 << endl;

    cout << endl << "Simpsons Rule: Function 2" << endl;
    cout << "T10=" << fixed << setprecision(4) << simpsons(10, F2::eval, exp(1), 5) << " E10=" << fixed
         << setprecision(9) << abs(simpsons(10, F2::eval, exp(1), 5) - F2::TV) / F2::TV * 100 << endl;
    cout << "T100=" << fixed << setprecision(7) << simpsons(100, F2::eval, exp(1), 5) << " E100=" << fixed
         << setprecision(9) << abs(simpsons(100, F2::eval, exp(1), 5) - F2::TV) / F2::TV * 100 << endl;


    cout << endl << "Generating plots..." << endl;
    plot(xt, ytf1, 1, "trapezoidal")
    ? cout << "Trapezoidal rule for f1 succeeded" << endl
    : cerr << "Trapezoidal rule for f1 failed" << endl;

    plot(xt, ytf2, 2, "trapezoidal")
    ? cout << "Trapezoidal rule for f2 succeeded" << endl
    : cerr << "Trapezoidal rule for f2 failed" << endl;

    plot(xs, ysf1, 1, "simpsons")
    ? cout << "Simpsons rule for f1 succeeded" << endl
    : cerr << "Simpsons rule for f1 failed" << endl;

    plot(xs, ysf2, 2, "simpsons")
    ? cout << "Simpsons rule for f2 succeeded" << endl
    : cerr << "Simpsons rule for f2 failed" << endl;

    return 0;
};