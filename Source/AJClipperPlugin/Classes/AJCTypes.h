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

#include "CoreTypes.h"
#include "clipper.hpp"
#include "AJCTypes.generated.h"

#define FAJCCONST_INT8_SCALE     100000000.f
#define FAJCCONST_INT8_SCALE_INV 0.00000001f

#define FAJCCONST_INT4_SCALE     10000.f
#define FAJCCONST_INT4_SCALE_INV 0.0001f

#define FAJCCONST_INT3_SCALE     1000.f
#define FAJCCONST_INT3_SCALE_INV 0.001f

#define FAJCCONST_INT2_SCALE     100.f
#define FAJCCONST_INT2_SCALE_INV 0.01f

#define FAJCCONST_INT_SCALE     FAJCCONST_INT3_SCALE
#define FAJCCONST_INT_SCALE_INV FAJCCONST_INT3_SCALE_INV

typedef TArray<FVector2D>      FAJCVectorPath;
typedef TArray<FAJCVectorPath> FAJCVectorPaths;

typedef ClipperLib::cInt     FAJCInt;
typedef ClipperLib::IntPoint FAJCIntPoint;
typedef ClipperLib::Path     FAJCPointPath;
typedef ClipperLib::Paths    FAJCPointPaths;

UENUM(BlueprintType)
enum class EAJCClipType : uint8
{
    CTIntersection,
    CTUnion,
    CTDifference,
    CTXor
};

UENUM(BlueprintType)
enum class EAJCPolyType : uint8
{
    PTSubject,
    PTClip
};

UENUM(BlueprintType)
enum class EAJCPolyFillType : uint8
{
    PFTEvenOdd,
    PFTNonZero,
    PFTPositive,
    PFTNegative
};

UENUM(BlueprintType)
enum class EAJCJoinType : uint8
{
    JTSquare,
    JTRound,
    JTMiter
};

UENUM(BlueprintType)
enum class EAJCEndType : uint8
{
    ETClosedPolygon,
    ETClosedLine,
    ETOpenButt,
    ETOpenSquare,
    ETOpenRound
};

USTRUCT(BlueprintType)
struct AJCLIPPERPLUGIN_API FAJCOffsetClipperConfig
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    float Delta = 0.f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    float MiterLimit = 2.f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    float ArcTolerance = 0.25f;

    FAJCOffsetClipperConfig() = default;

    FAJCOffsetClipperConfig(float InDelta)
        : Delta(InDelta)
    {
    }
};

USTRUCT(BlueprintType)
struct AJCLIPPERPLUGIN_API FAJCPathPtrRef
{
	GENERATED_BODY()

    struct FAJCPathRef* Data = nullptr;

    FAJCPathPtrRef() = default;
    FAJCPathPtrRef(struct FAJCPathRef& InPathRef) : Data(&InPathRef) {}

    FORCEINLINE bool IsValid() const
    {
        return Data != nullptr;
    }
};

USTRUCT(BlueprintType)
struct AJCLIPPERPLUGIN_API FAJCPathConstPtrRef
{
	GENERATED_BODY()

    const struct FAJCPathRef* Data = nullptr;

    FAJCPathConstPtrRef() = default;
    FAJCPathConstPtrRef(const struct FAJCPathRef& InPathRef) : Data(&InPathRef) {}

    FORCEINLINE bool IsValid() const
    {
        return Data != nullptr;
    }
};

USTRUCT(BlueprintType)
struct AJCLIPPERPLUGIN_API FAJCVectorPathRef
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
    TArray<FVector2D> Data; // FAJCVectorPath

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    bool bClosedPoly = true;
};

USTRUCT(BlueprintType)
struct AJCLIPPERPLUGIN_API FAJCPathRef
{
	GENERATED_BODY()

    FAJCPointPath Data;

    // Clipper config

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    bool bClosedPoly = true;

    // Clipper offset config

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    EAJCJoinType JoinType = EAJCJoinType::JTSquare;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    EAJCEndType EndType = EAJCEndType::ETClosedPolygon;

    // ClipperLib enum getter

    ClipperLib::JoinType GetJoinType() const
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

    ClipperLib::EndType GetEndType() const
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

    // Path utility functions

    FORCEINLINE int32 Num() const
    {
        return Data.size();
    }

    FORCEINLINE FAJCPathPtrRef AsPtr()
    {
        return FAJCPathPtrRef(*this);
    }

    FORCEINLINE FAJCPathConstPtrRef AsConstPtr() const
    {
        return FAJCPathConstPtrRef(*this);
    }

    FORCEINLINE int32 FindPoint(const FAJCIntPoint& Point) const
    {
        for (int32 i=0; i<Num(); ++i)
        {
            if (Point == Data[i])
            {
                return i;
            }
        }

        return -1;
    }
};

USTRUCT(BlueprintType)
struct AJCLIPPERPLUGIN_API FAJCPathsRef
{
	GENERATED_BODY()

    FAJCPointPaths Data;

    FORCEINLINE int32 GetPathCount() const
    {
        return Data.size();
    }
};

class FAJCUtils
{
public:

    FORCEINLINE static FAJCInt ScaleToCInt(float v)
    {
        return v < 0.f
            ? static_cast<FAJCInt>(v*FAJCCONST_INT_SCALE - .5f)
            : static_cast<FAJCInt>(v*FAJCCONST_INT_SCALE + .5f);
    }

    FORCEINLINE static float ScaleToFloat(FAJCInt v)
    {
        return static_cast<float>(v) * FAJCCONST_INT_SCALE_INV;
    }

    FORCEINLINE static FAJCIntPoint ScaleToCIntPoint(float X, float Y)
    {
        return FAJCIntPoint(ScaleToCInt(X), ScaleToCInt(Y));
    }

    FORCEINLINE static FAJCIntPoint ScaleToCIntPoint(const FVector2D& v)
    {
        return ScaleToCIntPoint(v.X, v.Y);
    }

    FORCEINLINE static void ScaleToCIntPoint(const TArray<FVector2D>& Vectors, FAJCPointPath& Path)
    {
        Path.clear();
        Path.reserve(Vectors.Num());
        for (const FVector2D& Point : Vectors)
            Path << FAJCUtils::ScaleToCIntPoint(Point);
    }

    FORCEINLINE static FVector2D ScaleToVector2D(FAJCInt X, FAJCInt Y)
    {
        return FVector2D(ScaleToFloat(X), ScaleToFloat(Y));
    }

    FORCEINLINE static FVector2D ScaleToVector2D(const FAJCIntPoint& pt)
    {
        return ScaleToVector2D(pt.X, pt.Y);
    }

    FORCEINLINE static void ScaleToVector2D(const FAJCPointPath& Path, TArray<FVector2D>& Vectors)
    {
        Vectors.Reset(Path.size());

        for (const FAJCIntPoint& Point : Path)
        {
            Vectors.Emplace(ScaleToFloat(Point.X), ScaleToFloat(Point.Y));
        }
    }
};

FORCEINLINE uint32 GetTypeHash(const FAJCIntPoint& InPoint)
{
    int64 X = GetTypeHash(InPoint.X);
    int64 Y = GetTypeHash(InPoint.Y);
	return GetTypeHash(X | (Y<<32));
}
