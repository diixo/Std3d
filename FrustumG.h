
/* ------------------------------------------------------

   View Frustum - Std3D

--------------------------------------------------------*/


#ifndef _FRUSTUM_
#define _FRUSTUM_

#include "Vec3.h"
#include "Matrix.hpp"
#include "Plane.h"
#include "AABox.h"


// Orbital position
struct CPosition
{
   CPosition::CPosition(const float pitch, const float yaw, const float radius)
      : mPitch(pitch)
      , mYaw(yaw)
      , mRadius(radius)
   {
   }

   inline
   CPosition& operator += (float increase)
   {
      mRadius = maximum(0.f, mRadius + increase);
      return *this;
   }

   inline
   CPosition& operator -= (float increase)
   {
      mRadius = maximum(0.f, mRadius - increase);
      return *this;
   }

   float mPitch;
   float mYaw;
   float mRadius;

   Matrix4x4 calculateView() const;
};


inline
Matrix4x4 CPosition::calculateView() const
{
   // Vnew = Mr * (Mt * V)
   // Vnew = (Mr * Mt) * V = Mtransform * V

   // V = Mtransform' * Vnew
   // Mtansform' = (Mr * Mt)' = Mt' * Mr'

   const Vec3 eye(0.f, 0.f, mRadius);
   Matrix4x4 Mrot = Matrix4x4::makeRotateX(mPitch) * Matrix4x4::makeRotateY(mYaw);

   // return Mt' * Mr'
   return Matrix4x4::makeTranslate(-eye) * Mrot;
}


struct CMovement
{
public:

   CMovement(const Vec3& vEye, const Vec3& vLookAt, const Vec3& vUp)
      : eye(vEye)
      , lookAt(vLookAt)
      , up(vUp)
      , pitch(0.f)
      , yaw(0.f)
      , forward(0.f)
      , side(0.f)
   {
   }

   Vec3 eye;
   Vec3 lookAt;
   Vec3 up;

   float pitch;
   float yaw;

   float forward; // movement ahead
   float side;    // movement aside

   Matrix4x4 calculateView() const;
};

inline
Matrix4x4 CMovement::calculateView() const
{
   Matrix4x4 viewMtx      = Matrix4x4::makeLookAt(eye, lookAt, up);                      // final view matrix.
   Matrix4x4 rotateRefCam = Matrix4x4::makeRotateX(pitch) * Matrix4x4::makeRotateY(yaw); // camera coordinate-system rotation.

   // Make final view Matrix with movement but without any rotations.
   viewMtx = Matrix4x4::makeTranslate(Vec3(side, 0.f, forward)) * viewMtx;

   // Apply rotation pitch around camera-origin in camera-coordinate system.
   viewMtx = rotateRefCam * viewMtx;

   return viewMtx;
}


class FrustumG
{
public:

   enum EClipPlane
   {
      P_TOP = 0,
      P_BOTTOM,
      P_LEFT,
      P_RIGHT,
      P_NEAR,
      P_FAR,
      P_AMOUNT
   };

   enum 
   { 
      OUTSIDE,  
      INSIDE, 
      INTERSECT 
   };

   FrustumG();

   void setCamInternals(float fovY, UInt32 width, UInt32 height, float nearD, float farD);

   void update(const Vec3& eye, const Vec3& lookAt, const Vec3& up);

   void print() const;

////////////////////////////////////////////////////////////////

   bool pointInFrustum(const Vec3& point) const;
   int sphereInFrustum(const Vec3& center, float radius) const;
   int boxInFrustum   (const AABox& box) const;

//protected:

   Plane mClipPlanes[P_AMOUNT];                 ///< clipping planes

   Vec3 mNTL, mNTR, mNBL, mNBR, mFTL, mFTR, mFBL, mFBR;   ///< frustum pyramid points

   float mNearD, mFarD, mRatio, mFovY;            ///< camera configuration

   float mNW, mNH, mFW, mFH;

};


#endif
