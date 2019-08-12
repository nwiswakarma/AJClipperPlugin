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

#include "AJCUtilityLibrary.h"
#include "Poly/GULPolyUtilityLibrary.h"

ClipperLib::ClipType FAJCClipperConfig::GetClipType() const
{
    return FAJCUtilityLibrary::GetClipType(ClipType);
}

ClipperLib::PolyFillType FAJCClipperConfig::GetPolyFillType() const
{
    return FAJCUtilityLibrary::GetPolyFillType(PolyFillType);
}

void FAJCUtilityLibrary::ConvertVectorPathToPointPath(const FAJCVectorPath& VectorPath, FAJCPathRef& PathRef)
{
    ConvertVectorPathToPointPath(VectorPath, PathRef.Data);
}

void FAJCUtilityLibrary::ConvertVectorPathToPointPath(const FAJCVectorPath& VectorPath, FAJCPointPath& PointPath)
{
    // Clear and reserve space for point path
    PointPath.clear();
    PointPath.reserve(VectorPath.Num());

    for (const FVector2D& Point : VectorPath)
    {
        PointPath << FAJCUtils::ScaleToCIntPoint(Point);
    }
}

void FAJCUtilityLibrary::ConvertPointPathsToVectorPaths(const FAJCPointPaths& PointPaths, FAJCVectorPaths& VectorPaths)
{
    VectorPaths.SetNum(PointPaths.size(), true);

    for (int32 PathIndex=0; PathIndex<VectorPaths.Num(); ++PathIndex)
    {
        FAJCUtils::ScaleToVector2D(PointPaths[PathIndex], VectorPaths[PathIndex]);
    }
}

void FAJCUtilityLibrary::ConvertPointPathsToVectorPaths(const FAJCPointPaths& PointPaths, TArray<FAJCVectorPathRef>& VectorPaths)
{
    VectorPaths.SetNum(PointPaths.size(), true);

    for (int32 PathIndex=0; PathIndex<VectorPaths.Num(); ++PathIndex)
    {
        FAJCUtils::ScaleToVector2D(PointPaths[PathIndex], VectorPaths[PathIndex].Data);
    }
}

void FAJCUtilityLibrary::Clip(
    FAJCPointPaths& Solution,
    const TArray<FAJCPathRef>& SubjectPathRefs,
    const TArray<FAJCPathRef>& ClipPathRefs,
    EAJCClipType ClipType,
    EAJCPolyFillType FillType,
    bool bPreserveCollinear
    )
{
    using namespace ClipperLib;

    Clipper c;

    if (bPreserveCollinear)
    {
        c.PreserveCollinear(true);
    }

    for (int32 i=0; i<SubjectPathRefs.Num(); ++i)
    {
        c.AddPath(SubjectPathRefs[i].Data, ptSubject, SubjectPathRefs[i].bClosedPoly);
    }

    for (int32 i=0; i<ClipPathRefs.Num(); ++i)
    {
        c.AddPath(ClipPathRefs[i].Data, ptClip, ClipPathRefs[i].bClosedPoly);
    }

    c.Execute(GetClipType(ClipType), Solution, GetPolyFillType(FillType));
}

void FAJCUtilityLibrary::Clip(
    FAJCPointPaths& Solution,
    const FAJCPathRef& SubjectPathRef,
    const FAJCPathRef& ClipPathRef,
    EAJCClipType ClipType,
    EAJCPolyFillType FillType,
    bool bPreserveCollinear
    )
{
    ClipperLib::Clipper c;

    if (bPreserveCollinear)
    {
        c.PreserveCollinear(true);
    }

    c.AddPath(SubjectPathRef.Data, ClipperLib::ptSubject, SubjectPathRef.bClosedPoly);
    c.AddPath(ClipPathRef.Data, ClipperLib::ptClip, ClipPathRef.bClosedPoly);

    c.Execute(GetClipType(ClipType), Solution, GetPolyFillType(FillType));
}

void FAJCUtilityLibrary::OffsetClip(const FAJCPathRef& PathRef, const FAJCOffsetClipperConfig& Config, FAJCPointPaths& Paths, const bool bSimplifyPath)
{
    using namespace ClipperLib;

    FAJCPointPaths SourcePaths;

    if (bSimplifyPath)
    {
        // Self union-clip to simplify path
        Clipper c;
        c.AddPath(PathRef.Data, ptSubject, true);
        c.Execute(ctUnion, SourcePaths, pftEvenOdd, pftEvenOdd);
    }
    else
    {
        SourcePaths.push_back(PathRef.Data);
    }

    // Offset clip
    ClipperOffset co(Config.MiterLimit, Config.ArcTolerance);
    co.AddPaths(SourcePaths, PathRef.GetJoinType(), PathRef.GetEndType());
    co.Execute(Paths, (float) FAJCUtils::ScaleToCInt(Config.Delta));
}

void FAJCUtilityLibrary::SimplifyPath(const FAJCPathRef& PathRef, FAJCPointPaths& OutPaths)
{
    using namespace ClipperLib;

    // Self union-clip to simplify clip
    Clipper c;
    c.AddPath(PathRef.Data, ptSubject, true);
    c.Execute(ctUnion, OutPaths, pftEvenOdd, pftEvenOdd);
}

int32 FAJCUtilityLibrary::IsPointOnPolygon(const FVector2D& Point, const FAJCPointPath& InPath)
{
    return ClipperLib::PointInPolygon(FAJCUtils::ScaleToCIntPoint(Point), InPath);
}

int32 FAJCUtilityLibrary::IsPointOnPolygons(const FVector2D& Point, const FAJCPointPaths& InPaths)
{
    for (const FAJCPointPath& Path : InPaths)
    {
        int32 Result = ClipperLib::PointInPolygon(FAJCUtils::ScaleToCIntPoint(Point), Path);

        if (Result != 0)
        {
            return Result;
        }
    }

    return 0;
}

float FAJCUtilityLibrary::GetVectorPathArea(const FAJCVectorPathRef& VectorPath)
{
    return UGULPolyUtilityLibrary::GetArea(VectorPath.Data);
}
