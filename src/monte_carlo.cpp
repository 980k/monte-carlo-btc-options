#include <cmath>
#include <random>
#include <iostream>
#include <algorithm>
#include "implied_volatility.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;

class MonteCarlo {

    public:
        double stdErr, stdDev;

        MonteCarlo() {
            stdErr = 0;
            stdDev = 0;
        }

        double monte_carlo(double K, double T, double S, double r, double div, double m, int N, int M) {
            double dt;
            double nudt;
            double sigsdt; 
            double lnS;
            double sig;
            double sum_CT;
            double sum_CT2;

            sig = implied_volatility_iterative(S, K, T, r, m);

            dt = T/N;
            nudt = (r - div - 0.5 * std::pow(sig, 2)) * dt;
            sigsdt = sig * sqrt(dt);
            lnS = std::log(S);

            sum_CT = 0;
            sum_CT2 = 0;

            for(int j = 1; j <= M; j++) {
                double lnSt = lnS;
                for(int i = 1; i <= N; i++) {
                std::random_device rd;
                std::mt19937 gen(rd()); 
                std::normal_distribution<double> dist(0.0, 1.0);

                double random_normal = dist(gen);

                lnSt = lnSt + nudt + sigsdt * random_normal;
                }
                double ST = exp(lnSt);
                double CT = std::max(0.0, ST - K);
                sum_CT = sum_CT + CT;
                sum_CT2 = sum_CT2 + CT*CT;
            }

            double call_value = sum_CT/M * exp(-r * T);
            double SD = sqrt((sum_CT2 - sum_CT*sum_CT/M) * exp(-2*r*T)/(M - 1));
            double SE = SD / sqrt(M);

            stdDev = SD;
            stdErr = SE;

            return call_value;
        }

        double monte_carlo_antihetic(double K, double T, double S, double r, double div, double m, int N, int M) {
            double dt;
            double nudt;
            double sigsdt;
            double lnS;
            double sig;

            sig = implied_volatility_iterative(S, K, T, r, m);

            dt = T/N;
            nudt = (r - div - 0.5 * std::pow(sig, 2)) * dt;
            sigsdt = sig * sqrt(dt);
            lnS = log(S);

            double sum_CT = 0;
            double sum_CT2 = 0;

            for(int j = 1; j <= M; j++) {
                double lnSt1 = lnS;
                double lnSt2 = lnS;
                for(int i = 1; i <= N; i++) {
                    std::random_device rd;
                    std::mt19937 gen(rd()); 
                    std::normal_distribution<double> dist(0.0, 1.0);

                    double random_normal = dist(gen);

                    lnSt1 = lnSt1 + nudt + sigsdt*(random_normal);
                    lnSt2 = lnSt2 + nudt + sigsdt*(-(random_normal));
                }
                    double St1 = exp(lnSt1);
                    double St2 = exp(lnSt2);
                    double CT = 0.5*(std::max(0.0, St1 - K) + std::max(0.0, St2 - K));
                    sum_CT = sum_CT + CT;
                    sum_CT2 = sum_CT2 + CT*CT;
            }

            double call_value = sum_CT/M*exp(-r*T);
            double SD = sqrt((sum_CT2 - sum_CT*sum_CT/M)*exp(-2*r*T)/(M-1));
            double SE = SD/sqrt(M);

            stdDev = SD;
            stdErr = SE;

            return call_value;
        }
};

PYBIND11_MODULE(montecarlo, handle) {
    py::class_<MonteCarlo>(
                           handle, "PyMonteCarlo"
                           )
        .def(py::init<>())
        .def("monte_carlo", &MonteCarlo::monte_carlo)
        .def("monte_carlo_antihetic", &MonteCarlo::monte_carlo_antihetic)
        .def_readwrite("stdErr", &MonteCarlo::stdErr)
        .def_readwrite("stdDev", &MonteCarlo::stdDev)
        ;
}