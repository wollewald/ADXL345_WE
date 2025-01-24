/* defines the structure xyzFloat which is used for gyroscopes, accelerometers and 
   magnetometers such as ICM209468, MPU9250, ADXL345, etc. */

#ifndef XYZ_FLOAT_H_
#define XYZ_FLOAT_H_
#include <xyzFloat.h>
struct xyzFloat {
    float x;
    float y;
    float z;

    xyzFloat();
    xyzFloat(float const x, float const y, float const z);

    xyzFloat operator+() const;
    xyzFloat operator-() const;
    xyzFloat operator+(xyzFloat const & summand) const;
    xyzFloat operator-(xyzFloat const & subtrahend) const;
    xyzFloat operator*(float const operand) const;
	xyzFloat operator*(xyzFloat const & operand) const;
    xyzFloat operator/(float const divisor) const;
    xyzFloat & operator+=(xyzFloat const & summand);
    xyzFloat & operator-=(xyzFloat const & subtrahend);
    xyzFloat & operator*=(xyzFloat const & operand);
	xyzFloat & operator*=(float const operand);
    xyzFloat & operator/=(float const divisor);
};
#endif