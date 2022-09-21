#include <cmath>
#include "implied_volatility.hpp"

#ifndef Pi
#define Pi 3.141592653589793238462643
#endif

double CND(double X) {

    double L, K, w;

    double const a1 = 0.31938153, a2 = -0.356563782, a3 = 1.781477937;
    double const a4 = -1.821255978, a5 = 1.330274429;

    L = fabs(X);
    K = 1.0 / (1.0 + 0.2316419 * L);
    w = 1.0 - 1.0 / sqrt(2 * Pi) * exp(-L *L / 2) * (a1 * K + a2 * K *K + a3 * pow(K,3) + a4 * pow(K,4) + a5 * pow(K,5));

    if (X < 0 ){
    w= 1.0 - w;
    }
    return w;
}

double BlackScholes(double S, double X, double T, double r, double v) {
    double d1, d2;

    d1=(log(S/X)+(r+v*v/2)*T)/(v*sqrt(T));
    d2=d1-v*sqrt(T);

    return S*CND(d1)-X * exp(-r*T)*CND(d2);
}

double implied_volatility_iterative(double S, double X, double T, double r, double m) {
    double value = m;
    double high = 5;
    double low = 0;
    double vol;

    do {
        if(BlackScholes(S, X, T, r, (high + low)/2) > value) {
            high = (high + low) / 2;
        }
        else {
            low = (high + low) / 2;
        }
    }
    while((high - low) > 0.0001);

    vol = (high + low) / 2;

    return vol;
}