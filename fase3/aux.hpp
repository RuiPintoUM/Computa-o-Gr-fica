#ifndef AUX_HPP
#define AUX_HPP

#include <cmath>
#include "structures.hpp"

class aux {
    public:
        static void multMatrixVector(float *m, float *v, float* res) {
            for (int j = 0; j < 4; ++j) {
                res[j] = 0;
                for (int k = 0; k < 4; ++k) {
                    res[j] += v[k] * m[j * 4 + k];
                }
            }
        }

        static void multVectorMatrix(float *v, float *m, float* res) {
            for (int j = 0; j < 4; j++) {
                res[j] = 0;
                for (int k = 0; k < 4; k++) {
                    res[j] += v[k] * m[k * 4 + j];
                }
            }
        }

        static float multVectors(float *v1, float *v2) {
            float res = 0;
            for (int j = 0; j < 4; j++) {
                res += v1[j] * v2[j];
            }
            return res;
        }

        static void normalize(Point *a) {
            float norm = sqrt(a->getX() * a->getX() + a->getY() * a->getY() + a->getZ() * a->getZ());
            a->setX(a->getX() / norm);
            a->setY(a->getY() / norm);
            a->setZ(a->getZ() / norm);
        }

        static void cross(Point *a, Point *b, Point *res) {
            res->setX(a->getY() * b->getZ() - a->getZ() * b->getY());
            res->setY(a->getZ() * b->getX() - a->getX() * b->getZ());
            res->setZ(a->getX() * b->getY() - a->getY() * b->getX());
        } 
};

#endif
