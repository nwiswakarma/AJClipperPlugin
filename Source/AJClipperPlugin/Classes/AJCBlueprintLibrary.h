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
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AJCTypes.h"
#include "AJCBlueprintLibrary.generated.h"

USTRUCT(BlueprintType)
struct AJCLIPPERPLUGIN_API FAJCClipperConfig
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    EAJCClipType ClipType = EAJCClipType::CTIntersection;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    EAJCPolyFillType PolyFillType = EAJCPolyFillType::PFTEvenOdd;

    // ClipperLib enum getter

    ClipperLib::ClipType GetClipType() const;
    ClipperLib::PolyFillType GetPolyFillType() const;
};

UCLASS()
class AJCLIPPERPLUGIN_API UAJCBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable)
    static void SimplifyPath(const FAJCPathRef& PathRef, FAJCPathsRef& OutPaths);

    UFUNCTION(BlueprintCallable)
    static void ConvertVectorPathToPointPath(const FAJCVectorPathRef& VectorPath, FAJCPathRef& OutPath);

    UFUNCTION(BlueprintCallable)
    static TArray<FAJCVectorPathRef> ConvertPointPathsToVectorPaths(const FAJCPathsRef& PointPaths);

    UFUNCTION(BlueprintCallable)
    static void CreateVectorPathsFromPaths(const FAJCPathsRef& InPaths, TArray<FAJCVectorPathRef>& OutPaths, bool bFlattenPaths = false);

    UFUNCTION(BlueprintCallable)
    static void CreatePathsFromVectorPaths(const TArray<FAJCVectorPathRef>& InPaths, FAJCPathsRef& OutPaths);

    UFUNCTION(BlueprintCallable)
    static void VectorPathClip(
        TArray<FAJCVectorPathRef>& OutPaths,
        const TArray<FAJCVectorPathRef>& SubjectVectorPaths,
        const TArray<FAJCVectorPathRef>& ClipVectorPaths,
        EAJCClipType ClipType,
        EAJCPolyFillType FillType = EAJCPolyFillType::PFTEvenOdd
        );

    UFUNCTION(BlueprintCallable)
    static void VectorPathOffsetClip(
        TArray<FAJCVectorPathRef>& OutPaths,
        const FAJCVectorPathRef& VectorPath,
        const FAJCOffsetClipperConfig& Config,
        EAJCJoinType JoinType,
        EAJCEndType EndType,
        bool bSimplifyPath = true,
        bool bReverseOutput = false
        );

    UFUNCTION(BlueprintCallable)
    static void VectorPathClipByOffset(
        TArray<FAJCVectorPathRef>& OutPaths,
        const FAJCVectorPathRef& VectorPath,
        const FAJCOffsetClipperConfig& Config,
        EAJCJoinType JoinType,
        EAJCEndType EndType,
        bool bSimplifyPath = true
        );

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Offset Clip Single"))
    static void OffsetClipSingle(FAJCPathsRef& OutPaths, const FAJCPathRef& PathRef, const FAJCOffsetClipperConfig& Config, bool bReverseOutput);

    UFUNCTION(BlueprintCallable, meta=(DisplayName="Offset Clip Multi"))
    static void OffsetClipMulti(FAJCPathsRef& OutPaths, const FAJCPathsRef& PathsRef, const FAJCOffsetClipperConfig& Config, bool bReverseOutput);

    UFUNCTION(BlueprintCallable)
    static int32 IsPointOnPolygon(const FVector2D& Point, const FAJCPathRef& InPath);

    UFUNCTION(BlueprintCallable)
    static int32 IsPointOnPolygons(const FVector2D& Point, const FAJCPathsRef& InPaths);

    UFUNCTION(BlueprintCallable)
    static int32 FindLargestVectorPath(const TArray<FAJCVectorPathRef>& VectorPaths);
};
