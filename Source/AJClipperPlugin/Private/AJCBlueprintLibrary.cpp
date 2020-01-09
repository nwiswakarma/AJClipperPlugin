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

#include "AJCBlueprintLibrary.h"
#include "AJCUtilityLibrary.h"

void UAJCBlueprintLibrary::SimplifyPath(const FAJCPathRef& PathRef, FAJCPathsRef& OutPaths)
{
    FAJCUtilityLibrary::SimplifyPath(PathRef, OutPaths.Data);
}

void UAJCBlueprintLibrary::ConvertVectorPathToPointPath(const FAJCVectorPathRef& VectorPath, FAJCPathRef& OutPath)
{
    FAJCUtilityLibrary::ConvertVectorPathToPointPath(VectorPath.Data, OutPath);
}

TArray<FAJCVectorPathRef> UAJCBlueprintLibrary::ConvertPointPathsToVectorPaths(const FAJCPathsRef& PathsRef)
{
    TArray<FAJCVectorPathRef> OutPaths;
    FAJCUtilityLibrary::ConvertPointPathsToVectorPaths(PathsRef.Data, OutPaths);

    return MoveTemp(OutPaths);
}

void UAJCBlueprintLibrary::CreateVectorPathsFromPaths(const FAJCPathsRef& InPaths, TArray<FAJCVectorPathRef>& OutPaths, bool bFlattenPaths)
{
    const FAJCPointPaths& Paths(InPaths.Data);
    TArray<FAJCVectorPathRef> VectorPaths;
    int32 TotalPointCount = 0;

    VectorPaths.SetNum(InPaths.GetPathCount(), true);

    for (int32 PathIndex=0; PathIndex<VectorPaths.Num(); ++PathIndex)
    {
        FAJCUtils::ScaleToVector2D(Paths[PathIndex], VectorPaths[PathIndex].Data);
        TotalPointCount += VectorPaths[PathIndex].Data.Num();
    }

    if (bFlattenPaths)
    {
        OutPaths.Reset();

        if (VectorPaths.Num() > 0)
        {
            OutPaths.SetNum(1, true);
            OutPaths[0].Data.Reserve(TotalPointCount);

            for (int32 PathIndex=0; PathIndex<VectorPaths.Num(); ++PathIndex)
            {
                OutPaths[0].Data.Append(VectorPaths[PathIndex].Data);
            }
        }
    }
    else
    {
        OutPaths = MoveTemp(VectorPaths);
    }
}

void UAJCBlueprintLibrary::CreatePathsFromVectorPaths(const TArray<FAJCVectorPathRef>& InPaths, FAJCPathsRef& OutPaths)
{
    OutPaths.Data.resize(InPaths.Num());

    for (int32 i=0; i<InPaths.Num(); ++i)
    {
        const FAJCVectorPathRef& VectorPath(InPaths[i]);
        FAJCPointPath& Path(OutPaths.Data[i]);

        for (const FVector2D& Point : VectorPath.Data)
        {
            Path << FAJCUtils::ScaleToCIntPoint(Point);
        }
    }
}

void UAJCBlueprintLibrary::VectorPathClip(
    TArray<FAJCVectorPathRef>& OutPaths,
    const TArray<FAJCVectorPathRef>& SubjectVectorPaths,
    const TArray<FAJCVectorPathRef>& ClipVectorPaths,
    EAJCClipType ClipType,
    EAJCPolyFillType FillType,
    bool bPreserveCollinear
    )
{
    TArray<FAJCPathRef> SubjectPaths;
    TArray<FAJCPathRef> ClipPaths;

    SubjectPaths.SetNum(SubjectVectorPaths.Num());
    ClipPaths.SetNum(ClipVectorPaths.Num());

    for (int32 i=0; i<SubjectPaths.Num(); ++i)
    {
        ConvertVectorPathToPointPath(SubjectVectorPaths[i], SubjectPaths[i]);
        SubjectPaths[i].bClosedPoly = SubjectVectorPaths[i].bClosedPoly;
    }

    for (int32 i=0; i<ClipPaths.Num(); ++i)
    {
        ConvertVectorPathToPointPath(ClipVectorPaths[i], ClipPaths[i]);
        ClipPaths[i].bClosedPoly = ClipVectorPaths[i].bClosedPoly;
    }

    FAJCPathsRef SolutionRef;
    FAJCUtilityLibrary::Clip(SolutionRef.Data, SubjectPaths, ClipPaths, ClipType, FillType, bPreserveCollinear);

    OutPaths.Reset();
    FAJCUtilityLibrary::ConvertPointPathsToVectorPaths(SolutionRef.Data, OutPaths);
}

void UAJCBlueprintLibrary::VectorPathOffsetClip(
    TArray<FAJCVectorPathRef>& OutPaths,
    const FAJCVectorPathRef& VectorPath,
    const FAJCOffsetClipperConfig& Config,
    EAJCJoinType JoinType,
    EAJCEndType EndType,
    bool bSimplifyPath,
    bool bReverseOutput
    )
{
    FAJCPathRef PathRef;
    ConvertVectorPathToPointPath(VectorPath, PathRef);

    PathRef.JoinType = JoinType;
    PathRef.EndType = EndType;

    FAJCPathsRef PathsRef;
    FAJCUtilityLibrary::OffsetClip(PathsRef.Data, PathRef, Config, bSimplifyPath, bReverseOutput);

    OutPaths.Reset();
    FAJCUtilityLibrary::ConvertPointPathsToVectorPaths(PathsRef.Data, OutPaths);
}

void UAJCBlueprintLibrary::VectorPathClipByOffset(
    TArray<FAJCVectorPathRef>& OutPaths,
    const FAJCVectorPathRef& VectorPath,
    const FAJCOffsetClipperConfig& Config,
    EAJCJoinType JoinType,
    EAJCEndType EndType,
    bool bSimplifyPath
    )
{
    TArray<FAJCPathRef> SubjectPaths;
    TArray<FAJCPathRef> ClipPaths;

    SubjectPaths.SetNum(1);

    // Convert source vector path to point path

    FAJCPathRef& SrcPathRef(SubjectPaths[0]);
    ConvertVectorPathToPointPath(VectorPath, SrcPathRef);

    // Generate offset path

    SrcPathRef.bClosedPoly = true;
    SrcPathRef.JoinType = JoinType;
    SrcPathRef.EndType = EndType;

    FAJCPathsRef OffsetPathsRef;
    FAJCUtilityLibrary::OffsetClip(OffsetPathsRef.Data, SrcPathRef, Config, bSimplifyPath);

    ClipPaths.SetNum(OffsetPathsRef.GetPathCount());

    for (int32 i=0; i<ClipPaths.Num(); ++i)
    {
        ClipPaths[i].Data = OffsetPathsRef.Data[i];
    }

    // Clip source point path with the offset path

    FAJCPathsRef SolutionRef;
    FAJCUtilityLibrary::Clip(SolutionRef.Data, SubjectPaths, ClipPaths, EAJCClipType::CTDifference, EAJCPolyFillType::PFTEvenOdd);

    // Convert point path results to vector paths

    OutPaths.Reset();
    FAJCUtilityLibrary::ConvertPointPathsToVectorPaths(SolutionRef.Data, OutPaths);
}

void UAJCBlueprintLibrary::OffsetClipSingle(FAJCPathsRef& OutPaths, const FAJCPathRef& PathRef, const FAJCOffsetClipperConfig& Config, bool bReverseOutput)
{
    ClipperLib::ClipperOffset co(Config.MiterLimit, Config.ArcTolerance);
    co.AddPath(PathRef.Data, PathRef.GetJoinType(), PathRef.GetEndType());
    co.Execute(OutPaths.Data, (float)FAJCUtils::ScaleToCInt(Config.Delta));

    if (bReverseOutput)
    {
        ClipperLib::ReversePaths(OutPaths.Data);
    }
}

void UAJCBlueprintLibrary::OffsetClipMulti(FAJCPathsRef& OutPaths, const FAJCPathsRef& PathsRef, const FAJCOffsetClipperConfig& Config, bool bReverseOutput)
{
    ClipperLib::ClipperOffset co(Config.MiterLimit, Config.ArcTolerance);
    co.AddPaths(PathsRef.Data, ClipperLib::jtSquare, ClipperLib::etClosedPolygon);
    co.Execute(OutPaths.Data, (float)FAJCUtils::ScaleToCInt(Config.Delta));

    if (bReverseOutput)
    {
        ClipperLib::ReversePaths(OutPaths.Data);
    }
}

int32 UAJCBlueprintLibrary::IsPointOnPolygon(const FVector2D& Point, const FAJCPathRef& InPath)
{
    return ClipperLib::PointInPolygon(FAJCUtils::ScaleToCIntPoint(Point), InPath.Data);
}

int32 UAJCBlueprintLibrary::IsPointOnPolygons(const FVector2D& Point, const FAJCPathsRef& InPaths)
{
    for (const FAJCPointPath& Path : InPaths.Data)
    {
        int32 Result = ClipperLib::PointInPolygon(FAJCUtils::ScaleToCIntPoint(Point), Path);

        if (Result != 0)
        {
            return Result;
        }
    }

    return 0;
}

int32 UAJCBlueprintLibrary::FindLargestVectorPath(const TArray<FAJCVectorPathRef>& VectorPaths)
{
    int32 LargestPathIndex = -1;
    float LargestArea = -1.f;

    for (int32 i=0; i<VectorPaths.Num(); ++i)
    {
        const FAJCVectorPathRef& VectorPath(VectorPaths[i]);
        const float Area = FMath::Abs(FAJCUtilityLibrary::GetVectorPathArea(VectorPath));

        if (LargestArea < Area)
        {
            LargestPathIndex = i;
            LargestArea = Area;
        }
    }

    return LargestPathIndex;
}
