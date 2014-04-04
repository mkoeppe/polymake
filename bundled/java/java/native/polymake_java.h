/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class de_tuberlin_polymake_common_SharedMemoryMatrix */

#ifndef _Included_de_tuberlin_polymake_common_SharedMemoryMatrix
#define _Included_de_tuberlin_polymake_common_SharedMemoryMatrix
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     de_tuberlin_polymake_common_SharedMemoryMatrix
 * Method:    finalize
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_de_tuberlin_polymake_common_SharedMemoryMatrix_finalize
  (JNIEnv *, jobject);

/*
 * Class:     de_tuberlin_polymake_common_SharedMemoryMatrix
 * Method:    attachToShm
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_de_tuberlin_polymake_common_SharedMemoryMatrix_attachToShm
  (JNIEnv *, jobject, jint);

/*
 * Class:     de_tuberlin_polymake_common_SharedMemoryMatrix
 * Method:    copyCoords
 * Signature: (Lde/tuberlin/polymake/common/geometry/PointSet;)V
 */
JNIEXPORT void JNICALL Java_de_tuberlin_polymake_common_SharedMemoryMatrix_copyCoords__Lde_tuberlin_polymake_common_geometry_PointSet_2
  (JNIEnv *, jobject, jobject);

/*
 * Class:     de_tuberlin_polymake_common_SharedMemoryMatrix
 * Method:    copyCoords
 * Signature: (ILde/tuberlin/polymake/common/geometry/PolymakePoint;)V
 */
JNIEXPORT void JNICALL Java_de_tuberlin_polymake_common_SharedMemoryMatrix_copyCoords__ILde_tuberlin_polymake_common_geometry_PolymakePoint_2
  (JNIEnv *, jobject, jint, jobject);

/*
 * Class:     de_tuberlin_polymake_common_SharedMemoryMatrix
 * Method:    setCoords
 * Signature: (Lde/tuberlin/polymake/common/geometry/PointSet;)V
 */
JNIEXPORT void JNICALL Java_de_tuberlin_polymake_common_SharedMemoryMatrix_setCoords__Lde_tuberlin_polymake_common_geometry_PointSet_2
  (JNIEnv *, jobject, jobject);

/*
 * Class:     de_tuberlin_polymake_common_SharedMemoryMatrix
 * Method:    setCoords
 * Signature: (ILde/tuberlin/polymake/common/geometry/PolymakePoint;)V
 */
JNIEXPORT void JNICALL Java_de_tuberlin_polymake_common_SharedMemoryMatrix_setCoords__ILde_tuberlin_polymake_common_geometry_PolymakePoint_2
  (JNIEnv *, jobject, jint, jobject);

/*
 * Class:     de_tuberlin_polymake_common_SharedMemoryMatrix
 * Method:    getNPoints
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_de_tuberlin_polymake_common_SharedMemoryMatrix_getNPoints
  (JNIEnv *, jobject);

/*
 * Class:     de_tuberlin_polymake_common_SharedMemoryMatrix
 * Method:    getDim
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_de_tuberlin_polymake_common_SharedMemoryMatrix_getDim
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif