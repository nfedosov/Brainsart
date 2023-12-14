#include <complex>
#include <vector>
#include <cmath>

enum FilterType {
    Lowpass,
    Highpass,
    Bandstop
};

class ButterworthFilter {
public:
    ButterworthFilter(int order, double cutoffFrequency, double sampleRate, FilterType type) {
        this->order = order;
        this->cutoffFrequency = cutoffFrequency;
        this->sampleRate = sampleRate;
        this->filterType = type;
        calculateCoefficients();
        reset();
    }

    void reset() {
        x.resize(order*2-1);
        y.resize(order*2-1);
        for (int i = 0; i < order; i++) {
            x[i] = 0.0;
            y[i] = 0.0;
        }
    }

    double filter(double input) {
        // Shift the input values
        for (int i = order*2 - 2; i > 0; i--) {
            x[i] = x[i - 1];
            y[i] = y[i - 1];
        }
        x[0] = input;

        // Calculate the filtered output
        double output = 0.0;
        for (int i = 0; i < order*2-1; i++) {
            output += b[i] * x[i];
            if (i > 0) {
                output -= a[i] * y[i];
            }
        }
        y[0] = output;
        return output;
    }

private:
    void calculateCoefficients() {
        const double nyquist = sampleRate / 2.0;
        const double wc = 2.0 * M_PI * cutoffFrequency / nyquist;
        const double wc2 = wc * wc;
        const double sqrt2 = sqrt(2.0);

        b.resize(order + 1, 0.0);
        a.resize(order + 1, 0.0);

        for (int i = 0; i <= order; i++) {
            double val = (i % 2 == 0) ? 1.0 : -1.0;
            b[i] = binomial(order, i) * pow(wc, i);
            a[i] = binomial(order, i) * pow(wc2, i);
        }

        const double gain = pow(wc, -order);
        for (int i = 0; i <= order; i++) {
            b[i] *= gain;
            a[i] *= gain;
        }

        if (filterType == Lowpass) {
            // Nothing additional to do for lowpass filter
        } else if (filterType == Highpass) {
            // Invert the coefficients for highpass filter
            for (int i = 0; i <= order; i++) {
                b[i] *= (i % 2 == 0) ? 1.0 : -1.0;
                a[i] *= (i % 2 == 0) ? 1.0 : -1.0;
            }
            b[0] -= 1.0;
        } else if (filterType == Bandstop) {
            // Invert the coefficients for bandstop filter
            for (int i = 0; i <= order; i++) {
                b[i] *= (i % 2 == 0) ? 1.0 : -1.0;
                a[i] *= (i % 2 == 0) ? 1.0 : -1.0;
            }
            b[0] += 1.0;
        }
    }

    double binomial(int n, int k) {
        if (k == 0 || k == n) {
            return 1.0;
        }
        if (k < 0 || k > n) {
            return 0.0;
        }
        double result = 1.0;
        for (int i = 0; i < k; i++) {
            result *= (n - i);
            result /= (i + 1);
        }
        return result;
    }

public:
    int order;
    double cutoffFrequency;
    double sampleRate;
    FilterType filterType;
    std::vector<double> b;
    std::vector<double> a;
    std::vector<double> x;
    std::vector<double> y;
};



int main()
{
    double input[10] = {0,1,2,3,4,5,6,7,8,9};
    double output[10] = {0,0,0,0,0,0,0,0,0,0};

    ButterworthFilter *lowpass = new ButterworthFilter(2, 1.0, 1000.0, Lowpass);
    //lowpass->b[0] =  9.825916820482034e-06;
    //lowpass->b[1] = 1.965183364096407e-05;
    //lowpass->b[2] =  9.825916820482034e-06;

    //lowpass->a[0] = 1.0;
    //lowpass->a[1] = -1.9911142922016536;
    //lowpass->a[2] =0.9911535958689355;






    for (int i = 0; i < 10; i++)
    {
        output[i] = lowpass->filter(input[i]);
        printf("%lf\n",output[i]);
    }



    return 0;
}






/*#include "butterworthfilter.h"

#include <complex>
#include <vector>
#include <cmath>



ButterworthFilter::ButterworthFilter(int order, double cutoffFrequency, double sampleRate, FilterType type) {
        this->order = order;
        this->cutoffFrequency = cutoffFrequency;
        this->sampleRate = sampleRate;
        this->filterType = type;
        calculateCoefficients();
        reset();
    }

 void ButterworthFilter::reset() {
        x.resize(order);
        y.resize(order);
        for (int i = 0; i < order; i++) {
            x[i] = 0.0;
            y[i] = 0.0;
        }
    }

    double filter(double input) {
        // Shift the input values
        for (int i = order - 1; i > 0; i--) {
            x[i] = x[i - 1];
            y[i] = y[i - 1];
        }
        x[0] = input;

        // Calculate the filtered output
        double output = 0.0;
        for (int i = 0; i < order; i++) {
            output += b[i] * x[i];
            if (i > 0) {
                output -= a[i] * y[i];
            }
        }
        y[0] = output;
        return output;
    }

private:
    void calculateCoefficients() {
        const double nyquist = sampleRate / 2.0;
        const double wc = 2.0 * M_PI * cutoffFrequency / nyquist;
        const double wc2 = wc * wc;
        const double sqrt2 = sqrt(2.0);

        b.resize(order + 1, 0.0);
        a.resize(order + 1, 0.0);

        for (int i = 0; i <= order; i++) {
            double val = (i % 2 == 0) ? 1.0 : -1.0;
            b[i] = binomial(order, i) * pow(wc, i);
            a[i] = binomial(order, i) * pow(wc2, i);
        }

        const double gain = pow(wc, -order);
        for (int i = 0; i <= order; i++) {
            b[i] *= gain;
            a[i] *= gain;
        }

        if (filterType == Lowpass) {
            // Nothing additional to do for lowpass filter
        } else if (filterType == Highpass) {
            // Invert the coefficients for highpass filter
            for (int i = 0; i <= order; i++) {
                b[i] *= (i % 2 == 0) ? 1.0 : -1.0;
                a[i] *= (i % 2 == 0) ? 1.0 : -1.0;
            }
            b[0] -= 1.0;
        } else if (filterType == Bandstop) {
            // Invert the coefficients for bandstop filter
            for (int i = 0; i <= order; i++) {
                b[i] *= (i % 2 == 0) ? 1.0 : -1.0;
                a[i] *= (i % 2 == 0) ? 1.0 : -1.0;
            }
            b[0] += 1.0;
        }
    }

    double binomial(int n, int k) {
        if (k == 0 || k == n) {
            return 1.0;
        }
        if (k < 0 || k > n) {
            return 0.0;
        }
        double result = 1.0;
        for (int i = 0; i < k; i++) {
            result *= (n - i);
            result /= (i + 1);
        }
        return result;
    }

private:
    int order;
    double cutoffFrequency;
    double sampleRate;
    FilterType filterType;
    std::vector<double> b;
    std::vector<double> a;
    std::vector<double> x;
    std::vector<double> y;
};*/
