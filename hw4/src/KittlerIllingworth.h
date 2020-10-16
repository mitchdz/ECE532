#ifndef KITTLERILLINGWORTH_H
#define KITTLERILLINGWORTH_H

#include <stdint.h>
#include <math.h>

double Kittleru1(uint8_t *h, int t);
double KittlerP(uint8_t *h, int t);
double Kittlerq1(uint8_t *h, int t);
double Kittlerq2(uint8_t *h, int t);
double Kittlervar1(uint8_t *h, int t);
double Kittlervar2(uint8_t *h, int t);
double Kittlerf(uint8_t *h, int t);


/* @brief P is the observed "probability distribution"
 *        P = h(t)/#numpixels
 * @param h histogram created as a single uint8_t array
 * @param t index of histogram
 * @return double
 */
double KittlerP(uint8_t *h, int t)
{
    uint8_t hval = h[t];
    double P = ((double)hval / 255);

    return P;
}



/* @brief Kittler q1
 *       t
 *  q1 = Σ P(h,t)
 *      i=0
 * @param h histogram created as a single uint8_t array
 * @param t index of histogram
 * @return double
 */
double Kittlerq1(uint8_t *h, int t)
{
    double sum = 0, P;

    for (int i = 0; i <= t; i++) {
        P = KittlerP(h, t);
        sum += P;
    }

    return sum;
}

/* @brief Kittler q2
 *       255
 *  q2 = Σ P(h,t) = 1 - q1(h,t)
 *      i=t+1
 * @param h histogram created as a single uint8_t array
 * @param t index of histogram
 * @return double
 */
double Kittlerq2(uint8_t *h, int t)
{
    return 1.0 - Kittlerq1(h, t);
}



/* @brief Kittler u1
 *       t
 *  u1 = Σ i*P(h,t)/q1(h,t)
 *      i=0
 * @param h histogram created as a single uint8_t array
 * @param t index of histogram
 * @return double
 */
double Kittleru1(uint8_t *h, int t)
{
    double sum = 0, P, q1;

    P = KittlerP(h,t);
    q1 = Kittlerq1(h,t);

    for (int i = 0; i <= t; i++) {
        sum += (i*P)/q1;
    }
    return sum;
}


/* @brief Kittler u2
 *      255
 *  u2 = Σ i*P(h,t)/q2(h,t)
 *     i=t+1
 * @param h histogram created as a single uint8_t array
 * @param t index of histogram
 * @return double
 */
double Kittleru2(uint8_t *h, int t)
{
    double sum = 0, P, q2;

    P  = KittlerP(h,t);
    q2 = Kittlerq2(h,t);

    for (int i = t + 1; i <= 255; i++) {
        sum += i*P/q2;
    }
    return sum;
}



/* @brief Kittler var1
 *       t
 *  var1 = Σ ([i-u1(t)]^2 * P(t) * q1(t))
 *      i=0
 * @param h histogram created as a single uint8_t array
 * @param t index of histogram
 * @return double
 */
double Kittlervar1(uint8_t *h, int t)
{
    double sum = 0, P, u1, q1;
    P  = KittlerP(h,t);
    u1 = Kittleru1(h,t);
    q1 = Kittlerq1(h,t);
    for (int i = 0; i <= t; i++) {
        sum += pow(i-u1,2) * P * q1;
    }
    return sum;
}

/* @brief Kittler var2
 *      255
 *  var2 = Σ ([i-u2(h,t)]^2 * P(h,t) * q2(h,t))
 *     i=t+1
 * @param h histogram created as a single uint8_t array
 * @param t index of histogram
 * @return double
 */
double Kittlervar2(uint8_t *h, int t)
{
    double sum = 0, u2, P, q2;
    P  = KittlerP(h,t);
    u2 = Kittleru2(h,t);
    q2 = Kittlerq2(h,t);
    for (int i = t + 1; i <= 255; i++) {
        sum += pow(i-u2,2)*(P) * q2;
    }
    return sum;
}

/* @brief f is representation of histogram as sum of 2 gaussian functions
 *   f(t) = q1*gauss1 + q2*gauss2
 *
 * @param h histogram as a pointer to uint8_t array
 * @param t index of histogram gaussian representation
 *
 * @return double
 */
double Kittlerf(uint8_t *h, int t)
{
    double q1   = Kittlerq1(h,t);
    double var1 = Kittlervar1(h,t);
    double u1   = Kittleru1(h,t);
    double g1 = q1/( var1*sqrt(2*M_PI) ) *
        pow( M_E, -1.0 * pow( (t-u1),2)/(2*var1 ) );

    double q2   = Kittlerq2(h,t);
    double var2 = Kittlervar2(h,t);
    double u2   = Kittleru2(h,t);
    double g2 = q2/( var2*sqrt(2*M_PI) ) *
        pow( M_E, -1.0 * pow( (t-u2),2)/(2*var2) );

    return g1 + g2;

}

#endif
