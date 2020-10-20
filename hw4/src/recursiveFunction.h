#ifndef RECURSIVEFUNCTION_H
#define RECURSIVEFUNCTION_H

#include <stdint.h>
#include <math.h>
#include "KittlerIllingworth.h"


double Rvar2(uint8_t *h, int t);
double Rq1(uint8_t *h, int t);
double Rq2(uint8_t *h, int t);
double Ru(uint8_t *h, int t);
double Ru1(uint8_t *h, int t);
double Ru2(uint8_t *h, int t);



double Rq1(uint8_t *h, int t)
{
    //q1(0) = P(0)
    if ( t == 0 ) return KittlerP(h, 0);

    double q1 = 0;

    double P = KittlerP(h,t);
    double q1prev = Rq1(h,t-1);

    q1 = q1prev + P;
    return q1;
}

double Rq2(uint8_t *h, int t)
{
    //q2(t) = 1 - q1(t)
    return (1 - Rq1(h,t));
}

double Ru1(uint8_t *h, int t)
{
    // u1(0) = 0
    if ( t == 0 ) return 0;

    double mu1 = 0;

    double q1prev  = Rq1(h,t-1);
    double mu1prev = Ru1(h,t-1);
    double P       = KittlerP(h,t);
    double q1      = Rq1(h,t);

    mu1 = (q1prev*mu1prev+t*P)/q1;

    return mu1;
}

double Ru(uint8_t *h, int t)
{
    double sum = 0, P;

    //      255
    //  u =  Σ i*P(h,t)
    //      i=0
    for (int i = 0; i <= 255; i++) {
        P = KittlerP(h, t);
        sum += i*P;
    }

    return sum;
}

double Ru2(uint8_t *h, int t)
{
    double mu = Ru(h,t);
    double q1 = Rq1(h,t);
    double q2 = Rq2(h,t);
    double mu1 = Ru1(h,t);

    double mu2 = 0;
    // u2(0) = u/q2(0)
    if (t == 0) {
        mu2 = mu/q2;
        return mu2;
    }

    mu2 = (mu-q1*mu1)/q2;

    return mu2;
}



double Rvar1(uint8_t *h, int t)
{
    double var1 = 0;
    double mu1       = Ru1(h,t);
    double P        = KittlerP(h,t);
    double q1       = Rq1(h,t);
    double q1prev   = Rq1(h,t-1);
    double var1prev = Rvar1(h,t-1);
    double mu1prev   = Ru1(h,t-1);

    //var1(0) = 0
    if ( t == 0) return 0;

    var1 = (q1prev*(var1prev+(mu1prev-mu1)*(mu1prev-mu1)) +P*(t-mu1)*(t-mu1))/q1;
    return var1;
}


double Rvar2(uint8_t *h, int t)
{


    double var2=0;

    double mu2=0,P=0,q2=0,q2prev=0,var2prev=0,mu2prev=0;

    mu2       = Ru2(h,t);
    P        = KittlerP(h,t);
    q2       = Rq2(h,t);
    q2prev   = Rq2(h,t-1);
    var2prev = Rvar2(h,t-1);
    mu2prev   = Ru2(h,t-1);

    //         255
    //  var2(0) = Σ [i - u2(0)]^2 * P(i)/q2(0)
    //         i=1
    //if ( t == 0 ) {
    //    sum = 0;
    //    for (i = 1; i <= 255; i++) {
    //        P  = KittlerP(h,t);
    //        sum += (pow(i-u2,2) * P/q2);
    //    }
    //    return sum;
    //}

    // var2(t) = (1/q2) * (q2prev
    //           {var2prev + [u2prev - u2]^2}
    //           - P(t)*[t-u2prev]^2)
    //var2 = (1.0/q2)*(q2prev*(var2prev+pow(u2prev-u2,2)) - P*pow(t-u2,2));
    var2 = (q2prev*(var2prev+(mu2prev-mu2)*(mu2prev-mu2)) -P*(t-mu2)*(t-mu2))/q2;
    return var2;
}

#endif
