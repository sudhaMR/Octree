// Fill out your copyright notice in the Description page of Project Settings.

#include "OctTree.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"

// Sets default values
AOctTree::AOctTree()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SetBoundingBox(FVector(0, 0, 0), FVector(128,128, 128), int(1));
	minNodeSize = 1;
	maxObjects = 3;
	maxNodeSize = boundingBox.extent.X;
	isPaused = false;
}

void AOctTree::SpawnOctree(FVector spawnLocation, FVector spawnExtent, int depth, FColor c)
{
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfo;

	AOctTree *newTree = GetWorld()->SpawnActor<AOctTree>(spawnLocation, Rotation, SpawnInfo);

	newTree->init(spawnLocation, spawnExtent, depth, c, root, this, objectPosList);

	children.Push(newTree);
}

//Replaced init method 
void AOctTree::init(FVector const origin, FVector const extent, int const depth, FColor color, AOctTree *rootNode, AOctTree *parentNode, TArray<FVector> remainingObjPostList)
{
	PrimaryActorTick.bCanEverTick = false;
	SetBoundingBox(origin, extent,depth);
	minNodeSize = 1;
	maxNodeSize = (extent).X;
	root = rootNode;
	parent = parentNode;
	maxObjects = 3;
	isPaused = false;
	Node currentNode;

	while(remainingObjPostList.Num() > 0)
	{
		objectPosList.Push(remainingObjPostList.Pop(true));
	}

	currentNode.origin = origin;
	currentNode.extent = extent;
	DrawNode(color,origin,extent);
}

void AOctTree::DrawNode(FColor color, FVector origin, FVector extent)
{
	UWorld *world = GetWorld();

	DrawDebugBox(world, origin, extent, color, true, 120.0f, (uint8)'\000', 20);

	//	BuildTree();
	InsertObjects();
}

void AOctTree::DelayFunction()
{
	isPaused = true;
	UE_LOG(LogTemp, Warning, TEXT("resumed"));
}
	
void AOctTree::InsertObjects()
{
	UE_LOG(LogTemp, Warning, TEXT("Objects left: %d "), objectPosList.Num());

	FTimerHandle UnusedHandle;
	FTimerDelegate TimerDelegate;
	//TimerDelegate.BindUFunction(this, FName("DelayFunction"));

	if (objectPosList.Num() > 0) //&& boundingBox.extent.X > 100.0f)
	{
		while (objectPosList.Num() > 0)
		{
			//bool isPaused = false;
			//GetWorld()->GetTimerManager().SetTimer(UnusedHandle, [this]() {}, 2.0f, false, 2.0f);
			FVector obj = objectPosList.Pop();
			if (ObjectInNodeRange(obj, this->boundingBox))
			{
				if (nodeObjects.Num() > maxObjects)
				{
					objectPosList.Push(obj);

					//Attempt to add delay function
					/*GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &AOctTree::DelayFunction, 2.0f, false);

					while (!isPaused)
					{
						GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &AOctTree::DelayFunction, 1.0f, false);
						UE_LOG(LogTemp, Warning, TEXT("bool:  %d"),isPaused);
					}*/

					BuildTree();

					//isPaused = false;
					objectPosList.Pop();
				}
				else
				{
					SpawnObject(obj);
				}
			}
		}
	}
	else
	{
		return;
	}
}

//Spawns objects after subdividing the octants
void AOctTree::SpawnObject(FVector objLocation)
{
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfo;
	ASceneObject *newObject = GetWorld()->SpawnActor<ASceneObject>(objLocation, Rotation, SpawnInfo);
	newObject->scenePos = objLocation;

	//Store objects in the current node
	nodeObjects.Push(newObject);
}

void AOctTree::SetBoundingBox(FVector origin, FVector extent, int depth)
{
	boundingBox.origin = origin;
	boundingBox.extent = extent;
	boundingBox.depth = depth;
}

FVector AOctTree::GetBoundingBoxOrigin()
{
	return boundingBox.origin;
}

FVector AOctTree::GetBoundingBoxExtent()
{
	return boundingBox.extent;
}

void AOctTree::SetParent()
{
	root = this;
	GenerateSceneObject();
}

void AOctTree:: PrintNodeData()
{
	if (this->children.Num() > 0)
	{
		for (int childCount = 0; childCount < children.Num(); childCount++)
		{
			children[childCount]->PrintNodeData();
		}
	}
	else
	{
		return;
	}	
}

void AOctTree::PrintNode()
{
	AOctTree *firstNode = root;
	firstNode->PrintNodeData();
}

// Called when the game starts or when spawned
void AOctTree::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AOctTree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//Called by the SetParent() of the root node
void AOctTree::GenerateSceneObject()
{
	UE_LOG(LogTemp, Warning, TEXT("Generating scene objects"));

	objectPosList.Push(FVector(1025, 1025, 1025));

	for (int i = 0; i < 30; ++i)
	{
		FVector v = FVector(FMath::RandRange(-1024.0f, 1024.0f), FMath::RandRange(-1024.0f, 1024.0f), FMath::RandRange(-1024.0f, 1024.0f));

		objectPosList.Push(v);
	}
}

int FindMin(const int &a,const int &b)
{
	int min = a < b ? a : b;
	return min;
}

int FindMax(const int &a, const int &b)
{
	int max = a > b ? a : b;
	return max;
}

bool AOctTree::ObjectInNodeRange(FVector object, Node octNode)
{
	
	float nodeMinX = FindMin(octNode.origin.X - octNode.extent.X, octNode.origin.X + octNode.extent.X);
	float nodeMaxX = FindMax(octNode.origin.X - octNode.extent.X, octNode.origin.X + octNode.extent.X);

	float nodeMinY = FindMin(octNode.origin.Y - octNode.extent.Y, octNode.origin.Y + octNode.extent.Y);
	float nodeMaxY = FindMax(octNode.origin.Y - octNode.extent.Y, octNode.origin.Y + octNode.extent.Y);

	float nodeMinZ = FindMin(octNode.origin.Z - octNode.extent.Z, octNode.origin.Z + octNode.extent.Z);
	float nodeMaxZ = FindMax(octNode.origin.Z - octNode.extent.Z, octNode.origin.Z + octNode.extent.Z);

	if( (object.X >= nodeMinX && object.X < nodeMaxX)
		&& (object.Y >= nodeMinY && object.Y < nodeMaxY)
		&& (object.Z >= nodeMinZ && object.Z < nodeMaxZ))
				return true;

	return false;
}

void AOctTree::CollectExistingObjects()
{
		//Add object position to list of obj to be respawned
		objectPosList.Push(nodeObjects[nodeObjects.Num()-1]->scenePos);

		//Remove object from scene
		ASceneObject* a = nodeObjects.Pop(true);

		//a->DrawDebugComponents();
		a->DestroyActor();
}

void AOctTree::BuildTree()
{
		FVector dimension = boundingBox.extent;

		FVector half = dimension / 2;

		FVector center = half;

		Node octNodes[8];

		FColor color = FColor(FMath::RandRange(0, 255), FMath::RandRange(0, 255), 
			FMath::RandRange(0, 255));

		UWorld *world = GetWorld();
	
		//Collect existing objects from undivided node
		if(nodeObjects.Num() > 0)
		while (nodeObjects.Num() > 0)
		{
			FTimerHandle UnusedHandle;
			FTimerDelegate TimerDelegate;
			CollectExistingObjects();
		}


		//return;

		UE_LOG(LogTemp, Warning, TEXT("Building tree"));

		//Top Left back : -x,-y, +z
		octNodes[0].origin = FVector(boundingBox.origin.X - center.X, boundingBox.origin.Y - center.Y,
			boundingBox.origin.Z + center.Z);
		octNodes[0].extent = boundingBox.extent / 2;

		//Top right back: +x,-y,+z
		octNodes[1].origin = FVector(boundingBox.origin.X + center.X, boundingBox.origin.Y - center.Y,
			boundingBox.origin.Z + center.Z);
		octNodes[1].extent = boundingBox.extent / 2;

		//DrawDebugBox(world, octNodes[1].origin, octNodes[1].extent, color, true, 120, (uint8)'\000', 20);
		//UE_LOG(LogTemp, Warning, TEXT("2 box: %f , %f , %f"), octNodes[1].origin.X, octNodes[1].origin.Y, octNodes[1].origin.Z);


		//Top Left front: -x,+y,+z
		octNodes[2].origin = FVector(boundingBox.origin.X - center.X, boundingBox.origin.Y + center.Y,
			boundingBox.origin.Z + center.Z);
		octNodes[2].extent = boundingBox.extent / 2;

		//Top right front: +x,+y,+z
		octNodes[3].origin = FVector(boundingBox.origin.X + center.X, boundingBox.origin.Y + center.Y,
			boundingBox.origin.Z + center.Z);
		octNodes[3].extent = boundingBox.extent / 2;

		//Bottom Left back: -x,-y,-z
		octNodes[4].origin = FVector(boundingBox.origin.X - center.X, boundingBox.origin.Y - center.Y,
			boundingBox.origin.Z - center.Z);
		octNodes[4].extent = boundingBox.extent / 2;

		//Bottom right back: +x,-y,-z
		octNodes[5].origin = FVector(boundingBox.origin.X + center.X, boundingBox.origin.Y - center.Y,
			boundingBox.origin.Z - center.Z);
		octNodes[5].extent = boundingBox.extent / 2;

		//Bottom Left front: -x,+y,-z
		octNodes[6].origin = FVector(boundingBox.origin.X - center.X, boundingBox.origin.Y + center.Y,
			boundingBox.origin.Z - center.Z);
		octNodes[6].extent = boundingBox.extent / 2;

		//Bottom right front: +x,+y,-z
		octNodes[7].origin = FVector(boundingBox.origin.X + center.X, boundingBox.origin.Y + center.Y,
			boundingBox.origin.Z - center.Z);
		octNodes[7].extent = boundingBox.extent / 2;

		int newDepth = boundingBox.depth + int(1);
		for (int i = 0; i < 8; ++i)
		{
			FTimerHandle UnusedHandle;
			FTimerDelegate TimerDelegate;
			color = FColor(FMath::RandRange(0, 255), FMath::RandRange(0, 255), FMath::RandRange(0, 255));
			SpawnOctree(octNodes[i].origin, octNodes[i].extent, newDepth, color);

		}
}