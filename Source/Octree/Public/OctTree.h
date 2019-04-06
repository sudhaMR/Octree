// Fill out your copyright notice in the Description page of Project Settings.
#include "SceneObject.h"
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OctTree.generated.h"

UCLASS()
class OCTREE_API AOctTree : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOctTree();
	AOctTree(FVector origin, FVector extent);
	struct Node
	{
		FVector origin;
		FVector extent;
		int depth;
	};

	Node boundingBox;
	TArray<AOctTree*> children;

	void PrintNodeData();
	void PrintNode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	AOctTree *root;
	AOctTree *parent;

	TArray<FVector> objectPosList;

	TArray<ASceneObject*> nodeObjects;

	int minNodeSize;
	int maxNodeSize;

	int maxObjects;

	UFUNCTION(BlueprintCallable, Category = "BoundingBox")
		void SetBoundingBox(FVector origin, FVector extent, int depth);

	UFUNCTION(BlueprintCallable, Category = "BoundingBox")
		FVector GetBoundingBoxOrigin();

	UFUNCTION(BlueprintCallable, Category = "BoundingBox")
		FVector GetBoundingBoxExtent();

	UFUNCTION(BlueprintCallable, Category = "BoundingBox")
		void SetParent();

	UFUNCTION(BlueprintCallable, Category = "BoundingBox")
		void init(FVector const origin, FVector const extent, int const depth, FColor c, AOctTree *rootNode, AOctTree *parentNode, TArray<FVector> objectPosList);

	UFUNCTION(BlueprintCallable, Category = "BoundingBox")
	void DrawNode(FColor color, FVector origin, FVector extent);

	void BuildTree();

	bool ObjectInNodeRange(FVector object, Node octNode);

	UFUNCTION()
	void SpawnOctree(FVector spawnLocation, FVector spawnExtent, int depth, FColor c);

	UFUNCTION()
	void SpawnObject(FVector objLocation);

	UFUNCTION()
	void GenerateSceneObject();

	UFUNCTION()
	void InsertObjects();
};
