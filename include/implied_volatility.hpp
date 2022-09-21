#ifndef IMPLIED_VOLATILITY
#define IMPLIED_VOLATILITY

double CND(double X);
double BlackScholes(double S, double X, double T, double r, double v);
double implied_volatility_iterative(double S, double X, double T, double r, double m);

#endif