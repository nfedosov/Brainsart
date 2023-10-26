#ifndef FIRWIN_H
#define FIRWIN_H

#include <string>
#include <vector>
using namespace std;



class FirWin
{
public:
    FirWin(int numtaps, double low_cutoff, double high_cutoff, double fs, int Nch,  string window="hamming");
    vector<double> h;
    std::vector<std::vector<double>> buf;
};

#endif // FIRWIN_H
