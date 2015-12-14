#include "wisard.h"
#include "androidutils.h"
#include "../../../../cpp/include/wup/wup.hpp"

using wup::Wisard;
using wup::WUPException;
using wup::Pattern;
using wup::sbreader;
using wup::sbwriter;

JNIEXPORT jlong JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1create__II
  (JNIEnv * env, jclass c, jint inputBits, jint numRams)
{
    try {
        return (jlong) new Wisard((int)inputBits, (int)numRams);
    } catch (WUPException e) {
        return throwWUPException(env, e.what());
    }
}

JNIEXPORT jlong JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1create__III
  (JNIEnv * env, jclass c, jint inputBits, jint numRams, jint classes)
{
    try {
        return (jlong) new Wisard((int)inputBits, (int)numRams, (int)classes);
    } catch (WUPException e) {
        return throwWUPException(env, e.what());
    }
}

JNIEXPORT jlong JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1create__Ljava_lang_String_2
  (JNIEnv * env, jclass c, jstring filename)
{
    try {
        const char *nativeString = env->GetStringUTFChars(filename, NULL);
        sbreader<int> reader(nativeString, 1024*1024);
        Wisard *w = NULL;
        
        if (reader.good()) {
            try {
                w = new Wisard(reader);
                //LOGI("Import succeeded");
            } catch (WUPException e) {
            	throwWUPException(env, "Could not read from the input file");
            }
        } else {
        	throwWUPException(env, "Could not open the input file");
        }

        env->ReleaseStringUTFChars(filename, nativeString);
        return (jlong) w;
    } catch (WUPException e) {
        return throwWUPException(env, e.what());
    }
}

JNIEXPORT void JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1close
  (JNIEnv *env, jclass c, jlong ptr)
{
    try {
        delete (Wisard*)ptr;
    } catch (WUPException e) {
        throwWUPException(env, e.what());
    }
}

JNIEXPORT void JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1exportTo
  (JNIEnv * env, jclass c, jlong ptr, jstring filename)
{
    try {
        const char *nativeString = env->GetStringUTFChars(filename, NULL);
        sbwriter<int> writer(nativeString, 1024*1024);

        if (writer.good()) {
            try {
                ((Wisard*) ptr)->exportTo(writer);
                //LOGI("Export succeeded");
            } catch (WUPException e) {
            	throwWUPException(env, "Could not write to the output file");
            }
        } else {
        	throwWUPException(env, "Could not create the output file");
        }

        env->ReleaseStringUTFChars(filename, nativeString);
    } catch (WUPException e) {
        throwWUPException(env, e.what());
    }
}

JNIEXPORT jint JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1classes
  (JNIEnv * env, jclass c, jlong ptr)
{
    try {
        return ((Wisard*)ptr)->classes();
    } catch (WUPException e) {
        return throwWUPException(env, e.what());
    }
}

JNIEXPORT jint JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1ramBits
  (JNIEnv * env, jclass c, jlong ptr)
{
    try {
        return ((Wisard*)ptr)->ramBits();
    } catch (WUPException e) {
        return throwWUPException(env, e.what());
    }
}


JNIEXPORT jint JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1inputBits
  (JNIEnv * env, jclass c, jlong ptr)
{
    try {
        return ((Wisard*)ptr)->inputBits();
    } catch (WUPException e) {
        return throwWUPException(env, e.what());
    }
}

JNIEXPORT void JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1learn
  (JNIEnv * env, jclass c, jlong ptr, jintArray pattern, jint target)
{
    try {
        ((Wisard*)ptr)->learn(Pattern(env, pattern), (int)target);
    } catch (WUPException e) {
        throwWUPException(env, e.what());
    }
}

JNIEXPORT void JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1forget
  (JNIEnv * env, jclass c, jlong ptr, jint target)
{
    try {
        ((Wisard*)ptr)->forget((int)target);
    } catch (WUPException e) {
        throwWUPException(env, e.what());
    }
}

JNIEXPORT jint JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1readCounts
  (JNIEnv * env, jclass c, jlong ptr, jintArray pattern)
{
    try {
        return ((Wisard*)ptr)->readCounts(Pattern(env, pattern));
    } catch (WUPException e) {
        return throwWUPException(env, e.what());
    }
}

JNIEXPORT jint JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1readBleaching__J_3I
  (JNIEnv * env, jclass c, jlong ptr, jintArray pattern)
{
    try {
        return ((Wisard*)ptr)->readBleaching(Pattern(env, pattern));
    } catch (WUPException e) {
        return throwWUPException(env, e.what());
    }
}

JNIEXPORT jint JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1readBleaching__J_3IIF
  (JNIEnv * env, jclass c, jlong ptr, jintArray pattern, jint step, jfloat minConfidence)
{
    try {
        return ((Wisard*)ptr)->readBleaching(
                Pattern(env, pattern), (int)step, (float)minConfidence);
    } catch (WUPException e) {
        return throwWUPException(env, e.what());
    }
}

JNIEXPORT jint JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1readBinary
  (JNIEnv * env, jclass c, jlong ptr, jintArray pattern)
{
    try {
        return ((Wisard*)ptr)->readBinary(Pattern(env, pattern));
    } catch (WUPException e) {
        return throwWUPException(env, e.what());
    }
}

JNIEXPORT jfloat JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1getConfidence
  (JNIEnv * env, jclass c, jlong ptr)
{
    try {
        return ((Wisard*)ptr)->getConfidence();
    } catch (WUPException e) {
        return throwWUPException(env, e.what());
    }
}

JNIEXPORT jfloat JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1getExcitation
  (JNIEnv * env, jclass c, jlong ptr, jint target)
{
    try {
        return ((Wisard*)ptr)->getExcitation((int)target);
    } catch (WUPException e) {
        return throwWUPException(env, e.what());
    }
}

JNIEXPORT jint JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1getFirstBestPrediction
  (JNIEnv * env, jclass c, jlong ptr)
{
    try {
        return ((Wisard*)ptr)->getFirstBestPrediction();
    } catch (WUPException e) {
        return throwWUPException(env, e.what());
    }
}

JNIEXPORT jint JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1getSecondBestPrediction
  (JNIEnv * env, jclass c, jlong ptr)
{
    try {
        return ((Wisard*)ptr)->getSecondBestPrediction();
    } catch (WUPException e) {
        return throwWUPException(env, e.what());
    }
}

JNIEXPORT jint JNICALL Java_com_github_diegofps_android_andwup_models_Wisard_native_1getThirdBestPrediction
  (JNIEnv * env, jclass c, jlong ptr)
{
    try {
        return ((Wisard*)ptr)->getThirdBestPrediction();
    } catch (WUPException e) {
        return throwWUPException(env, e.what());
    }
}

