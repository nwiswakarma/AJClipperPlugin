////////////////////////////////////////////////////////////////////////////////
//
// MIT License
// 
// Copyright (c) 2018-2019 Nuraga Wiswakarma
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
////////////////////////////////////////////////////////////////////////////////
// 

#pragma once

#include "CoreMinimal.h"
#include "AJCTypes.h"

class AJCLIPPERPLUGIN_API FAJCUtilityLibrary
{
public:

    FORCEINLINE static ClipperLib::ClipType GetClipType(EAJCClipType ClipType)
    {
        switch (ClipType)
        {
            case EAJCClipType::CTIntersection:
                return ClipperLib::ctIntersection;
            case EAJCClipType::CTUnion:
                return ClipperLib::ctUnion;
            case EAJCClipType::CTDifference:
                return ClipperLib::ctDifference;
            case EAJCClipType::CTXor:
                return ClipperLib::ctXor;

            default:
                return ClipperLib::ctIntersection;
        }
    }

    FORCEINLINE static ClipperLib::PolyFillType GetPolyFillType(EAJCPolyFillType PolyFillType)
    {
        switch (PolyFillType)
        {
            case EAJCPolyFillType::PFTEvenOdd:
                return ClipperLib::pftEvenOdd;
            case EAJCPolyFillType::PFTNonZero:
                return ClipperLib::pftNonZero;
            case EAJCPolyFillType::PFTPositive:
                return ClipperLib::pftPositive;
            case EAJCPolyFillType::PFTNegative:
                return ClipperLib::pftNegative;

            default:
                return ClipperLib::pftEvenOdd;
        }
    }

    FORCEINLINE static ClipperLib::JoinType GetJoinType(EAJCJoinType JoinType)
    {
        switch (JoinType)
        {
            case EAJCJoinType::JTSquare:
                return ClipperLib::jtSquare;
            case EAJCJoinType::JTRound:
                return ClipperLib::jtRound;
            case EAJCJoinType::JTMiter:
                return ClipperLib::jtMiter;

            default:
                return ClipperLib::jtSquare;
        }
    }

    FORCEINLINE static ClipperLib::EndType GetEndType(EAJCEndType EndType)
    {
        switch (EndType)
        {
            case EAJCEndType::ETClosedPolygon:
                return ClipperLib::etClosedPolygon;
            case EAJCEndType::ETClosedLine:
                return ClipperLib::etClosedLine;
            case EAJCEndType::ETOpenButt:
                return ClipperLib::etOpenButt;
            case EAJCEndType::ETOpenSquare:
                return ClipperLib::etOpenSquare;
            case EAJCEndType::ETOpenRound:
                return ClipperLib::etOpenRound;

            default:
                return ClipperLib::etClosedPolygon;
        }
    }

    // Path creation functions

    static void ConvertVectorPathToPointPath(const FAJCVectorPath& VectorPath, FAJCPathRef& PathRef);
    static void ConvertVectorPathToPointPath(const FAJCVectorPath& VectorPath, FAJCPointPath& PointPath);
    static void ConvertPointPathsToVectorPaths(const FAJCPointPaths& PointPaths, FAJCVectorPaths& VectorPaths);
    static void ConvertPointPathsToVectorPaths(const FAJCPointPaths& PointPaths, TArray<FAJCVectorPathRef>& VectorPaths);

    // Path clip functions

    static void Clip(
        FAJCPointPaths& Solution,
        const TArray<FAJCPathRef>& SubjectPathRefs,
        const TArray<FAJCPathRef>& ClipPathRefs,
        EAJCClipType ClipType,
        EAJCPolyFillType FillType,
        bool bPreserveCollinear = false
        );

    static void Clip(
        FAJCPointPaths& Solution,
        const FAJCPathRef& SubjectPathRef,
        const FAJCPathRef& ClipPathRef,
        EAJCClipType ClipType,
        EAJCPolyFillType FillType,
        bool bPreserveCollinear = false
        );

    static void OffsetClip(
        FAJCPointPaths& Paths,
        const FAJCPathRef& PathRef,
        const FAJCOffsetClipperConfig& Config,
        bool bSimplifyPath = true,
        bool bReverseOutput = false
        );

    static void OffsetClip(
        FAJCPointPaths& Paths,
        const FAJCPointPaths& InPaths,
        const FAJCOffsetClipperConfig& Config,
        EAJCJoinType InJoinType,
        EAJCEndType InEndType
        );

    // Path Utility functions

    static void SimplifyPath(const FAJCPathRef& PathRef, FAJCPointPaths& OutPaths);
    static void SimplifyPaths(FAJCPointPaths& OutPaths, const TArray<FAJCPathRef>& PathRefs, bool bPreserveCollinear = false);

    static int32 IsPointOnPolygon(const FVector2D& Point, const FAJCPointPath& InPath);
    static int32 IsPointOnPolygons(const FVector2D& Point, const FAJCPointPaths& InPaths);

    static float GetVectorPathArea(const FAJCVectorPathRef& VectorPath);
};
