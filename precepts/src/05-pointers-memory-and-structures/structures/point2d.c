#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

struct Point2D_t {
    double x; /* X-coordinate */
    double y; /* Y-coordinate */
} typedef Point2D;

/*
 * Point2D * GenRandomPoint1(void)
 *
 * [Description]
 *    Generate random point (x, y) in ([0, 1], [0, 1])
 *
 * [Return Value]
 *    Returns Point2D with random x and y set.
 *
 * [Notes]
 *
 */
Point2D
GenRandomPoint1(void) {
    Point2D ret;

    ret.x = (double) rand() / (double) RAND_MAX;
    ret.y = (double) rand() / (double) RAND_MAX;

    return ret;
}

/*
 * Point2D * GenRandomPoint2(void)
 *
 * [Description]
 *    Generate random point (x, y) in ([0, 1], [0, 1])
 *
 * [Return Value]
 *    On success, returns Point2D * with random x and y set.
 *    On failure, returns NULL.
 *
 * [Notes]
 *    You must free the returned memory buffer after using it.
 *
 */
Point2D *
GenRandomPoint2(void) {
    Point2D *ret = NULL;

    ret = (Point2D *) malloc(sizeof(Point2D));
    if (ret != NULL) {
        ret->x = (double) rand() / (double) RAND_MAX;
        ret->y = (double) rand() / (double) RAND_MAX;
    }

    return ret;
}

/*
 * Point2D * GenRandomPoint3(void)
 *
 * [Description]
 *    Generate random point (x, y) in ([0, 1], [0, 1])
 *
 * [Return Value]
 *    On success, returns 0.
 *    On failure, returns -1.
 *
 * [Notes]
 *    Caller's input Point2D is filled with random coordinates.
 *
 */
int
GenRandomPoint3(Point2D *input) {
    int ret = -1;

    if (input != NULL) {
        input->x = (double) rand() / (double) RAND_MAX;
        input->y = (double) rand() / (double) RAND_MAX;
    }

    return ret;
}

/*
 * double CalcTriangleArea(Point2D *, Point2D *, Point2D *)
 *
 * [Description]
 *    Calculate area enclosed by the triangle
 *
 * [Return Value]
 *    Returns area size of triangle
 *
 * [Notes]
 *    Passed pointer should all be a valid pointer
 *    All three points should not lie in same line.
 *
 */

double
CalcTriangleArea(Point2D *p1, Point2D *p2, Point2D *p3) {
    assert(p1);
    assert(p2);
    assert(p3);

    return 0.5 * fabs(p1->x * p2->y + p2->x * p3->y + p3->x * p1->y
                - p1->x * p3->y - p3->x * p2->y - p2->x * p1->y);
}

int
main(void) {
    Point2D p1, *pp2, *pp3;
   
    /* Change the random seed value with time() */
    srand(time(NULL));

    if ((pp3 = (Point2D *)malloc(sizeof(Point2D))) == NULL) {
        fprintf(stderr, "Failed to allocate buffer for pp3\n");
        exit(1);
    }

    /* initialize 3 random points */
    p1 = GenRandomPoint1();
    pp2 = GenRandomPoint2();
    GenRandomPoint3(pp3);

    /* Print the size of random triangle */
    printf("Random triangle is %g sized\n", CalcTriangleArea(&p1, pp2, pp3));

    /* free up resources */
    free(pp2);
    free(pp3);

    return 0;
}
