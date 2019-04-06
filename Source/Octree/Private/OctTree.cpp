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
}

//Replaced init method 
void AOctTree::init(FVector const origin, FVector const extent, int const depth, FColor color, AOctTree *rootNode, AOctTree *parentNode, TArray<FVector> objPosList)
{
	PrimaryActorTick.bCanEverTick = false;
	SetBoundingBox(origin, extent,depth);
	minNodeSize = 1;
	maxNodeSize = (extent).X;
	root = rootNode;
	parent = parentNode;
	maxObjects = 11;
	Node currentNode;

	while(objPosList.Num() > 0)
	{
		objectPosList.Push(objPosList.Pop());
	}

	UE_LOG(LogTemp, Warning, TEXT("Init origin: %f , %f , %f"), origin.X, origin.Y, origin.Z);

	UE_LOG(LogTemp, Warning, TEXT("Init extent: %f , %f , %f"), extent.X, extent.Y, extent.Z);

	currentNode.origin = origin;
	currentNode.extent = extent;
	DrawNode(color,origin,extent);
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

void AOctTree::SpawnOctree(FVector spawnLocation, FVector spawnExtent, int depth, FColor c)
{
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfo;


	UE_LOG(LogTemp, Warning, TEXT("Spawn origin: %f , %f , %f"), spawnLocation.X, spawnLocation.Y, spawnLocation.Z);

	UE_LOG(LogTemp, Warning, TEXT("Spawn extent: %f , %f , %f"), spawnExtent.X, spawnExtent.Y, spawnExtent.Z);

	AOctTree *newTree = GetWorld()->SpawnActor<AOctTree>(spawnLocation, Rotation, SpawnInfo);

	newTree->init(spawnLocation, spawnExtent, depth, c, root, this, objectPosList);

	children.Push(newTree);
}

void AOctTree::DrawNode(FColor color, FVector origin, FVector extent)
{
	UWorld *world = GetWorld();
	
	FString s = "Depth";
	s.AppendInt(boundingBox.depth);

	DrawDebugBox(world, origin, extent,color,true, 120.0f,(uint8)'\000',20);

//	BuildTree();
	InsertObjects();
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
	/*
	objectPosList.Push(FVector(-511, -500, -480));

	objectPosList.Push(FVector(511, 500, 480));

	objectPosList.Push(FVector(-511, 500, 480));

	objectPosList.Push(FVector(511, 500, -480));

	objectPosList.Push(FVector(511, 500, -480));

	*/

	objectPosList.Push(FVector(1025, 1025, 1025));

	for (int i = 0; i < 9; ++i)
	{
		FVector v = FVector(FMath::RandRange(-1024.0f, 1024.0f), FMath::RandRange(-1024.0f, 1024.0f), FMath::RandRange(-1024.0f, 1024.0f));

		objectPosList.Push(v);
	}
}

void AOctTree::InsertObjects()
{
	UE_LOG(LogTemp, Warning, TEXT("Objects left: %d "), objectPosList.Num());
	if (objectPosList.Num() > 0 && boundingBox.extent.X > 500.0f)
	{
		while (objectPosList.Num() > 0)
		{
			FVector obj = objectPosList.Pop();
			if (ObjectInNodeRange(obj, this->boundingBox))
			{
				if (nodeObjects.Num() > maxObjects)
				{
					objectPosList.Push(obj);
					BuildTree();
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

	//Store objects in the current node
	nodeObjects.Push(newObject);
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
	
//	UE_LOG(LogTemp, Warning, TEXT("Object node: %f , %f , %f"), object.X, object.Y, object.Z);

	UE_LOG(LogTemp, Warning, TEXT("OctNode origin: %f , %f , %f"), octNode.origin.X, octNode.origin.Y, octNode.origin.Z);

	UE_LOG(LogTemp, Warning, TEXT("OctNode extent range: %f to %f, %f to %f , %f to %f"), octNode.origin.X - octNode.extent.X, octNode.origin.X + octNode.extent.X,
		octNode.origin.Y - octNode.extent.Y, octNode.origin.Y + octNode.extent.Y,
		octNode.origin.Z - octNode.extent.Z, octNode.origin.Z + octNode.extent.Z);

	float nodeMinX = FindMin(octNode.origin.X - octNode.extent.X, octNode.origin.X + octNode.extent.X);
	float nodeMaxX = FindMax(octNode.origin.X - octNode.extent.X, octNode.origin.X + octNode.extent.X);

	float nodeMinY = FindMin(octNode.origin.Y - octNode.extent.Y, octNode.origin.Y + octNode.extent.Y);
	float nodeMaxY = FindMax(octNode.origin.Y - octNode.extent.Y, octNode.origin.Y + octNode.extent.Y);

	float nodeMinZ = FindMin(octNode.origin.Z - octNode.extent.Z, octNode.origin.Z + octNode.extent.Z);
	float nodeMaxZ = FindMax(octNode.origin.Z - octNode.extent.Z, octNode.origin.Z + octNode.extent.Z);

	UE_LOG(LogTemp, Warning, TEXT("OctNode origin x min: %f , max %f"), nodeMinX, nodeMaxX);

	UE_LOG(LogTemp, Warning, TEXT("OctNode origin y min: %f , max %f"), nodeMinY, nodeMaxY);

	UE_LOG(LogTemp, Warning, TEXT("OctNode origin z min: %f , max %f"), nodeMinZ, nodeMaxZ);

	if( (object.X >= nodeMinX && object.X < nodeMaxX)
		&& (object.Y >= nodeMinY && object.Y < nodeMaxY)
		&& (object.Z >= nodeMinZ && object.Z < nodeMaxZ))
				return true;

	return false;
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
		

		if (boundingBox.extent.X/2 < 500)
		{
			//PrintNode();
			//return;
		}

		UE_LOG(LogTemp, Warning, TEXT("Building tree"));


		//Top Left back : -x,-y, +z
		octNodes[0].origin = FVector(boundingBox.origin.X - center.X, boundingBox.origin.Y - center.Y,
			boundingBox.origin.Z + center.Z);
		octNodes[0].extent = boundingBox.extent / 2;

		//DrawDebugBox(world, octNodes[0].origin, octNodes[0].extent, color, true, 120, (uint8)'\000', 20);
		//UE_LOG(LogTemp, Warning, TEXT("1 box: %f , %f , %f"), octNodes[0].origin.X, octNodes[0].origin.Y, octNodes[0].origin.Z);

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

		//DrawDebugBox(world, octNodes[2].origin, octNodes[2].extent, color, true, 120, (uint8)'\000', 20);
		//UE_LOG(LogTemp, Warning, TEXT("3 box: %f , %f , %f"), octNodes[2].origin.X, octNodes[2].origin.Y, octNodes[2].origin.Z);


		//Top right front: +x,+y,+z
		octNodes[3].origin = FVector(boundingBox.origin.X + center.X, boundingBox.origin.Y + center.Y,
			boundingBox.origin.Z + center.Z);
		octNodes[3].extent = boundingBox.extent / 2;

		//DrawDebugBox(world, octNodes[3].origin, octNodes[3].extent, color, true, 120, (uint8)'\000', 20);
		//UE_LOG(LogTemp, Warning, TEXT("4 box: %f , %f , %f"), octNodes[3].origin.X, octNodes[3].origin.Y, octNodes[3].origin.Z);


		//Bottom Left back: -x,-y,-z
		octNodes[4].origin = FVector(boundingBox.origin.X - center.X, boundingBox.origin.Y - center.Y,
			boundingBox.origin.Z - center.Z);
		octNodes[4].extent = boundingBox.extent / 2;

		//DrawDebugBox(world, octNodes[4].origin, octNodes[4].extent, color, true, 120, (uint8)'\000', 20);
		//UE_LOG(LogTemp, Warning, TEXT("5 box: %f , %f , %f"), octNodes[4].origin.X, octNodes[4].origin.Y, octNodes[4].origin.Z);


		//Bottom right back: +x,-y,-z
		octNodes[5].origin = FVector(boundingBox.origin.X + center.X, boundingBox.origin.Y - center.Y,
			boundingBox.origin.Z - center.Z);
		octNodes[5].extent = boundingBox.extent / 2;

		//DrawDebugBox(world, octNodes[5].origin, octNodes[5].extent, color, true, 120, (uint8)'\000',20);
		//UE_LOG(LogTemp, Warning, TEXT("6 box: %f , %f , %f"), octNodes[5].origin.X, octNodes[5].origin.Y, octNodes[5].origin.Z);

		//Bottom Left front: -x,+y,-z
		octNodes[6].origin = FVector(boundingBox.origin.X - center.X, boundingBox.origin.Y + center.Y,
			boundingBox.origin.Z - center.Z);
		octNodes[6].extent = boundingBox.extent / 2;

		//DrawDebugBox(world, octNodes[6].origin, octNodes[6].extent, color, true, 120, (uint8)'\000', 20);
		//UE_LOG(LogTemp, Warning, TEXT("7 box: %f , %f , %f"), octNodes[6].origin.X, octNodes[6].origin.Y, octNodes[6].origin.Z);

		//Bottom right front: +x,+y,-z
		octNodes[7].origin = FVector(boundingBox.origin.X + center.X, boundingBox.origin.Y + center.Y,
			boundingBox.origin.Z - center.Z);
		octNodes[7].extent = boundingBox.extent / 2;

		//DrawDebugBox(world, octNodes[7].origin, octNodes[7].extent, color, true, 120, (uint8)'\000', 20);
		//UE_LOG(LogTemp, Warning, TEXT("8 box: %f , %f , %f"), octNodes[7].origin.X, octNodes[7].origin.Y, octNodes[7].origin.Z);

		int newDepth = boundingBox.depth + int(1);

		for (int j = 0; j < objectPosList.Num(); ++j)
		{		
			for (int i = 0; i < 8; ++i)
			{
				if (objectPosList.Num() > 0)
					if(ObjectInNodeRange(objectPosList[j], octNodes[i]))
					{
						UE_LOG(LogTemp, Warning, TEXT("In region"));
						FVector Location(octNodes[i].origin);
						FRotator Rotation(0.0f, 0.0f, 0.0f);
						FActorSpawnParameters SpawnInfo;
						FTimerHandle UnusedHandle;
						FTimerDelegate TimerDelegate;

						UE_LOG(LogTemp, Warning, TEXT("OctNode origin: %f , %f , %f"), octNodes[i].origin.X, octNodes[i].origin.Y, octNodes[i].origin.Z);

						UE_LOG(LogTemp, Warning, TEXT("OctNode extent: %f , %f , %f"), octNodes[i].extent.X, octNodes[i].extent.Y, octNodes[i].extent.Z);

						UE_LOG(LogTemp, Warning, TEXT("Object node: %f , %f , %f"), objectPosList[j].X, objectPosList[j].Y, objectPosList[j].Z);

						color = FColor(FMath::RandRange(0, 255), FMath::RandRange(0, 255),
							FMath::RandRange(0, 255));

						/*TimerDelegate.BindUFunction(this, FName("SpawnOctree"),
							octNodes[i].origin, octNodes[i].extent, newDepth, color);
						GetWorldTimerManager().SetTimer(UnusedHandle, TimerDelegate, 2.0f, false);*/
					
						SpawnOctree(octNodes[i].origin, octNodes[i].extent, newDepth, color);

						/*if (nodeObjects.Num() > 0)
						{
							InsertObjects();
						}*/
						
					}
			}
		}

}