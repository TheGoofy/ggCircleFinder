#ifndef GGLINEAR_H
#define GGLINEAR_H

/* Fit the data (x_i, y_i) to the linear relationship

   Y = c0 + c1 x

   returning,

   c0, c1  --  coefficients
   cov00, cov01, cov11  --  variance-covariance matrix of c0 and c1,
   sumsq   --   sum of squares of residuals

   This fit can be used in the case where the errors for the data are
   uknown, but assumed equal for all points. The resulting
   variance-covariance matrix estimates the error in the coefficients
   from the observed variance of the points around the best fit line.
*/

#ifndef size_t
#define size_t __SIZE_TYPE__
#endif

template <typename T>
void
gsl_fit_linear (const T *x, const size_t xstride,
                const T *y, const size_t ystride,
                const size_t n,
                double *c0, double *c1,
                double *cov_00, double *cov_01, double *cov_11, double *sumsq)
{
  double m_x = 0, m_y = 0, m_dx2 = 0, m_dxdy = 0;

  size_t i;

  for (i = 0; i < n; i++)
    {
      m_x += (x[i * xstride] - m_x) / (i + 1.0);
      m_y += (y[i * ystride] - m_y) / (i + 1.0);
    }

  for (i = 0; i < n; i++)
    {
      const double dx = x[i * xstride] - m_x;
      const double dy = y[i * ystride] - m_y;

      m_dx2 += (dx * dx - m_dx2) / (i + 1.0);
      m_dxdy += (dx * dy - m_dxdy) / (i + 1.0);
    }

  /* In terms of y = a + b x */

  {
    double s2 = 0, d2 = 0;
    double b = m_dxdy / m_dx2;
    double a = m_y - m_x * b;

    *c0 = a;
    *c1 = b;

    /* Compute chi^2 = \sum (y_i - (a + b * x_i))^2 */

    for (i = 0; i < n; i++)
      {
        const double dx = x[i * xstride] - m_x;
        const double dy = y[i * ystride] - m_y;
        const double d = dy - b * dx;
        d2 += d * d;
      }

    s2 = d2 / (n - 2.0);        /* chisq per degree of freedom */

    *cov_00 = s2 * (1.0 / n) * (1 + m_x * m_x / m_dx2);
    *cov_11 = s2 * 1.0 / (n * m_dx2);

    *cov_01 = s2 * (-m_x) / (n * m_dx2);

    *sumsq = d2;
  }
}


#endif // GGLINEAR_H
