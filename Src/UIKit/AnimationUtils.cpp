#include "Common/Precompile.h"

#include "UIKit/AnimationUtils.h"

namespace d14engine::uikit
{
    float AnimationUtils::AccelUniformDecelMotion(float dx, float dt, float s, float t1, float t2)
    {
        //       acceleration             uniform              deceleration
        // start -----------> waypoint 1 ----------> waypoint2 -----------> end
        //          motion                 motion                 motion
        //
        //  s --- total distance
        //  v --- uniform speed
        //  a --- ac/deceleration
        // t1 --- variable speed motion time, i.e. start to waypoint 1 (waypoint 2 to end)
        // t2 --- uniform speed motion time, i.e. waypoint 1 to waypoint 2.
        // 
        // v = s / (t1 + t2)
        // 
        // Then simple to get the distance between start and waypoint 1:
        // 
        // s1 = v * t1 / 2 = s * t1 / [2 * (t1 + t2)]
        // 
        // We can compute the variable speed (between start and waypoint 1) from current position:
        // 
        // v1 = sqrt[ (2 * s * x) / (t1 * (t1 + t2)) ]

        if (dx < s)
        {
            float s1 = s * t1 / (2.0f * (t1 + t2));

            // Set uniform speed as default value; also note the formula of variable speed
            // contains this item, so we can reuse it in place to reduce some calculation.
            float currSpeed = s / (t1 + t2);

            // Acceleration motion
            if (dx < s1)
            {
                // We must add a small increment (0.1f) to push it moving;
                // otherwise it will always stay at the starting position.
                currSpeed = sqrt(2.0f * currSpeed * (dx + 0.1f) / t1);
            }
            // Deceleration motion, symmetrical with acceleration.
            else if (dx > (s - s1))
            {
                // Do NOT repeat the above trick here (i.e. add a small increment),
                // since that might cause passing through a negative value to sqrt.
                currSpeed = sqrt(2.0f * currSpeed * (s - dx) / t1);
            }
            dx += dt * currSpeed;
        }
        return std::clamp(dx, 0.0f, s);
    }
}