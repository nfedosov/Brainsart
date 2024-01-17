#ifndef FIRWIN_H
#define FIRWIN_H

class FirWin
{
public:
    FirWin(int numtaps, double low_cutoff, double high_cutoff, double fs, int Nch, std::string window="hamming");
    std::vector<double> h;
    std::vector<std::vector<double>> buf;
};

#endif // FIRWIN_H
