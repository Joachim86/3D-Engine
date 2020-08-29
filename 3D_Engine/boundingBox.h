#pragma once

#include "engine.h"

//http://www.miguelcasillas.com/?p=30

enum Direction
{
    LEFT,
    RIGHT,
    FORWARD,
    BACKWARD,
    UP,
    DOWN
};

struct TAABB
{
    olc::vec3d m_vecMax;
    olc::vec3d m_vecMin;
};

bool AABBtoAABB(const TAABB& tBox1, const TAABB& tBox2)
{
    return(tBox1.m_vecMax.x > tBox2.m_vecMin.x  &&
           tBox1.m_vecMin.x < tBox2.m_vecMax.x  &&
           tBox1.m_vecMax.y > tBox2.m_vecMin.y  &&
           tBox1.m_vecMin.y < tBox2.m_vecMax.y  &&
           tBox1.m_vecMax.z > tBox2.m_vecMin.z  &&
           tBox1.m_vecMin.z < tBox2.m_vecMax.z);
}

Direction getBlockedDirection(const TAABB& tBox1, const TAABB& tBox2)
{

}